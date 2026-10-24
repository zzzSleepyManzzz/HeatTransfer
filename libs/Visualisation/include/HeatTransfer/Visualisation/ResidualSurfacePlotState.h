#pragma once

namespace HeatTransfer::Visualisation
{
    struct ResidualSurfacePlotState
    {
        bool ResetZoom = false;
        int SelectedColorMap = 6; // Hot by default

        bool HideLinesOn = false;
        bool RemoveFillOn = false;
    };
}