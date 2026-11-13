#pragma once

#include <Eigen/Dense>

namespace HeatTransfer::Solver
{
    class Stencil
    {
    public:
        Stencil() = delete;

        static Eigen::MatrixXd ComputeResidual(const Eigen::MatrixXd& matrix,
                                               const Eigen::MatrixXd& rhs,
                                               const Eigen::MatrixXd& insulated);
    };
}
