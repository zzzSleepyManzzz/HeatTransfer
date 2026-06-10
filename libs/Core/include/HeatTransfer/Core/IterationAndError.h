#pragma once

namespace HeatTransfer::Core
{
    struct IterationAndError
    {
        int iteration;
        double errorMax;
        double errorMean;
        double errorRMS;
    };
}