#pragma once

namespace HeatTransfer::Core
{
    struct ResidualMetric
    {
        int Iteration;
        double ResidualMax;
        double ResidualMean;
        double ResidualRMS;
    };
}