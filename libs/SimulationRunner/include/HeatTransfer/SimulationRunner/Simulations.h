#pragma once

#include <memory>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/MaterialProperties.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/SimulationRunner/SimulationConfig.h"
#include "HeatTransfer/SimulationRunner/SimulationOutput.h"

#include "HeatTransfer/Solvers/ISolver.h"

namespace HeatTransfer::SimulationRunner
{
    class Simulations
    {
    public:
        Simulations(std::shared_ptr<SimulationConfig> config);
        void Run();
        void Print();

        std::shared_ptr<SimulationOutput> GetOutput();

    private:
        Core::SimulationParameters _parameters;
        Core::MaterialProperties _materialProperties;
        Core::BoundaryConditions _boundaryCondition;
        std::shared_ptr<Solvers::ISolver> _solver;
    };
}