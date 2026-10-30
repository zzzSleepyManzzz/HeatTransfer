#include "HeatTransfer/Solvers/JacobiMethod.h"

#include <tracy/Tracy.hpp>

namespace HeatTransfer::Solvers
{
    JacobiMethod::JacobiMethod(const Core::SimulationParameters& parameters,
                               const Core::MaterialProperties& materialProperties,
                               const Core::BoundaryConditions& boundaryCondition)
        : IMultigridSolver(parameters, materialProperties, boundaryCondition)
    {
    }

    void JacobiMethod::Smoothen(Eigen::MatrixXd& matrix,
                                const Eigen::MatrixXd& rhs,
                                const Eigen::MatrixXd& insulated)
    {
        ZoneScoped;

        const Eigen::Index rows = matrix.rows();
        const Eigen::Index cols = matrix.cols();

        const Eigen::MatrixXd old = matrix;

        matrix.block(1, 1, rows - 2, cols - 2) =
            (1 - DampingFactor) * old.block(1, 1, rows - 2, cols - 2) +
            DampingFactor *
                ((old.block(0, 1, rows - 2, cols - 2) + old.block(2, 1, rows - 2, cols - 2) +
                  old.block(1, 0, rows - 2, cols - 2) + old.block(1, 2, rows - 2, cols - 2)) *
                     0.25 +
                 rhs.block(1, 1, rows - 2, cols - 2));

        matrix.array() = insulated.array().select(old.array(), matrix.array());
    }
}
