#pragma once

namespace HeatTransfer::Core
{
    struct TimeStepMetric
    {
        int TimeStep;
        double Time;
        int Iterations;
        double TemperatureMax;
        double TemperatureMean;
        double TemperatureMin;
    };
}
