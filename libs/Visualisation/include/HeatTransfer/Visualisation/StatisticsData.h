#pragma once

#include <memory>

#include "HeatTransfer/SimulationRunner/SimulationOutput.h"

namespace HeatTransfer::Visualisation
{
    class StatisticsData
    {
    public:
        StatisticsData(std::shared_ptr<HeatTransfer::SimulationRunner::SimulationOutput> output);

        double GetMaxTemperature();
        double GetMinTemperature();
        double GetMeanTemperature();

        int GetNumIterations();
        double GetFinalMaxError();
        double GetFinalMeanError();
        double GetFinal_RMS_Error();

    private:
        double _maxTemperature;
        double _minTemperature;
        double _meanTemperature;

        int _numIterations;
        double _finalMaxError;
        double _finalMeanError;
        double _final_RMS_Error;
    };
}