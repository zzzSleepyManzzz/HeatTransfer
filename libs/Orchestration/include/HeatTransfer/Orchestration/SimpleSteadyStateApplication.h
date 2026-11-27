#pragma once

#include <chrono>
#include <format>
#include <iostream>
#include <memory>

#include "HeatTransfer/SimulationRunner/Simulation.h"

namespace HeatTransfer::Orchestration
{
    class SimpleSteadyStateApplication
    {
    public:
        SimpleSteadyStateApplication();
        void Run();
    };
}