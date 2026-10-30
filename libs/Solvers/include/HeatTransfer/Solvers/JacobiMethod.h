#pragma once

#include "HeatTransfer/Solvers/IMultigridSolver.h"

namespace HeatTransfer::Solvers
{
    class JacobiMethod : public IMultigridSolver
    {
    public:
        JacobiMethod(const Core::SimulationParameters& parameters,
                     const Core::MaterialProperties& materialProperties,
                     const Core::BoundaryConditions& boundaryCondition);

    private:
        // Undamped Jacobi never reduces the checkerboard error, so it cannot smooth
        // Damping factor must be used for Jacobi to be a smoother
        static constexpr double DampingFactor = 0.8;

        void Smoothen(Eigen::MatrixXd& matrix,
                      const Eigen::MatrixXd& rhs,
                      const Eigen::MatrixXd& insulated) override;
    };
}
