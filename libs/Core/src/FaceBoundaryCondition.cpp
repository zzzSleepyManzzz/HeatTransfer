#include "HeatTransfer/Core/FaceBoundaryCondition.h"

namespace HeatTransfer::Core
{
    FaceBoundaryCondition FaceBoundaryCondition::FixedTemperature(double temperature)
    {
        return {.Type = BoundaryType::FIXED_TEMPERATURE, .Temperature = temperature};
    }

    FaceBoundaryCondition FaceBoundaryCondition::HeatFluxIn(double heatFlux)
    {
        return {.Type = BoundaryType::HEAT_FLUX, .HeatFlux = heatFlux};
    }

    FaceBoundaryCondition FaceBoundaryCondition::Insulated()
    {
        return HeatFluxIn(0.0);
    }

    FaceBoundaryCondition FaceBoundaryCondition::Convection(double heatTransferCoefficient,
                                                            double ambientTemperature)
    {
        return {.Type = BoundaryType::CONVECTION,
                .HeatTransferCoefficient = heatTransferCoefficient,
                .AmbientTemperature = ambientTemperature};
    }
}
