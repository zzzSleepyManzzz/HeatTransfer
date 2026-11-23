#pragma once

#include <vector>

namespace HeatTransfer::Visualisation
{
    struct PlotSeries
    {
        std::vector<double> Values;
        double Min = 0.0;
        double Max = 0.0;
    };
}
