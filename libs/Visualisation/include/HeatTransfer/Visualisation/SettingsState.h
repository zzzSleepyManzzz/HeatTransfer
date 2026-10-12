#pragma once

#include <string>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/MaterialProperties.h"
#include "HeatTransfer/Core/SimulationParameters.h"

namespace HeatTransfer::Visualisation
{
    struct SettingsState
    {
        bool LightModeOn = false;
        float FontScaling = 1.0f;
        int IterativeMethod = 2;        // SOR by default
        int ResidualTrackingMetric = 0; // MAX by default
        int ErrorTrackingMetric = 0;    // MAX by default
        Core::SimulationParameters TemporaryParameters = Core::SimulationParameters::Default();
        Core::MaterialProperties TemporaryMaterialProperties = Core::MaterialProperties::Default();
        Core::BoundaryConditions TemporaryBoundaryConditions = Core::BoundaryConditions::Default();
    };
}