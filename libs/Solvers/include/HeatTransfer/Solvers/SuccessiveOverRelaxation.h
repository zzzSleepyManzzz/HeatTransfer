#pragma once

#include <Eigen/Dense>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/Solvers/ISolver.h"

namespace HeatTransfer::Solvers
{
    class SuccessiveOverRelaxation : public ISolver
    {
    public:
        SuccessiveOverRelaxation(const Core::SimulationParameters& parameters,
                                 const Core::BoundaryConditions& boundaryCondition);
        void ComputeSimulation() override;

    private:
        void IterateTemperature(int maxIterations,
                                double tolerance,
                                double relaxationFactor,
                                int& totalIterations);
    };
}