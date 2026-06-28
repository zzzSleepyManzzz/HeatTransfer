#include "HeatTransfer/Visualisation/StatisticsData.h"

namespace HeatTransfer::Visualisation
{
    StatisticsData::StatisticsData(
        std::shared_ptr<HeatTransfer::SimulationRunner::SimulationOutput> output,
        double durationInSeconds)
        : _durationInSeconds(durationInSeconds)
    {
        auto temperatureMatrix = output->TemperatureMatrix;
        const auto& errors = output->Errors;

        _minTemperature = temperatureMatrix->minCoeff();
        _maxTemperature = temperatureMatrix->maxCoeff();
        _meanTemperature = temperatureMatrix->mean();

        auto finalError = errors.back();

        _numIterations = finalError->Iteration;
        _finalMaxError = finalError->ErrorMax;
        _finalMeanError = finalError->ErrorMean;
        _final_RMS_Error = finalError->ErrorRMS;

        const auto& residualMetrics = output->ResidualMetrics;
        auto finalResidual = residualMetrics.back();

        _finalMaxResidual = finalResidual->ResidualMax;
        _finalMeanResidual = finalResidual->ResidualMean;
        _final_RMS_Residual = finalResidual->ResidualRMS;
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

    double StatisticsData::GetFinal_RMS_Error()
    {
        return _final_RMS_Error;
    }

    double StatisticsData::GetFinalMaxResidual()
    {
        return _finalMaxResidual;
    }

    double StatisticsData::GetFinalMeanResidual()
    {
        return _finalMeanResidual;
    }

    double StatisticsData::GetFinal_RMS_Residual()
    {
        return _final_RMS_Residual;
    }
}