#pragma once

#include "HeatTransfer/Solvers/Smoothers/ISmoother.h"

namespace HeatTransfer::Solvers::Smoothers
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
