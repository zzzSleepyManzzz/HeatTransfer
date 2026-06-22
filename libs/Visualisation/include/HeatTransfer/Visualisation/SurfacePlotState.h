#pragma once

namespace HeatTransfer::Visualisation
{
    struct SurfacePlotState
    {
        bool resetZoom = false;
        int selectedColorMap = 6; // Hot by default

        bool hideLinesOn = false;
        bool removeFillOn = false;
    };
}