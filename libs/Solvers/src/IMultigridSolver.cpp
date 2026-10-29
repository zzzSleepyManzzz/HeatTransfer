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
        const Eigen::Index rows = _temperatureMatrix->rows();
        const Eigen::Index cols = _temperatureMatrix->cols();

        InsulatedMask insulated(rows, cols);

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

        Eigen::MatrixXd heating = Eigen::MatrixXd::Constant(rows, cols, 0.25 * pow(h, 2) * q_g / k);
        heating.array() = insulated.select(0.0, heating.array());

        _errorStack = {Eigen::MatrixXd()};
        _residualStack = {heating};
        _insulatedStack = {insulated};

        while (CoarseSize(_insulatedStack.back().rows()) >= MIN_COARSE_SIZE &&
               CoarseSize(_insulatedStack.back().cols()) >= MIN_COARSE_SIZE)
        {
            _insulatedStack.push_back(CoarsenMask(_insulatedStack.back()));

            const auto& mask = _insulatedStack.back();

            _errorStack.push_back(Eigen::MatrixXd::Zero(mask.rows(), mask.cols()));
            _residualStack.push_back(Eigen::MatrixXd::Zero(mask.rows(), mask.cols()));
        }
    }

    Eigen::MatrixXd& IMultigridSolver::LevelMatrix(std::size_t level)
    {
        return level == 0 ? *_temperatureMatrix : _errorStack[level];
    }

    void IMultigridSolver::VCycle()
    {
        ZoneScoped;

        const std::size_t numLevels = _insulatedStack.size();

        // Down: smooth, then restrict the residual to the next level. The equation is
        // scaled by h^2 / 4 and h doubles, so the coarse right-hand side is 4x the residual.
        for (std::size_t i = 0; i + 1 < numLevels; i++)
        {
            auto& E = LevelMatrix(i);

            for (int s = 0; s < PRE_SMOOTHING_SWEEPS; s++)
                Smoothen(E, _residualStack[i], _insulatedStack[i]);

            auto& R = _residualStack[i + 1];
            R = 4.0 * Restrict(ComputeResidual(E, _residualStack[i], _insulatedStack[i]));
            R.array() = _insulatedStack[i + 1].select(0.0, R.array());

            _errorStack[i + 1].setZero();
        }

        if (numLevels > 1)
            SolveCoarsest();
        else
            Smoothen(*_temperatureMatrix, _residualStack[0], _insulatedStack[0]);

        // Up: add the interpolated correction (never on insulated nodes), then smooth
        for (std::size_t i = numLevels - 1; i-- > 0;)
        {
            auto& E = LevelMatrix(i);

            Eigen::MatrixXd correction = Prolongate(_errorStack[i + 1], E.rows(), E.cols());
            correction.array() = _insulatedStack[i].select(0.0, correction.array());
            E += correction;

            for (int s = 0; s < POST_SMOOTHING_SWEEPS; s++)
                Smoothen(E, _residualStack[i], _insulatedStack[i]);
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

    ISolver::InsulatedMask IMultigridSolver::CoarsenMask(const InsulatedMask& mask)
    {
        const Eigen::Index rows = CoarseSize(mask.rows());
        const Eigen::Index cols = CoarseSize(mask.cols());

        InsulatedMask coarse = InsulatedMask::Constant(rows, cols, false);
        coarse.row(0).setConstant(true);
        coarse.row(rows - 1).setConstant(true);
        coarse.col(0).setConstant(true);
        coarse.col(cols - 1).setConstant(true);

        for (Eigen::Index J = 1; J < cols - 1; J++)
        {
            for (Eigen::Index I = 1; I < rows - 1; I++)
            {
                const Eigen::Index numRows = std::min<Eigen::Index>(3, mask.rows() - (2 * I - 1));
                const Eigen::Index numCols = std::min<Eigen::Index>(3, mask.cols() - (2 * J - 1));

                coarse(I, J) = mask.block(2 * I - 1, 2 * J - 1, numRows, numCols).any();
            }
        }

        return coarse;
    }

    // Full weighting: 1/16 [1 2 1; 2 4 2; 1 2 1] around fine node (2I, 2J)
    Eigen::MatrixXd IMultigridSolver::Restrict(const Eigen::MatrixXd& matrix)
    {
        ZoneScoped;

        const Eigen::Index rows = matrix.rows();
        const Eigen::Index cols = matrix.cols();

        Eigen::MatrixXd restricted = Eigen::MatrixXd::Zero(CoarseSize(rows), CoarseSize(cols));

        // Fine nodes past the edge (even sizes only) count as 0
        auto M = [&](Eigen::Index i, Eigen::Index j)
        { return (i < rows && j < cols) ? matrix(i, j) : 0.0; };

        for (Eigen::Index J = 1; J < restricted.cols() - 1; J++)
        {
            for (Eigen::Index I = 1; I < restricted.rows() - 1; I++)
            {
                const Eigen::Index i = 2 * I;
                const Eigen::Index j = 2 * J;

                restricted(I, J) =
                    (4.0 * M(i, j) + 2.0 * (M(i - 1, j) + M(i + 1, j) + M(i, j - 1) + M(i, j + 1)) +
                     M(i - 1, j - 1) + M(i - 1, j + 1) + M(i + 1, j - 1) + M(i + 1, j + 1)) /
                    16.0;
            }
        }

        return restricted;
    }

    Eigen::MatrixXd IMultigridSolver::Prolongate(const Eigen::MatrixXd& matrix,
                                                 Eigen::Index rows,
                                                 Eigen::Index cols)
    {
        ZoneScoped;

        Eigen::MatrixXd prolongated(rows, cols);

        for (Eigen::Index j = 0; j < cols; j++)
        {
            const Eigen::Index J0 = j / 2;
            const Eigen::Index J1 = (j % 2 == 0) ? J0 : J0 + 1;

            for (Eigen::Index i = 0; i < rows; i++)
            {
                const Eigen::Index I0 = i / 2;
                const Eigen::Index I1 = (i % 2 == 0) ? I0 : I0 + 1;

                prolongated(i, j) =
                    0.25 * (matrix(I0, J0) + matrix(I1, J0) + matrix(I0, J1) + matrix(I1, J1));
            }
        }

        return prolongated;
    }
}