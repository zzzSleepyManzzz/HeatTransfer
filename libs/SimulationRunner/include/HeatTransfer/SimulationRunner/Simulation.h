#pragma once

#include <memory>
#include <stop_token>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/MaterialProperties.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/SimulationRunner/SimulationConfig.h"
#include "HeatTransfer/SimulationRunner/SimulationOutput.h"

#include "HeatTransfer/Solvers/ISolver.h"

namespace HeatTransfer::SimulationRunner
{
    class Simulation
    {
    public:
        Simulation(std::shared_ptr<SimulationConfig> config);
        void Run(std::stop_token stopToken = {});
        void Print();
        int GetCurrentIteration() const;

        std::shared_ptr<SimulationOutput> GetOutput();

    private:
        Core::SimulationParameters _parameters;
        Core::MaterialProperties _materialProperties;
        Core::BoundaryConditions _boundaryCondition;
        std::shared_ptr<Solvers::ISolver> _solver;
    };
}