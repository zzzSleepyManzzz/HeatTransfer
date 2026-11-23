#pragma once

#include <array>

#include "HeatTransfer/Visualisation/PlotField.h"

namespace HeatTransfer::Visualisation
{
    struct VolumeViewState
    {
        bool ResetZoom = false;
        std::array<int, 2> SelectedColorMap = {6, 6}; // One per PlotField, Hot by default

        bool ShowOuterFaces = true;

        std::array<bool, 3> ShowPlane = {false, false, false};
        std::array<int, 3> PlaneIndex = {-1, -1, -1};

        bool ShowLines = true;
    };
}
