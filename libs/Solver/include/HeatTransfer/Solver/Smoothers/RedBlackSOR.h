#pragma once

#include "HeatTransfer/Solver/Smoothers/ISmoother.h"

namespace HeatTransfer::Solver::Smoothers
{
    class RedBlackSOR : public ISmoother
    {
    public:
        explicit RedBlackSOR(double relaxationFactor);

        void Smooth(Eigen::MatrixXd& matrix,
                    const Eigen::MatrixXd& rhs,
                    const Eigen::MatrixXd& insulated,
                    SweepDirection direction) const override;

    private:
        double _relaxationFactor;
    };
}
