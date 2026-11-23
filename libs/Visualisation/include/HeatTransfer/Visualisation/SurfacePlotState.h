#pragma once

#include "HeatTransfer/Visualisation/PlotField.h"

namespace HeatTransfer::Visualisation
{
    struct SurfacePlotState
    {
        PlotField Field = PlotField::TEMPERATURE;

        bool ResetZoom = false;
        bool FitToSlice = false;  // Refit the axes to a new slice, keeping the rotation
        int SelectedColorMap = 6; // Hot by default

        bool HideLinesOn = false;
        bool RemoveFillOn = false;
    };
}
