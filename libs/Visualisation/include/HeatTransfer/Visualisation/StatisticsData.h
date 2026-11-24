#pragma once

#include <memory>

#include "HeatTransfer/Core/ErrorMetric.h"
#include "HeatTransfer/Core/ResidualMetric.h"

#include "HeatTransfer/SimulationRunner/SimulationOutput.h"

namespace HeatTransfer::Visualisation
{
    class StatisticsData
    {
    public:
        StatisticsData(std::shared_ptr<HeatTransfer::SimulationRunner::SimulationOutput> output,
                       double durationInSeconds);

        double GetDurationInSeconds();

        double GetMaxTemperature();
        double GetMinTemperature();
        double GetMeanTemperature();

        const Core::ErrorMetric& GetFinalError();
        const Core::ResidualMetric& GetFinalResidual();

        bool IsConverged();

    private:
        double _durationInSeconds;

        double _maxTemperature;
        double _minTemperature;
        double _meanTemperature;

        Core::ErrorMetric _finalError;
        Core::ResidualMetric _finalResidual;

        bool _isConverged;
    };
}