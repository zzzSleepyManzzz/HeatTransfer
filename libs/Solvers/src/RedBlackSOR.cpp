#include "HeatTransfer/Solvers/RedBlackSOR.h"

#include <tracy/Tracy.hpp>

namespace HeatTransfer::Solvers
{
    RedBlackSOR::RedBlackSOR(const Core::SimulationParameters& parameters,
                             const Core::MaterialProperties& materialProperties,
                             const Core::BoundaryConditions& boundaryCondition)
        : ISolver(parameters, materialProperties, boundaryCondition)
    {
    }

    void RedBlackSOR::Smoothen(Eigen::MatrixXd& matrix,
                               const Eigen::MatrixXd& rhs,
                               const InsulatedMask& insulated)
    {
        ZoneScoped;

        const Eigen::Index rows = matrix.rows();
        const Eigen::Index cols = matrix.cols();

        auto& T = matrix;

        auto relaxationFactor = _parameters.RelaxationFactor;

        auto RED = 0;
        auto BLACK = 1;

        for (auto colour : {RED, BLACK})
        {
            // A node only reads nodes of the other colour, so columns are independent
#pragma omp parallel for schedule(static)
            for (Eigen::Index j = 1; j < cols - 1; j++)
            {
                // First interior row of this colour in column j, then every other row
                const Eigen::Index start = 1 + ((1 + j + colour) % 2);

                for (Eigen::Index i = start; i < rows - 1; i += 2)
                {
                    if (insulated(i, j))
                        continue;

                    T(i, j) = (1 - relaxationFactor) * T(i, j) +
                              relaxationFactor *
                                  (0.25 * (T(i - 1, j) + T(i + 1, j) + T(i, j - 1) + T(i, j + 1)) +
                                   rhs(i, j));
                }
            }
        }
    }
}
