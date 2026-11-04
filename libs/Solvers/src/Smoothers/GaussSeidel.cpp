#include "HeatTransfer/Solvers/Smoothers/GaussSeidel.h"

#include <tracy/Tracy.hpp>

namespace HeatTransfer::Solvers::Smoothers
{
    void GaussSeidel::Smooth(Eigen::MatrixXd& matrix,
                             const Eigen::MatrixXd& rhs,
                             const Eigen::MatrixXd& insulated,
                             SweepDirection direction) const
    {
        ZoneScoped;

        const Eigen::Index rows = matrix.rows();
        const Eigen::Index cols = matrix.cols();

        auto& T = matrix;

        // A backward sweep is the transpose of a forward sweep, so pairing them keeps the
        // V-cycle symmetric
        const bool forward = direction == SweepDirection::FORWARD;

        for (Eigen::Index n = 1; n < cols - 1; n++)
        {
            const Eigen::Index j = forward ? n : cols - 1 - n;

            for (Eigen::Index m = 1; m < rows - 1; m++)
            {
                const Eigen::Index i = forward ? m : rows - 1 - m;

                if (insulated(i, j))
                    continue;

                T(i, j) =
                    0.25 * (T(i - 1, j) + T(i + 1, j) + T(i, j - 1) + T(i, j + 1)) + rhs(i, j);
            }
        }
    }
}
