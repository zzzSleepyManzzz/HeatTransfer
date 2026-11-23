#pragma once

#include "HeatTransfer/Solver/Smoothers/ISmoother.h"

namespace HeatTransfer::Solver::Smoothers
{
    class Jacobi : public ISmoother
    {
    public:
        void Smooth(Core::Tensor3d& matrix,
                    const Core::Tensor3d& rhs,
                    const SystemMatrix& system,
                    SweepDirection direction) const override;

    private:
        // Undamped Jacobi never reduces the checkerboard error, so it cannot smooth
        // Damping factor must be used for Jacobi to be a smoother
        // 6/7 is optimal for the 3D 7-point stencil, damping every high frequency by at least 5/7
        static constexpr double DAMPING_FACTOR = 6.0 / 7.0;
    };
}
