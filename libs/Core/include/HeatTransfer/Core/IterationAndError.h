#pragma once

namespace HeatTransfer::Core
{
    struct IterationAndError
    {
        int Iteration;
        double ErrorMax;
        double ErrorMean;
        double ErrorRMS;
    };
}