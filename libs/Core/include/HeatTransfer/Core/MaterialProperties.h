#pragma once

namespace HeatTransfer::Core
{
    struct MaterialProperties
    {
        double ThermalConductivity;    // [W/(m·K)]
        double Density;                // [kg/m³]
        double SpecificHeatCapacity;   // [J/(kg·K)]
        double InternalHeatGeneration; // [W/m³]

        static MaterialProperties Default();
    };
}