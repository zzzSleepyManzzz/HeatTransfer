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
        const std::vector<double>& Get_Temperature_Values();
        const std::vector<double>& Get_Residual_Values();

        int GetRows();
        int GetCols();

        double Get_X_Min();
        double Get_X_Max();

        double Get_Y_Min();
        double Get_Y_Max();

        double Get_Temperature_Min();
        double Get_Temperature_Max();

        double Get_Residual_Min();
        double Get_Residual_Max();

        const std::vector<double>& Get_Iterations();

        const std::vector<double>& Get_MaxErrors();
        const std::vector<double>& Get_MeanErrors();
        const std::vector<double>& Get_RMS_Errors();

        const std::vector<double>& Get_MaxResiduals();
        const std::vector<double>& Get_MeanResiduals();
        const std::vector<double>& Get_RMS_Residuals();

        double Get_Iterations_Min();
        double Get_Iterations_Max();

        double Get_MaxErrors_Min();
        double Get_MaxErrors_Max();

        double Get_MeanErrors_Min();
        double Get_MeanErrors_Max();

        double Get_RMS_Errors_Min();
        double Get_RMS_Errors_Max();

        double Get_MaxResiduals_Min();
        double Get_MaxResiduals_Max();

        double Get_MeanResiduals_Min();
        double Get_MeanResiduals_Max();

        double Get_RMS_Residuals_Min();
        double Get_RMS_Residuals_Max();

    private:
        std::vector<double> _x_values;
        std::vector<double> _y_values;
        std::vector<double> _temperature_values;
        std::vector<double> _residual_values;

        int _rows;
        int _cols;

        double _x_min;
        double _x_max;

        double _y_min;
        double _y_max;

        double _temperature_min;
        double _temperature_max;

        double _residual_min;
        double _residual_max;

        std::vector<double> _iterations;

        std::vector<double> _maxErrors;
        std::vector<double> _meanErrors;
        std::vector<double> _RMS_Errors;

        std::vector<double> _maxResiduals;
        std::vector<double> _meanResiduals;
        std::vector<double> _RMS_Residuals;

        double _iterations_min;
        double _iterations_max;

        double _maxErrors_min;
        double _maxErrors_max;

        double _meanErrors_min;
        double _meanErrors_max;

        double _RMS_Errors_min;
        double _RMS_Errors_max;

        double _maxResiduals_min;
        double _maxResiduals_max;

        double _meanResiduals_min;
        double _meanResiduals_max;

        double _RMS_Residuals_min;
        double _RMS_Residuals_max;
    };
}