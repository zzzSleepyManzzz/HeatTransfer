#pragma once

#include "HeatTransfer/Solvers/Smoothers/ISmoother.h"

namespace HeatTransfer::Solvers::Smoothers
{
    class Jacobi : public ISmoother
    {
    public:
        void Smooth(Eigen::MatrixXd& matrix,
                    const Eigen::MatrixXd& rhs,
                    const Eigen::MatrixXd& insulated,
                    SweepDirection direction) const override;

    private:
        // Undamped Jacobi never reduces the checkerboard error, so it cannot smooth
        // Damping factor must be used for Jacobi to be a smoother
        static constexpr double DampingFactor = 0.8;
    };
}
