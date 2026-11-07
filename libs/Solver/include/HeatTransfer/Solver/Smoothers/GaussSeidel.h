#pragma once

#include "HeatTransfer/Solver/Smoothers/ISmoother.h"

namespace HeatTransfer::Solver::Smoothers
{
    class GaussSeidel : public ISmoother
    {
    public:
        void Smooth(Eigen::MatrixXd& matrix,
                    const Eigen::MatrixXd& rhs,
                    const Eigen::MatrixXd& insulated,
                    SweepDirection direction) const override;
    };
}
