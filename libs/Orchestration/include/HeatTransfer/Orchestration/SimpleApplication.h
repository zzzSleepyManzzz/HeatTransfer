#pragma once

#include <chrono>
#include <format>
#include <iostream>
#include <memory>

#include "HeatTransfer/SimulationRunner/Simulations.h"

namespace HeatTransfer::Orchestration
{
    class SimpleApplication
    {
    public:
        SimpleApplication();
        void Run();
    };
}