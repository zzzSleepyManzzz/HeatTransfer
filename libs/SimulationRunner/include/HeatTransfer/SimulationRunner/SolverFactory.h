#pragma once

#include <memory>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/MaterialProperties.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/Solvers/ISolver.h"

namespace HeatTransfer::SimulationRunner
{
    class SolverFactory
    {
    public:
        SolverFactory(const Core::SimulationParameters& parameters,
                      const Core::MaterialProperties& materialProperties,
                      const Core::BoundaryConditions& boundaryCondition);
        std::shared_ptr<Solvers::ISolver> Create();

    private:
        Core::SimulationParameters _parameters;
        Core::MaterialProperties _materialProperties;
        Core::BoundaryConditions _boundaryCondition;
    };
}