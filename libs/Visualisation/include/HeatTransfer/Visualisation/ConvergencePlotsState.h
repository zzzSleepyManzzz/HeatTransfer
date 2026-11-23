#pragma once

namespace HeatTransfer::Visualisation
{
    enum class ConvergenceMetric
    {
        ERROR,
        RESIDUAL
    };

    struct ConvergencePlotsState
    {
        ConvergenceMetric Metric = ConvergenceMetric::ERROR;

        bool ResetZoom = false;
    };
}
