#pragma once

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/MaterialProperties.h"
#include "HeatTransfer/Core/Method.h"
#include "HeatTransfer/Core/SimulationParameters.h"

namespace HeatTransfer::SimulationRunner
{
    struct SimulationConfig
    {
        HeatTransfer::Core::SimulationParameters Parameters;
        HeatTransfer::Core::MaterialProperties MaterialProperties;
        HeatTransfer::Core::BoundaryConditions BoundaryConditions;
        HeatTransfer::Core::Method Method;
    };
}