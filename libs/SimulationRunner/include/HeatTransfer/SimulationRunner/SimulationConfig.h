#pragma once

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/Method.h"
#include "HeatTransfer/Core/SimulationParameters.h"

namespace HeatTransfer::SimulationRunner
{
    struct SimulationConfig
    {
        HeatTransfer::Core::SimulationParameters parameters;
        HeatTransfer::Core::BoundaryConditions boundaryConditions;
        HeatTransfer::Core::Method method;
    };
}