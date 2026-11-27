#pragma once

#include <optional>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/InitialConditions.h"
#include "HeatTransfer/Core/MaterialProperties.h"
#include "HeatTransfer/Core/SimulationParameters.h"

namespace HeatTransfer::SimulationRunner
{
    struct SimulationConfig
    {
        HeatTransfer::Core::SimulationParameters Parameters;
        HeatTransfer::Core::MaterialProperties MaterialProperties;
        HeatTransfer::Core::BoundaryConditions BoundaryConditions;
        std::optional<HeatTransfer::Core::InitialConditions> InitialConditions;
    };
}