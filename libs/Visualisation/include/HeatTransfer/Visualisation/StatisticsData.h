#pragma once

#include <memory>

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

        int GetNumIterations();
        double GetFinalMaxError();
        double GetFinalMeanError();
        double GetFinalRMSError();

        double GetFinalMaxResidual();
        double GetFinalMeanResidual();
        double GetFinalRMSResidual();

        bool IsConverged();

    private:
        double _durationInSeconds;

        double _maxTemperature;
        double _minTemperature;
        double _meanTemperature;

        int _numIterations;
        double _finalMaxError;
        double _finalMeanError;
        double _finalRMSError;

        double _finalMaxResidual;
        double _finalMeanResidual;
        double _finalRMSResidual;

        bool _isConverged;
    };
}