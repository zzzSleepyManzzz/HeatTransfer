#pragma once

#include <string>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/SimulationParameters.h"

namespace HeatTransfer::Visualisation
{
    struct SettingsState
    {
        bool lightModeOn = false;
        int iterativeMethod = 2; // SOR by default
        Core::SimulationParameters temporaryParameters = Core::SimulationParameters::Default();
        Core::BoundaryConditions temporaryBoundaryConditions = Core::BoundaryConditions::Default();
    };
}