#pragma once

namespace HeatTransfer::Visualisation
{
    struct SliceState
    {
        int Axis = 2;   // Z-axis by default
        int Index = -1; // Negative selects the middle slice
    };
}
