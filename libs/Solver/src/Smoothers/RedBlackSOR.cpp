#include "HeatTransfer/Solver/Smoothers/RedBlackSOR.h"

#include <array>

#include <tracy/Tracy.hpp>

namespace HeatTransfer::Solver::Smoothers
{
    RedBlackSOR::RedBlackSOR(double relaxationFactor) : _relaxationFactor(relaxationFactor) {}

    void RedBlackSOR::Smooth(Eigen::MatrixXd& matrix,
                             const Eigen::MatrixXd& rhs,
                             const Eigen::MatrixXd& insulated,
                             SweepDirection direction) const
    {
        ZoneScoped;

        const Eigen::Index rows = matrix.rows();
        const Eigen::Index cols = matrix.cols();

        auto& T = matrix;

        auto RED = 0;
        auto BLACK = 1;

        // Reversing the colour order gives the transpose of the sweep, so pairing them keeps the
        // V-cycle symmetric
        const auto colours =
            direction == SweepDirection::FORWARD ? std::array{RED, BLACK} : std::array{BLACK, RED};

        for (auto colour : colours)
        {
#pragma omp parallel for schedule(static)
            for (Eigen::Index j = 1; j < cols - 1; j++)
            {
                const Eigen::Index start = 1 + ((1 + j + colour) % 2);

                for (Eigen::Index i = start; i < rows - 1; i += 2)
                {
                    if (insulated(i, j))
                        continue;

                    T(i, j) = (1 - _relaxationFactor) * T(i, j) +
                              _relaxationFactor *
                                  (0.25 * (T(i - 1, j) + T(i + 1, j) + T(i, j - 1) + T(i, j + 1)) +
                                   rhs(i, j));
                }
            }
        }
    }
}
