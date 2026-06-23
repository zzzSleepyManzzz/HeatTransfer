#include "HeatTransfer/Visualisation/StatisticsData.h"

namespace HeatTransfer::Visualisation
{
    StatisticsData::StatisticsData(
        std::shared_ptr<HeatTransfer::SimulationRunner::SimulationOutput> output)
    {
        auto temperatureMatrix = output->TemperatureMatrix;
        const auto& errors = output->Errors;

        _minTemperature = temperatureMatrix->minCoeff();
        _maxTemperature = temperatureMatrix->maxCoeff();
        _meanTemperature = temperatureMatrix->mean();

        auto finalError = errors.back();

        _numIterations = finalError->iteration;
        _finalMaxError = finalError->errorMax;
        _finalMeanError = finalError->errorMean;
        _final_RMS_Error = finalError->errorRMS;
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
}