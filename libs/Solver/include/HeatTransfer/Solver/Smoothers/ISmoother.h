#pragma once

#include "HeatTransfer/Core/Tensor3d.h"

#include "HeatTransfer/Solver/Discretisation/SystemMatrix.h"

namespace HeatTransfer::Solver::Smoothers
{
    enum class SweepDirection
    {
        FORWARD,
        BACKWARD
    };

    class ISmoother
    {
    public:
        virtual ~ISmoother() = default;

        virtual void Smooth(Core::Tensor3d& matrix,
                            const Core::Tensor3d& rhs,
                            const SystemMatrix& system,
                            SweepDirection direction) const = 0;
    };
}
