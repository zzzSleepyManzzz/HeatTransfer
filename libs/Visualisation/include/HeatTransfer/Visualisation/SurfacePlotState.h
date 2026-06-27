#pragma once

namespace HeatTransfer::Visualisation
{
    struct SurfacePlotState
    {
        bool ResetZoom = false;
        int SelectedColorMap = 6; // Hot by default

        bool HideLinesOn = false;
        bool RemoveFillOn = false;
    };
}