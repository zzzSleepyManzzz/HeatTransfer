#pragma once

#include <chrono>
#include <format>
#include <memory>

#include "HeatTransfer/SimulationRunner/Simulations.h"
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