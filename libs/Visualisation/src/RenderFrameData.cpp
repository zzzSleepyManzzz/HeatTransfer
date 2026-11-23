#include "HeatTransfer/Visualisation/RenderFrameData.h"

#include <algorithm>
#include <stdexcept>

#include "HeatTransfer/Core/Tensor3dMapper.h"

namespace HeatTransfer::Visualisation
{
    RenderFrameData::RenderFrameData(
        std::shared_ptr<HeatTransfer::SimulationRunner::SimulationOutput> output)
    {
        _temperature = output->TemperatureTensor;
        _residual = output->ResidualTensor;

        const auto& errorMetrics = output->ErrorMetrics;
        const auto& residualMetrics = output->ResidualMetrics;

        const auto T = Core::Tensor3dMapper::AsVector(*_temperature);
        const auto R = Core::Tensor3dMapper::AsVector(*_residual);

        // Min and max over the whole field
        _temperature_min = T.minCoeff();
        _temperature_max = T.maxCoeff();

        _residual_min = R.minCoeff();
        _residual_max = R.maxCoeff();

        // Populate error and residual related information

        for (auto errorMetric : errorMetrics)
        {
            _iterations.Values.push_back(errorMetric->Iteration);
            _maxErrors.Values.push_back(errorMetric->ErrorMax);
            _meanErrors.Values.push_back(errorMetric->ErrorMean);
            _rmsErrors.Values.push_back(errorMetric->ErrorRMS);
        }

        for (auto residualMetric : residualMetrics)
        {
            _maxResiduals.Values.push_back(residualMetric->ResidualMax);
            _meanResiduals.Values.push_back(residualMetric->ResidualMean);
            _rmsResiduals.Values.push_back(residualMetric->ResidualRMS);
        }

        ComputeRange(_iterations);

        ComputeRange(_maxErrors);
        ComputeRange(_meanErrors);
        ComputeRange(_rmsErrors);

        ComputeRange(_maxResiduals);
        ComputeRange(_meanResiduals);
        ComputeRange(_rmsResiduals);
    }

    void RenderFrameData::ComputeRange(PlotSeries& series)
    {
        if (series.Values.empty())
            return;

        const auto [min, max] = std::minmax_element(series.Values.begin(), series.Values.end());

        series.Min = *min;
        series.Max = *max;
    }

    const Core::Tensor3d& RenderFrameData::GetField(PlotField field)
    {
        return field == PlotField::RESIDUAL ? *_residual : *_temperature;
    }

    double RenderFrameData::GetFieldMin(PlotField field)
    {
        return field == PlotField::RESIDUAL ? _residual_min : _temperature_min;
    }

    double RenderFrameData::GetFieldMax(PlotField field)
    {
        return field == PlotField::RESIDUAL ? _residual_max : _temperature_max;
    }

    const PlotSeries& RenderFrameData::GetIterations()
    {
        return _iterations;
    }

    const PlotSeries& RenderFrameData::GetConvergence(ConvergenceMetric metric,
                                                      Core::TrackingMetric statistic)
    {
        const bool isResidual = metric == ConvergenceMetric::RESIDUAL;

        switch (statistic)
        {
            case Core::TrackingMetric::MAX:
                return isResidual ? _maxResiduals : _maxErrors;
            case Core::TrackingMetric::MEAN:
                return isResidual ? _meanResiduals : _meanErrors;
            case Core::TrackingMetric::RMS:
                return isResidual ? _rmsResiduals : _rmsErrors;
            default:
                throw std::runtime_error("Invalid tracking metric");
        }
    }
}
