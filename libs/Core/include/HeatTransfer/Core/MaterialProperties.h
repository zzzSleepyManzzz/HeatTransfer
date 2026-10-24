#pragma once

namespace HeatTransfer::Core
{
    struct MaterialProperties
    {
        double ThermalConductivity;    // [W/(m·K)]
        double InternalHeatGeneration; // [W/m³]

        constexpr static MaterialProperties Default()
        {
            return {.ThermalConductivity = 237.0, .InternalHeatGeneration = 0.0};
        }
    };
}