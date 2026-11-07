#pragma once

#include <memory>

#include "HeatTransfer/SimulationRunner/SimulationConfig.h"
#include "HeatTransfer/SimulationRunner/SimulationOutput.h"

namespace HeatTransfer::SimulationRunner
{
    struct SimulationResult
    {
        std::shared_ptr<SimulationConfig> Config;
        std::shared_ptr<SimulationOutput> Output;
        double DurationInSeconds;
    };
}
