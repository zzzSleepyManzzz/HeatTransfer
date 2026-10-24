#pragma once

namespace HeatTransfer::Core
{
    struct ErrorMetric
    {
        int Iteration;
        double ErrorMax;
        double ErrorMean;
        double ErrorRMS;
    };
}