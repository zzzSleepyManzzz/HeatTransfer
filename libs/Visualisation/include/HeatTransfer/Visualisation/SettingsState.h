#pragma once

#include <string>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/SimulationParameters.h"

namespace HeatTransfer::Visualisation
{
    struct SettingsState
    {
        bool LightModeOn = false;
        int IterativeMethod = 2; // SOR by default
        Core::SimulationParameters TemporaryParameters = Core::SimulationParameters::Default();
        Core::BoundaryConditions TemporaryBoundaryConditions = Core::BoundaryConditions::Default();
    };
}