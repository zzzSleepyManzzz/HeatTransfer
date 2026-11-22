#include "HeatTransfer/Solver/Smoothers/SuccessiveOverRelaxation.h"

#include <tracy/Tracy.hpp>

#include "HeatTransfer/Solver/Discretisation/Stencil.h"

namespace HeatTransfer::Solver::Smoothers
{
    SuccessiveOverRelaxation::SuccessiveOverRelaxation(double relaxationFactor)
        : _relaxationFactor(relaxationFactor)
    {
    }

    void SuccessiveOverRelaxation::Smooth(Core::Tensor3d& matrix,
                                          const Core::Tensor3d& rhs,
                                          const SystemMatrix& system,
                                          SweepDirection direction) const
    {
        ZoneScoped;

        const Eigen::Index rows = matrix.dimension(0);
        const Eigen::Index cols = matrix.dimension(1);
        const Eigen::Index layers = matrix.dimension(2);

        auto& T = matrix;
        const auto& fixedTemperatureMask = system.GetFixedTemperatureMask();

        // A backward sweep is the transpose of a forward sweep, so pairing them keeps the
        // V-cycle symmetric
        const bool forward = direction == SweepDirection::FORWARD;

        for (Eigen::Index l = 0; l < layers; l++)
        {
            const Eigen::Index k = forward ? l : layers - 1 - l;

            for (Eigen::Index n = 0; n < cols; n++)
            {
                const Eigen::Index j = forward ? n : cols - 1 - n;

                for (Eigen::Index m = 0; m < rows; m++)
                {
                    const Eigen::Index i = forward ? m : rows - 1 - m;

                    if (fixedTemperatureMask(i, j, k))
                        continue;

                    T(i, j, k) = (1 - _relaxationFactor) * T(i, j, k) +
                                 _relaxationFactor * Stencil::LocalSolve(T, rhs, system, i, j, k);
                }
            }
        }
    }
}
