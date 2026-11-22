#pragma once

#include "HeatTransfer/Solver/Smoothers/ISmoother.h"

namespace HeatTransfer::Solver::Smoothers
{
    class SuccessiveOverRelaxation : public ISmoother
    {
    public:
        explicit SuccessiveOverRelaxation(double relaxationFactor);

        void Smooth(Core::Tensor3d& matrix,
                    const Core::Tensor3d& rhs,
                    const SystemMatrix& system,
                    SweepDirection direction) const override;

    private:
        double _relaxationFactor;
    };
}
