#include "HeatTransfer/Visualisation/StatisticsData.h"

#include <stdexcept>

#include "HeatTransfer/Core/Tensor3dMapper.h"

namespace HeatTransfer::Visualisation
{
    StatisticsData::StatisticsData(
        std::shared_ptr<HeatTransfer::SimulationRunner::SimulationOutput> output,
        double durationInSeconds)
        : _durationInSeconds(durationInSeconds)
    {
        const auto T = Core::Tensor3dMapper::AsVector(*output->TemperatureTensor);

        _minTemperature = T.minCoeff();
        _maxTemperature = T.maxCoeff();
        _meanTemperature = T.mean();

        const auto& errorMetrics = output->ErrorMetrics;
        const auto& residualMetrics = output->ResidualMetrics;

        if (errorMetrics.empty() || residualMetrics.empty())
        {
            throw std::runtime_error("Simulation output has no metrics");
        }

        _finalError = *errorMetrics.back();
        _finalResidual = *residualMetrics.back();

        _isConverged = output->IsConverged;
    }

    double StatisticsData::GetDurationInSeconds()
    {
        return _durationInSeconds;
    }

    double StatisticsData::GetMaxTemperature()
    {
        return _maxTemperature;
    }

    double StatisticsData::GetMinTemperature()
    {
        return _minTemperature;
    }

    double StatisticsData::GetMeanTemperature()
    {
        return _meanTemperature;
    }

    const Core::ErrorMetric& StatisticsData::GetFinalError()
    {
        return _finalError;
    }

    const Core::ResidualMetric& StatisticsData::GetFinalResidual()
    {
        return _finalResidual;
    }

    bool StatisticsData::IsConverged()
    {
        return _isConverged;
    }
}