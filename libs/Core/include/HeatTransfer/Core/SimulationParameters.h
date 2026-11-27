#pragma once

#include "HeatTransfer/Core/Smoother.h"
#include "HeatTransfer/Core/TrackingMetric.h"

namespace HeatTransfer::Core
{
    struct SimulationParameters
    {
        int Rows;
        int Columns;
        int Layers;
        double ResidualTolerance;
        double ErrorTolerance;
        double RelaxationFactor;
        int MaxIterations;
        TrackingMetric ResidualTrackingMetric;
        TrackingMetric ErrorTrackingMetric;
        Core::Smoother Smoother = Core::Smoother::RED_BLACK_SOR;
        double GridSpacing;
        double TimeStep;
        int NumberOfTimeSteps;

        static SimulationParameters Default();
    };
}