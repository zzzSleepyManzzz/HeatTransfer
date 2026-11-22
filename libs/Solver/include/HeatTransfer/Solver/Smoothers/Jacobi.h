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
        static constexpr double DAMPING_FACTOR = 0.8;
    };
}
