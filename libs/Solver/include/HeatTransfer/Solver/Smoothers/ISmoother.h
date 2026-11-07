#pragma once

#include <Eigen/Dense>

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

        virtual void Smooth(Eigen::MatrixXd& matrix,
                            const Eigen::MatrixXd& rhs,
                            const Eigen::MatrixXd& insulated,
                            SweepDirection direction) const = 0;
    };
}
