#pragma once

#include <Eigen/Dense>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/Solvers/ISolver.h"

namespace HeatTransfer::Solvers
{
    class JacobiMethod : public ISolver
    {
    public:
        JacobiMethod(const Core::SimulationParameters& parameters,
                     const Core::MaterialProperties& materialProperties,
                     const Core::BoundaryConditions& boundaryCondition);

    private:
        // Undamped Jacobi never reduces the checkerboard error, so it cannot smooth
        static constexpr double DampingFactor = 0.8;

        void Smoothen(Eigen::MatrixXd& matrix,
                      const Eigen::MatrixXd& rhs,
                      const InsulatedMask& insulated) override;
    };
}