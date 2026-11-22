#pragma once

namespace HeatTransfer::Core
{
    struct MaterialProperties
    {
        double ThermalConductivity;    // [W/(m·K)]
        double InternalHeatGeneration; // [W/m³]

        static MaterialProperties Default();
    };
}