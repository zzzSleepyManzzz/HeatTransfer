#pragma once

#include "HeatTransfer/Solver/Smoothers/ISmoother.h"

namespace HeatTransfer::Solver::Smoothers
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
