#pragma once

#include <chrono>
#include <memory>

#include "HeatTransfer/SimulationRunner/AsyncSimulation.h"
#include "HeatTransfer/SimulationRunner/Simulation.h"
#include "HeatTransfer/Visualisation/Renderer.h"

namespace HeatTransfer::Orchestration
{
    class Application
    {
    public:
        Application();
        void Run();
    };
}