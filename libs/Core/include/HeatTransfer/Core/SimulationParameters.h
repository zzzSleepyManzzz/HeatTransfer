#pragma once

#include "HeatTransfer/Core/TrackingMetric.h"

namespace HeatTransfer::Core
{
    struct SimulationParameters
    {
        int Rows;
        int Columns;
        double ResidualTolerance;
        double ErrorTolerance;
        double RelaxationFactor;
        int MaxIterations;
        TrackingMetric ResidualTrackingMetric;
        TrackingMetric ErrorTrackingMetric;

        constexpr static const SimulationParameters Default()
        {
            return {.Rows = 1000,
                    .Columns = 1000,
                    .ResidualTolerance = 1e-3,
                    .ErrorTolerance = 1e-6,
                    .RelaxationFactor = 1.7,
                    .MaxIterations = 1000,
                    .ResidualTrackingMetric = Core::TrackingMetric::MAX,
                    .ErrorTrackingMetric = Core::TrackingMetric::MAX};
        }
    };
}