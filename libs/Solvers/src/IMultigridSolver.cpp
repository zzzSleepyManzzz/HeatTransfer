#include "HeatTransfer/Solvers/IMultigridSolver.h"

#include <tracy/Tracy.hpp>

namespace HeatTransfer::Solvers
{
    IMultigridSolver::IMultigridSolver(const Core::SimulationParameters& parameters,
                                       const Core::MaterialProperties& materialProperties,
                                       const Core::BoundaryConditions& boundaryCondition)
        : ISolver(parameters, materialProperties, boundaryCondition)
    {
    }

    void IMultigridSolver::BuildLevels()
    {
        ZoneScoped;

        const auto rows = _temperatureMatrix->rows();
        const auto cols = _temperatureMatrix->cols();

        auto insulated = Eigen::MatrixXd(rows, cols);

        for (Eigen::Index j = 0; j < cols; j++)
        {
            for (Eigen::Index i = 0; i < rows; i++)
            {
                insulated(i, j) = IsInsulated(i, j, rows, cols);
            }
        }

        auto q_g = _materialProperties.InternalHeatGeneration;
        auto k = _materialProperties.ThermalConductivity;
        auto h = 1.0 / std::max(rows, cols);

        Eigen::MatrixXd heating =
            Eigen::MatrixXd::Constant(rows, cols, 0.25f * pow(h, 2) * q_g / k);

        heating.array() = insulated.array().select(0.0, heating.array());

        _errorStack = {Eigen::MatrixXd()}; // Never touched - empty matrix
        _residualStack = {heating};
        _insulatedStack = {insulated};

        while (CoarseSize(_insulatedStack.back().rows()) >= MIN_COARSE_SIZE &&
               CoarseSize(_insulatedStack.back().cols()) >= MIN_COARSE_SIZE)
        {
            _insulatedStack.push_back(CoarsenMask(_insulatedStack.back()));

            const auto& mask = _insulatedStack.back();
            const auto courseRows = mask.rows();
            const auto courseCols = mask.cols();

            _errorStack.push_back(Eigen::MatrixXd::Zero(courseRows, courseCols));
            _residualStack.push_back(Eigen::MatrixXd::Zero(courseRows, courseCols));
        }
    }

    Eigen::MatrixXd IMultigridSolver::ComputeResidual(const Eigen::MatrixXd& matrix,
                                                      const Eigen::MatrixXd& rhs,
                                                      const Eigen::MatrixXd& insulated)
    {
        const Eigen::Index rows = matrix.rows();
        const Eigen::Index cols = matrix.cols();

        const auto& E = matrix;
        Eigen::MatrixXd R = Eigen::MatrixXd::Zero(rows, cols);

        R.block(1, 1, rows - 2, cols - 2) =
            rhs.block(1, 1, rows - 2, cols - 2) - E.block(1, 1, rows - 2, cols - 2) +
            (E.block(0, 1, rows - 2, cols - 2) + E.block(2, 1, rows - 2, cols - 2) +
             E.block(1, 0, rows - 2, cols - 2) + E.block(1, 2, rows - 2, cols - 2)) *
                0.25;

        R.array() = insulated.array().select(0.0, R.array());
        return R;
    }

    Eigen::MatrixXd& IMultigridSolver::LevelMatrix(std::size_t level)
    {
        return level == 0 ? *_temperatureMatrix : _errorStack[level];
    }

    void IMultigridSolver::VCycle()
    {
        ZoneScoped;

        const auto numLevels = _insulatedStack.size();

        // Down V-Cycle

        for (auto i = 0; i < numLevels - 1; i++)
        {
            auto& E = LevelMatrix(i);

            for (auto s = 0; s < PRE_SMOOTHING_SWEEPS; s++)
                Smoothen(E, _residualStack[i], _insulatedStack[i]);

            auto& R = _residualStack[i + 1];

            // Correction factor of 4.0 is used to account for the fact that the coarse grid has
            // half the resolution of the fine grid
            R = 4.0 * Restrict(ComputeResidual(E, _residualStack[i], _insulatedStack[i]));
            R.array() = _insulatedStack[i + 1].array().select(0.0, R.array());

            _errorStack[i + 1].setZero();
        }

        // Solve coarsest grid

        if (numLevels > 1)
            SolveCoarsest();
        else
            Smoothen(*_temperatureMatrix, _residualStack[0], _insulatedStack[0]);

        // Up V-Cycle

        for (auto i = numLevels - 1; i > 0; i--)
        {
            auto& E = LevelMatrix(i - 1);

            Eigen::MatrixXd correction = Prolongate(_errorStack[i], E.rows(), E.cols());
            correction.array() = _insulatedStack[i - 1].array().select(0.0, correction.array());

            E += correction;

            for (auto s = 0; s < POST_SMOOTHING_SWEEPS; s++)
                Smoothen(E, _residualStack[i - 1], _insulatedStack[i - 1]);
        }
    }

    void IMultigridSolver::SolveCoarsest()
    {
        auto& E = _errorStack.back();
        const auto& R = _residualStack.back();
        const auto& insulated = _insulatedStack.back();

        for (int s = 0; s < COARSE_SWEEPS; s++)
        {
            Smoothen(E, R, insulated);
        }
    }

    Eigen::Index IMultigridSolver::CoarseSize(Eigen::Index n)
    {
        return n / 2 + 1;
    }

    Eigen::MatrixXd IMultigridSolver::CoarsenMask(const Eigen::MatrixXd& mask)
    {
        Eigen::MatrixXd coarseMask = (Restrict(mask).array() > 0.0).cast<double>();

        coarseMask.row(0).setConstant(1.0);
        coarseMask.row(coarseMask.rows() - 1).setConstant(1.0);
        coarseMask.col(0).setConstant(1.0);
        coarseMask.col(coarseMask.cols() - 1).setConstant(1.0);

        return coarseMask;
    }

    Eigen::MatrixXd IMultigridSolver::Restrict(const Eigen::MatrixXd& matrix)
    {
        // The standard weighting is given by:
        //
        // 1/16 * [1, 2, 1]
        //        [2, 4, 2]
        //        [1, 2, 1]

        ZoneScoped;

        const Eigen::Index rows = matrix.rows();
        const Eigen::Index cols = matrix.cols();

        Eigen::MatrixXd restrictedMatrix =
            Eigen::MatrixXd::Zero(CoarseSize(rows), CoarseSize(cols));

        auto M = [&](Eigen::Index I, Eigen::Index J)
        { return (I < rows && J < cols) ? matrix(I, J) : 0.0; };

        for (Eigen::Index j = 1; j < restrictedMatrix.cols() - 1; j++)
        {
            for (Eigen::Index i = 1; i < restrictedMatrix.rows() - 1; i++)
            {
                const auto I = 2 * i;
                const auto J = 2 * j;

                restrictedMatrix(i, j) =
                    (4.0 * M(I, J) + 2.0 * (M(I - 1, J) + M(I + 1, J) + M(I, J - 1) + M(I, J + 1)) +
                     M(I - 1, J - 1) + M(I - 1, J + 1) + M(I + 1, J - 1) + M(I + 1, J + 1)) /
                    16.0;
            }
        }

        return restrictedMatrix;
    }

    Eigen::MatrixXd IMultigridSolver::Prolongate(const Eigen::MatrixXd& matrix,
                                                 Eigen::Index rows,
                                                 Eigen::Index cols)
    {
        ZoneScoped;

        Eigen::MatrixXd prolongatedMatrix(rows, cols);

        for (Eigen::Index j = 0; j < cols; j++)
        {
            const auto J0 = j / 2;
            const auto J1 = (j % 2 == 0) ? J0 : J0 + 1;

            for (Eigen::Index i = 0; i < rows; i++)
            {
                const auto I0 = i / 2;
                const auto I1 = (i % 2 == 0) ? I0 : I0 + 1;

                prolongatedMatrix(i, j) =
                    0.25 * (matrix(I0, J0) + matrix(I1, J0) + matrix(I0, J1) + matrix(I1, J1));
            }
        }

        return prolongatedMatrix;
    }
}
