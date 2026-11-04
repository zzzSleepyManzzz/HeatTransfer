#pragma once

#include "HeatTransfer/Solvers/Smoothers/ISmoother.h"

namespace HeatTransfer::Solvers::Smoothers
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
