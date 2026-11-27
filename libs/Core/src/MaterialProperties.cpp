#include "HeatTransfer/Core/MaterialProperties.h"

namespace HeatTransfer::Core
{
    MaterialProperties MaterialProperties::Default()
    {
        // Aluminum material properties

        return {.ThermalConductivity = 237.0,
                .Density = 2700.0,
                .SpecificHeatCapacity = 897.0,
                .InternalHeatGeneration = 0.0};
    }
}
