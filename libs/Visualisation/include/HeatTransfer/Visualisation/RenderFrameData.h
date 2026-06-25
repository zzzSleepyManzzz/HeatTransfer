#pragma once

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

#include "HeatTransfer/SimulationRunner/SimulationOutput.h"

namespace HeatTransfer::Visualisation
{
    class RenderFrameData
    {
    public:
        RenderFrameData(std::shared_ptr<HeatTransfer::SimulationRunner::SimulationOutput> output);

        const std::vector<double>& Get_X_Values();
        const std::vector<double>& Get_Y_Values();
        const std::vector<double>& Get_Z_Values();

        int GetRows();
        int GetCols();

        double Get_X_Min();
        double Get_X_Max();

        double Get_Y_Min();
        double Get_Y_Max();

        double Get_Z_Min();
        double Get_Z_Max();

        const std::vector<double>& Get_Iterations();
        const std::vector<double>& Get_MaxErrors();
        const std::vector<double>& Get_MeanErrors();
        const std::vector<double>& Get_RMS_Errors();

        double Get_Iterations_Min();
        double Get_Iterations_Max();

        double Get_MaxErrors_Min();
        double Get_MaxErrors_Max();

        double Get_MeanErrors_Min();
        double Get_MeanErrors_Max();

        double Get_RMS_Errors_Min();
        double Get_RMS_Errors_Max();

    private:
        std::vector<double> _x_values;
        std::vector<double> _y_values;
        std::vector<double> _z_values;

        int _rows;
        int _cols;

        double _x_min;
        double _x_max;

        double _y_min;
        double _y_max;

        double _z_min;
        double _z_max;

        std::vector<double> _iterations;
        std::vector<double> _maxErrors;
        std::vector<double> _meanErrors;
        std::vector<double> _RMS_Errors;

        double _iterations_min;
        double _iterations_max;

        double _maxErrors_min;
        double _maxErrors_max;

        double _meanErrors_min;
        double _meanErrors_max;

        double _RMS_Errors_min;
        double _RMS_Errors_max;
    };
}