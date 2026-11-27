#pragma once

#include <chrono>
#include <format>
#include <iostream>
#include <memory>

#include "HeatTransfer/SimulationRunner/Simulation.h"

namespace HeatTransfer::Orchestration
{
    class SimpleTransientApplication
    {
    public:
        SimpleTransientApplication();
        void Run();
    };
}