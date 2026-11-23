#pragma once

#include "HeatTransfer/Visualisation/ConvergenceMetric.h"

namespace HeatTransfer::Visualisation
{
    struct ConvergencePlotsState
    {
        ConvergenceMetric Metric = ConvergenceMetric::ERROR;

        bool ResetZoom = false;
    };
}
