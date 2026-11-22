#include "HeatTransfer/Solver/Smoothers/Jacobi.h"

#include <tracy/Tracy.hpp>

#include "HeatTransfer/Solver/Discretisation/Stencil.h"

namespace HeatTransfer::Solver::Smoothers
{
    void Jacobi::Smooth(Core::Tensor3d& matrix,
                        const Core::Tensor3d& rhs,
                        const SystemMatrix& system,
                        [[maybe_unused]] SweepDirection direction) const
    {
        ZoneScoped;

        const Eigen::Index rows = matrix.dimension(0);
        const Eigen::Index cols = matrix.dimension(1);
        const Eigen::Index layers = matrix.dimension(2);

        // Every point is updated from the old values, so the sweep is already symmetric and the
        // direction does not matter
        const Core::Tensor3d old = matrix;
        const auto& fixedTemperatureMask = system.GetFixedTemperatureMask();

#pragma omp parallel for collapse(2) schedule(static)
        for (Eigen::Index k = 0; k < layers; k++)
        {
            for (Eigen::Index j = 0; j < cols; j++)
            {
                for (Eigen::Index i = 0; i < rows; i++)
                {
                    if (fixedTemperatureMask(i, j, k))
                        continue;

                    matrix(i, j, k) =
                        (1 - DAMPING_FACTOR) * old(i, j, k) +
                        DAMPING_FACTOR * Stencil::LocalSolve(old, rhs, system, i, j, k);
                }
            }
        }
    }
}
