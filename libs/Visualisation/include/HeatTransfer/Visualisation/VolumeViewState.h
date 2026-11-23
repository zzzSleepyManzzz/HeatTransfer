#pragma once

#include <array>

namespace HeatTransfer::Visualisation
{
    struct VolumeViewState
    {
        bool ResetZoom = false;
        float ViewScale = 1.1f;

        int TemperatureColorMap = 6; // Hot by default
        int ResidualColorMap = 6;

        bool ShowOuterFaces = true;

        std::array<bool, 3> ShowPlane = {false, false, false};
        std::array<int, 3> PlaneIndex = {-1, -1, -1};

        bool ShowLines = true;
    };
}
