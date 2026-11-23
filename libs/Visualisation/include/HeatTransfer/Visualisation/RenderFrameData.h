#pragma once

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

#include "HeatTransfer/Core/TrackingMetric.h"

#include "HeatTransfer/SimulationRunner/SimulationOutput.h"

#include "HeatTransfer/Visualisation/ConvergenceMetric.h"
#include "HeatTransfer/Visualisation/PlotField.h"
#include "HeatTransfer/Visualisation/PlotSeries.h"

namespace HeatTransfer::Visualisation
{
    class RenderFrameData
    {
    public:
        RenderFrameData(std::shared_ptr<HeatTransfer::SimulationRunner::SimulationOutput> output);

        const Core::Tensor3d& GetField(PlotField field);
        double GetFieldMin(PlotField field);
        double GetFieldMax(PlotField field);

        const PlotSeries& GetIterations();
        const PlotSeries& GetConvergence(ConvergenceMetric metric, Core::TrackingMetric statistic);

    private:
        std::shared_ptr<Core::Tensor3d> _temperature;
        std::shared_ptr<Core::Tensor3d> _residual;

        double _temperature_min;
        double _temperature_max;

        double _residual_min;
        double _residual_max;

        PlotSeries _iterations;

        PlotSeries _maxErrors;
        PlotSeries _meanErrors;
        PlotSeries _rmsErrors;

        PlotSeries _maxResiduals;
        PlotSeries _meanResiduals;
        PlotSeries _rmsResiduals;

        static void ComputeRange(PlotSeries& series);
    };
}
