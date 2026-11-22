#pragma once

#include "HeatTransfer/Core/BoundaryType.h"

namespace HeatTransfer::Core
{
    struct FaceBoundaryCondition
    {
        // TODO: Maybe refactor to use std::optional
        BoundaryType Type = BoundaryType::HEAT_FLUX;
        double Temperature = 300.0;            // FIXED_TEMPERATURE [K]
        double HeatFlux = 0.0;                 // HEAT_FLUX, into the plate [W/m²]
        double HeatTransferCoefficient = 10.0; // CONVECTION [W/(m²·K)]
        double AmbientTemperature = 300.0;     // CONVECTION [K]

        static FaceBoundaryCondition FixedTemperature(double temperature);
        static FaceBoundaryCondition HeatFluxIn(double heatFlux);
        static FaceBoundaryCondition Insulated();
        static FaceBoundaryCondition Convection(double heatTransferCoefficient,
                                                double ambientTemperature);
    };
}
