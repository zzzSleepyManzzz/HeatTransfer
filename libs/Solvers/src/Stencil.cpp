#include "HeatTransfer/Solvers/Stencil.h"

namespace HeatTransfer::Solvers
{
    Eigen::MatrixXd Stencil::ComputeResidual(const Eigen::MatrixXd& matrix,
                                             const Eigen::MatrixXd& rhs,
                                             const Eigen::MatrixXd& insulated)
    {
        const Eigen::Index rows = matrix.rows();
        const Eigen::Index cols = matrix.cols();

        const auto& E = matrix;
        Eigen::MatrixXd R = Eigen::MatrixXd::Zero(rows, cols);

        R.block(1, 1, rows - 2, cols - 2) =
            rhs.block(1, 1, rows - 2, cols - 2) - E.block(1, 1, rows - 2, cols - 2) +
            (E.block(0, 1, rows - 2, cols - 2) + E.block(2, 1, rows - 2, cols - 2) +
             E.block(1, 0, rows - 2, cols - 2) + E.block(1, 2, rows - 2, cols - 2)) *
                0.25;

        R.array() = insulated.array().select(0.0, R.array());
        return R;
    }
}
