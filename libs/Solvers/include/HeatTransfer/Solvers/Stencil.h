#pragma once

#include <Eigen/Dense>

namespace HeatTransfer::Solvers::Stencil
{
    Eigen::MatrixXd ComputeResidual(const Eigen::MatrixXd& matrix,
                                    const Eigen::MatrixXd& rhs,
                                    const Eigen::MatrixXd& insulated);
}
