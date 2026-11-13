#include "HeatTransfer/Solver/MultigridPreconditioner.h"

#include <tracy/Tracy.hpp>

#include "HeatTransfer/Solver/Stencil.h"

namespace HeatTransfer::Solver
{
    namespace
    {
        Eigen::Index CoarseSize(Eigen::Index n)
        {
            return n / 2 + 1;
        }

        Eigen::MatrixXd Restrict(const Eigen::MatrixXd& matrix)
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
                        (4.0 * M(I, J) +
                         2.0 * (M(I - 1, J) + M(I + 1, J) + M(I, J - 1) + M(I, J + 1)) +
                         M(I - 1, J - 1) + M(I - 1, J + 1) + M(I + 1, J - 1) + M(I + 1, J + 1)) /
                        16.0;
                }
            }

            return restrictedMatrix;
        }

        Eigen::MatrixXd
        Prolongate(const Eigen::MatrixXd& matrix, Eigen::Index rows, Eigen::Index cols)
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

        Eigen::MatrixXd CoarsenMask(const Eigen::MatrixXd& mask)
        {
            Eigen::MatrixXd coarseMask = (Restrict(mask).array() > 0.0).cast<double>();

            coarseMask.row(0).setConstant(1.0);
            coarseMask.row(coarseMask.rows() - 1).setConstant(1.0);
            coarseMask.col(0).setConstant(1.0);
            coarseMask.col(coarseMask.cols() - 1).setConstant(1.0);

            return coarseMask;
        }
    }

    MultigridPreconditioner::MultigridPreconditioner(
        std::shared_ptr<const Smoothers::ISmoother> smoother)
        : _smoother(std::move(smoother))
    {
    }

    void MultigridPreconditioner::BuildLevels(const Eigen::MatrixXd& insulated)
    {
        ZoneScoped;

        _errorStack = {Eigen::MatrixXd()};
        _residualStack = {Eigen::MatrixXd()};
        _insulatedStack = {insulated};

        while (CoarseSize(_insulatedStack.back().rows()) >= MIN_COARSE_SIZE &&
               CoarseSize(_insulatedStack.back().cols()) >= MIN_COARSE_SIZE)
        {
            _insulatedStack.push_back(CoarsenMask(_insulatedStack.back()));

            const auto& mask = _insulatedStack.back();
            const auto coarseRows = mask.rows();
            const auto coarseCols = mask.cols();

            _errorStack.push_back(Eigen::MatrixXd::Zero(coarseRows, coarseCols));
            _residualStack.push_back(Eigen::MatrixXd::Zero(coarseRows, coarseCols));
        }
    }

    void MultigridPreconditioner::Precondition(Eigen::MatrixXd& result, const Eigen::MatrixXd& rhs)
    {
        // One V-cycle gives an approximation of the solution to H * x = b
        // where H is the Hessian of f(x) = 0.5 * x^T H x - b^T x

        result = Eigen::MatrixXd::Zero(rhs.rows(), rhs.cols());
        VCycle(result, rhs);
    }

    void MultigridPreconditioner::VCycle(Eigen::MatrixXd& matrix, const Eigen::MatrixXd& rhs)
    {
        ZoneScoped;

        const auto numLevels = _insulatedStack.size();

        // The finest level uses the matrix and rhs passed in, the coarser levels use the stacks
        auto levelMatrix = [&](std::size_t level) -> Eigen::MatrixXd&
        { return level == 0 ? matrix : _errorStack[level]; };

        auto levelRhs = [&](std::size_t level) -> const Eigen::MatrixXd&
        { return level == 0 ? rhs : _residualStack[level]; };

        // Down V-Cycle

        for (std::size_t i = 0; i < numLevels - 1; i++)
        {
            auto& E = levelMatrix(i);

            for (auto s = 0; s < PRE_SMOOTHING_SWEEPS; s++)
                _smoother->Smooth(
                    E, levelRhs(i), _insulatedStack[i], Smoothers::SweepDirection::FORWARD);

            auto& R = _residualStack[i + 1];

            // Correction factor of 4.0 is used to account for the fact that the coarse grid has
            // half the resolution of the fine grid
            R = 4.0 * Restrict(Stencil::ComputeResidual(E, levelRhs(i), _insulatedStack[i]));
            R.array() = _insulatedStack[i + 1].array().select(0.0, R.array());

            _errorStack[i + 1].setZero();
        }

        // Solve coarsest grid

        if (numLevels > 1)
            SolveCoarsest();
        else
        {
            _smoother->Smooth(matrix, rhs, _insulatedStack[0], Smoothers::SweepDirection::FORWARD);
            _smoother->Smooth(matrix, rhs, _insulatedStack[0], Smoothers::SweepDirection::BACKWARD);
        }

        // Up V-Cycle

        for (auto i = numLevels - 1; i > 0; i--)
        {
            auto& E = levelMatrix(i - 1);

            Eigen::MatrixXd correction = Prolongate(_errorStack[i], E.rows(), E.cols());
            correction.array() = _insulatedStack[i - 1].array().select(0.0, correction.array());

            E += correction;

            // Post-smoothing sweeps backward so that it is the transpose of pre-smoothing
            for (auto s = 0; s < POST_SMOOTHING_SWEEPS; s++)
                _smoother->Smooth(E,
                                  levelRhs(i - 1),
                                  _insulatedStack[i - 1],
                                  Smoothers::SweepDirection::BACKWARD);
        }
    }

    void MultigridPreconditioner::SolveCoarsest()
    {
        auto& E = _errorStack.back();
        const auto& R = _residualStack.back();
        const auto& insulated = _insulatedStack.back();

        // Solving H * E = R, where H is the Hessian of f(E) = 0.5 * E^T H E - R^T E
        // The Hessian must remain symmetric positive definite (SPD) for the conjugate gradient
        // method to converge. Alternating sweeps keeps the coarse solve symmetric
        for (int s = 0; s < COARSE_SWEEPS / 2; s++)
        {
            _smoother->Smooth(E, R, insulated, Smoothers::SweepDirection::FORWARD);
            _smoother->Smooth(E, R, insulated, Smoothers::SweepDirection::BACKWARD);
        }
    }
}
