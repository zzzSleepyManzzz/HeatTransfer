#include "HeatTransfer/Solver/MultigridPreconditioner.h"

#include <array>

#include <tracy/Tracy.hpp>

#include "HeatTransfer/Core/Tensor3dMapper.h"

#include "HeatTransfer/Solver/Discretisation/Stencil.h"

namespace HeatTransfer::Solver
{
    namespace
    {
        Eigen::Index CoarseSize(Eigen::Index n)
        {
            return n / 2 + 1;
        }

        Core::Tensor3d::Dimensions CoarseDimensions(const Core::Tensor3d& matrix)
        {
            return Core::Tensor3d::Dimensions(CoarseSize(matrix.dimension(0)),
                                              CoarseSize(matrix.dimension(1)),
                                              CoarseSize(matrix.dimension(2)));
        }

        Core::Tensor3d Restrict(const Core::Tensor3d& matrix)
        {
            // Full weighting is the tensor product of the 1D weights 1/4 * [1, 2, 1], i.e. the
            // 27-point stencil with weights 1/64 * {8, 4, 2, 1} for the centre, faces, edges and
            // corners. Fine cells outside the domain are skipped, which makes restriction exactly
            // 1/8 times the transpose of trilinear prolongation, so the V-cycle stays symmetric

            ZoneScoped;

            constexpr std::array<double, 3> weights = {0.25, 0.5, 0.25};

            const Eigen::Index rows = matrix.dimension(0);
            const Eigen::Index cols = matrix.dimension(1);
            const Eigen::Index layers = matrix.dimension(2);

            Core::Tensor3d restricted(CoarseDimensions(matrix));

            const Eigen::Index coarseRows = restricted.dimension(0);
            const Eigen::Index coarseCols = restricted.dimension(1);
            const Eigen::Index coarseLayers = restricted.dimension(2);

#pragma omp parallel for collapse(2) schedule(static)
            for (Eigen::Index k = 0; k < coarseLayers; k++)
            {
                for (Eigen::Index j = 0; j < coarseCols; j++)
                {
                    for (Eigen::Index i = 0; i < coarseRows; i++)
                    {
                        double sum = 0.0;

                        for (int dk = -1; dk <= 1; dk++)
                        {
                            const Eigen::Index K = 2 * k + dk;

                            if (K < 0 || K >= layers)
                                continue;

                            for (int dj = -1; dj <= 1; dj++)
                            {
                                const Eigen::Index J = 2 * j + dj;

                                if (J < 0 || J >= cols)
                                    continue;

                                for (int di = -1; di <= 1; di++)
                                {
                                    const Eigen::Index I = 2 * i + di;

                                    if (I < 0 || I >= rows)
                                        continue;

                                    sum += weights[dk + 1] * weights[dj + 1] * weights[di + 1] *
                                           matrix(I, J, K);
                                }
                            }
                        }

                        restricted(i, j, k) = sum;
                    }
                }
            }

            return restricted;
        }

        Core::Tensor3d Prolongate(const Core::Tensor3d& matrix,
                                  const Core::Tensor3d::Dimensions& fineDimensions)
        {
            // Trilinear interpolation: each fine cell is the average of the (up to 8) coarse cells
            // around it. On even indices both corners in a direction coincide, which reduces to
            // injection where the grids coincide

            ZoneScoped;

            Core::Tensor3d prolongated(fineDimensions);

            const Eigen::Index rows = fineDimensions[0];
            const Eigen::Index cols = fineDimensions[1];
            const Eigen::Index layers = fineDimensions[2];

#pragma omp parallel for collapse(2) schedule(static)
            for (Eigen::Index k = 0; k < layers; k++)
            {
                for (Eigen::Index j = 0; j < cols; j++)
                {
                    const Eigen::Index K0 = k / 2;
                    const Eigen::Index K1 = (k % 2 == 0) ? K0 : K0 + 1;
                    const Eigen::Index J0 = j / 2;
                    const Eigen::Index J1 = (j % 2 == 0) ? J0 : J0 + 1;

                    for (Eigen::Index i = 0; i < rows; i++)
                    {
                        const Eigen::Index I0 = i / 2;
                        const Eigen::Index I1 = (i % 2 == 0) ? I0 : I0 + 1;

                        prolongated(i, j, k) =
                            0.125 * (matrix(I0, J0, K0) + matrix(I1, J0, K0) + matrix(I0, J1, K0) +
                                     matrix(I1, J1, K0) + matrix(I0, J0, K1) + matrix(I1, J0, K1) +
                                     matrix(I0, J1, K1) + matrix(I1, J1, K1));
                    }
                }
            }

            return prolongated;
        }

        Core::Tensor3d CoarsenMask(const Core::Tensor3d& mask)
        {
            // A coarse cell is fixed if any fine cell it restricts from is fixed. Faces that are
            // adiabatic on the fine grid stay adiabatic on the coarse grid
            Core::Tensor3d coarseMask = Restrict(mask);

            auto M = Core::Tensor3dMapper::AsVector(coarseMask);
            M = (M.array() > 0.0).cast<double>();

            return coarseMask;
        }

        SystemMatrix CoarsenSystem(const SystemMatrix& fine)
        {
            // Bi = h_coeff * h / k_coeff, and the coarse grid spacing is 2 h
            auto biotNumbers = fine.GetBiotNumbers();

            for (auto& biotNumber : biotNumbers)
                biotNumber *= 2.0;

            auto coarseMask = CoarsenMask(fine.GetFixedTemperatureMask());

            return SystemMatrix(coarseMask, biotNumbers);
        }
    }

    MultigridPreconditioner::MultigridPreconditioner(
        std::shared_ptr<const Smoothers::ISmoother> smoother)
        : _smoother(std::move(smoother))
    {
    }

    bool MultigridPreconditioner::CanCoarsen(const Core::Tensor3d& matrix) const
    {
        const auto coarse = CoarseDimensions(matrix);

        return coarse[0] >= MIN_COARSE_SIZE && coarse[1] >= MIN_COARSE_SIZE &&
               coarse[2] >= MIN_COARSE_SIZE;
    }

    void MultigridPreconditioner::BuildLevels(const SystemMatrix& system)
    {
        ZoneScoped;

        _errorStack = {Core::Tensor3d()};
        _residualStack = {Core::Tensor3d()};
        _systemStack = {system};

        while (CanCoarsen(_systemStack.back().GetFixedTemperatureMask()))
        {
            _systemStack.push_back(CoarsenSystem(_systemStack.back()));

            const auto dimensions = _systemStack.back().GetFixedTemperatureMask().dimensions();

            _errorStack.emplace_back(dimensions);
            _errorStack.back().setZero();

            _residualStack.emplace_back(dimensions);
            _residualStack.back().setZero();
        }
    }

    void MultigridPreconditioner::Precondition(Core::Tensor3d& result, const Core::Tensor3d& rhs)
    {
        // One V-cycle gives an approximation of the solution to H * x = b
        // where H is the Hessian of f(x) = 0.5 * x^T H x - b^T x

        result.resize(rhs.dimensions());
        result.setZero();
        VCycle(result, rhs);
    }

    void MultigridPreconditioner::VCycle(Core::Tensor3d& matrix, const Core::Tensor3d& rhs)
    {
        ZoneScoped;

        const auto numLevels = _systemStack.size();

        // The finest level uses the matrix and rhs passed in, the coarser levels use the stacks
        auto levelMatrix = [&](std::size_t level) -> Core::Tensor3d&
        { return level == 0 ? matrix : _errorStack[level]; };

        auto levelRhs = [&](std::size_t level) -> const Core::Tensor3d&
        { return level == 0 ? rhs : _residualStack[level]; };

        // Down V-Cycle

        for (std::size_t i = 0; i < numLevels - 1; i++)
        {
            auto& E = levelMatrix(i);

            for (auto s = 0; s < PRE_SMOOTHING_SWEEPS; s++)
                _smoother->Smooth(
                    E, levelRhs(i), _systemStack[i], Smoothers::SweepDirection::FORWARD);

            auto& R = _residualStack[i + 1];

            // The equations are scaled by h^2 / 6 (per unit volume), and the coarse grid spacing
            // is 2h, so the restricted residual is multiplied by 2^2 = 4
            R = Restrict(Stencil::ComputeResidual(E, levelRhs(i), _systemStack[i])) * 4.0;
            Stencil::ZeroFixedCells(R, _systemStack[i + 1].GetFixedTemperatureMask());

            _errorStack[i + 1].setZero();
        }

        // Solve coarsest grid

        if (numLevels > 1)
            SolveCoarsest();
        else
        {
            _smoother->Smooth(matrix, rhs, _systemStack[0], Smoothers::SweepDirection::FORWARD);
            _smoother->Smooth(matrix, rhs, _systemStack[0], Smoothers::SweepDirection::BACKWARD);
        }

        // Up V-Cycle

        for (auto i = numLevels - 1; i > 0; i--)
        {
            auto& E = levelMatrix(i - 1);

            Core::Tensor3d correction = Prolongate(_errorStack[i], E.dimensions());
            Stencil::ZeroFixedCells(correction, _systemStack[i - 1].GetFixedTemperatureMask());

            Core::Tensor3dMapper::AsVector(E) += Core::Tensor3dMapper::AsVector(correction);

            // Post-smoothing sweeps backward so that it is the transpose of pre-smoothing
            for (auto s = 0; s < POST_SMOOTHING_SWEEPS; s++)
                _smoother->Smooth(
                    E, levelRhs(i - 1), _systemStack[i - 1], Smoothers::SweepDirection::BACKWARD);
        }
    }

    void MultigridPreconditioner::SolveCoarsest()
    {
        auto& E = _errorStack.back();
        const auto& R = _residualStack.back();
        const auto& system = _systemStack.back();

        // Solving H * E = R, where H is the Hessian of f(E) = 0.5 * E^T H E - R^T E
        // The Hessian must remain symmetric positive definite (SPD) for the conjugate gradient
        // method to converge. Alternating sweeps keeps the coarse solve symmetric
        for (int s = 0; s < COARSE_SWEEPS / 2; s++)
        {
            _smoother->Smooth(E, R, system, Smoothers::SweepDirection::FORWARD);
            _smoother->Smooth(E, R, system, Smoothers::SweepDirection::BACKWARD);
        }
    }
}
