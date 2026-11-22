#include "HeatTransfer/Solver/Smoothers/RedBlackSOR.h"

#include <array>

#include <tracy/Tracy.hpp>

#include "HeatTransfer/Solver/Discretisation/Stencil.h"

namespace HeatTransfer::Solver::Smoothers
{
    RedBlackSOR::RedBlackSOR(double relaxationFactor) : _relaxationFactor(relaxationFactor) {}

    void RedBlackSOR::Smooth(Core::Tensor3d& matrix,
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

        auto RED = 0;
        auto BLACK = 1;

        // Reversing the colour order gives the transpose of the sweep, so pairing them keeps the
        // V-cycle symmetric
        const auto colours =
            direction == SweepDirection::FORWARD ? std::array{RED, BLACK} : std::array{BLACK, RED};

        for (auto colour : colours)
        {
#pragma omp parallel for collapse(2) schedule(static)
            for (Eigen::Index k = 0; k < layers; k++)
            {
                for (Eigen::Index j = 0; j < cols; j++)
                {
                    const Eigen::Index start = (j + k + colour) % 2;

                    for (Eigen::Index i = start; i < rows; i += 2)
                    {
                        if (fixedTemperatureMask(i, j, k))
                            continue;

                        T(i, j, k) =
                            (1 - _relaxationFactor) * T(i, j, k) +
                            _relaxationFactor * Stencil::LocalSolve(T, rhs, system, i, j, k);
                    }
                }
            }
        }
    }
}
