#include "HeatTransfer/Visualisation/StatisticsData.h"

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

        // A run cancelled before its first iteration has no metrics, so keep the zero defaults

        const auto& errorMetrics = output->ErrorMetrics;

        if (!errorMetrics.empty())
        {
            auto finalError = errorMetrics.back();

            _numIterations = finalError->Iteration;
            _finalMaxError = finalError->ErrorMax;
            _finalMeanError = finalError->ErrorMean;
            _finalRMSError = finalError->ErrorRMS;
        }

        const auto& residualMetrics = output->ResidualMetrics;

        if (!residualMetrics.empty())
        {
            auto finalResidual = residualMetrics.back();

            _finalMaxResidual = finalResidual->ResidualMax;
            _finalMeanResidual = finalResidual->ResidualMean;
            _finalRMSResidual = finalResidual->ResidualRMS;
        }

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

    int StatisticsData::GetNumIterations()
    {
        return _numIterations;
    }

    double StatisticsData::GetFinalMaxError()
    {
        return _finalMaxError;
    }

    double StatisticsData::GetFinalMeanError()
    {
        return _finalMeanError;
    }

    double StatisticsData::GetFinalRMSError()
    {
        return _finalRMSError;
    }

    double StatisticsData::GetFinalMaxResidual()
    {
        return _finalMaxResidual;
    }

    double StatisticsData::GetFinalMeanResidual()
    {
        return _finalMeanResidual;
    }

    double StatisticsData::GetFinalRMSResidual()
    {
        return _finalRMSResidual;
    }

    bool StatisticsData::IsConverged()
    {
        return _isConverged;
    }
}