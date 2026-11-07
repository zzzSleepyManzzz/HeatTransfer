#pragma once

#include "HeatTransfer/Solver/Smoothers/ISmoother.h"

namespace HeatTransfer::Solver::Smoothers
{
    class SuccessiveOverRelaxation : public ISmoother
    {
    public:
        explicit SuccessiveOverRelaxation(double relaxationFactor);

        void Smooth(Eigen::MatrixXd& matrix,
                    const Eigen::MatrixXd& rhs,
                    const Eigen::MatrixXd& insulated,
                    SweepDirection direction) const override;

    private:
        double _relaxationFactor;
    };
}
