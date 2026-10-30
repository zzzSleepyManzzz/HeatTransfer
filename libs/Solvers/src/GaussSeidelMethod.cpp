#include "HeatTransfer/Solvers/GaussSeidelMethod.h"

#include <tracy/Tracy.hpp>

namespace HeatTransfer::Solvers
{
    GaussSeidelMethod::GaussSeidelMethod(const Core::SimulationParameters& parameters,
                                         const Core::MaterialProperties& materialProperties,
                                         const Core::BoundaryConditions& boundaryCondition)
        : IMultigridSolver(parameters, materialProperties, boundaryCondition)
    {
    }

    void GaussSeidelMethod::Smoothen(Eigen::MatrixXd& matrix,
                                     const Eigen::MatrixXd& rhs,
                                     const Eigen::MatrixXd& insulated)
    {
        ZoneScoped;

        const Eigen::Index rows = matrix.rows();
        const Eigen::Index cols = matrix.cols();

        auto& T = matrix;

        // Eigen is column-major, so walk down each column
        for (Eigen::Index j = 1; j < cols - 1; j++)
        {
            for (Eigen::Index i = 1; i < rows - 1; i++)
            {
                if (insulated(i, j))
                    continue;

                T(i, j) =
                    0.25 * (T(i - 1, j) + T(i + 1, j) + T(i, j - 1) + T(i, j + 1)) + rhs(i, j);
            }
        }
    }
}
