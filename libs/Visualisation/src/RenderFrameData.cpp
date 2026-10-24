#include "HeatTransfer/Visualisation/RenderFrameData.h"

namespace HeatTransfer::Visualisation
{
    RenderFrameData::RenderFrameData(
        std::shared_ptr<HeatTransfer::SimulationRunner::SimulationOutput> output)
    {
        auto temperatureMatrix = output->TemperatureMatrix;
        const auto& errorMetrics = output->ErrorMetrics;

        auto residualMatrix = output->ResidualMatrix;
        const auto& residualMetrics = output->ResidualMetrics;

        auto& T = *temperatureMatrix;
        auto& R = *residualMatrix;

        // ImGui by default uses 16-bit indexing, which limits vertex indices to 65,535
        // Target ~5,000 max vertices instead of 65,535 to account for the index multiplier
        int total_elements = (int)T.rows() * (int)T.cols();
        int target_num_elements = 5000;
        int stride = std::max(
            1, (int)std::ceil(std::sqrt((double)total_elements / (double)target_num_elements)));

        _rows = (T.rows() + stride - 1) / stride;
        _cols = (T.cols() + stride - 1) / stride;

        _temperature_values.reserve(_rows * _cols);
        _residual_values.reserve(_rows * _cols);

        for (auto i = 0u; i < T.rows(); i += stride)
        {
            for (auto j = 0u; j < T.cols(); j += stride)
            {
                _temperature_values.push_back(T(i, j));
                _residual_values.push_back(R(i, j));
            }
        }

        // Populate other values

        int N = _cols * _rows;

        _x_values = std::vector<double>(N, 0);
        _y_values = std::vector<double>(N, 0);

        _x_min = 0;
        _x_max = T.cols();

        _y_min = 0;
        _y_max = T.rows();

        _temperature_min =
            *std::min_element(_temperature_values.begin(), _temperature_values.end());
        _temperature_max =
            *std::max_element(_temperature_values.begin(), _temperature_values.end());

        _residual_min = *std::min_element(_residual_values.begin(), _residual_values.end());
        _residual_max = *std::max_element(_residual_values.begin(), _residual_values.end());

        auto x_step = (_x_max - _x_min) / (_cols - 1);
        auto y_step = (_y_max - _y_min) / (_rows - 1);

        for (int i = 0; i < _rows; i++)
        {
            for (int j = 0; j < _cols; j++)
            {
                int index = i * _cols + j;
                _x_values[index] = _x_min + j * x_step;
                _y_values[index] = _y_min + i * y_step;
            }
        }

        // Populate error and residual related information

        for (auto errorMetric : errorMetrics)
        {
            _iterations.push_back(errorMetric->Iteration);
            _maxErrors.push_back(errorMetric->ErrorMax);
            _meanErrors.push_back(errorMetric->ErrorMean);
            _RMS_Errors.push_back(errorMetric->ErrorRMS);
        }

        for (auto residualMetric : residualMetrics)
        {
            _maxResiduals.push_back(residualMetric->ResidualMax);
            _meanResiduals.push_back(residualMetric->ResidualMean);
            _RMS_Residuals.push_back(residualMetric->ResidualRMS);
        }

        _iterations_min = *std::min_element(_iterations.begin(), _iterations.end());
        _iterations_max = *std::max_element(_iterations.begin(), _iterations.end());

        _maxErrors_min = *std::min_element(_maxErrors.begin(), _maxErrors.end());
        _maxErrors_max = *std::max_element(_maxErrors.begin(), _maxErrors.end());

        _meanErrors_min = *std::min_element(_meanErrors.begin(), _meanErrors.end());
        _meanErrors_max = *std::max_element(_meanErrors.begin(), _meanErrors.end());

        _RMS_Errors_min = *std::min_element(_RMS_Errors.begin(), _RMS_Errors.end());
        _RMS_Errors_max = *std::max_element(_RMS_Errors.begin(), _RMS_Errors.end());

        _maxResiduals_min = *std::min_element(_maxResiduals.begin(), _maxResiduals.end());
        _maxResiduals_max = *std::max_element(_maxResiduals.begin(), _maxResiduals.end());

        _meanResiduals_min = *std::min_element(_meanResiduals.begin(), _meanResiduals.end());
        _meanResiduals_max = *std::max_element(_meanResiduals.begin(), _meanResiduals.end());

        _RMS_Residuals_min = *std::min_element(_RMS_Residuals.begin(), _RMS_Residuals.end());
        _RMS_Residuals_max = *std::max_element(_RMS_Residuals.begin(), _RMS_Residuals.end());
    }

    const std::vector<double>& RenderFrameData::Get_X_Values()
    {
        return _x_values;
    }

    const std::vector<double>& RenderFrameData::Get_Y_Values()
    {
        return _y_values;
    }

    const std::vector<double>& RenderFrameData::Get_Temperature_Values()
    {
        return _temperature_values;
    }

    const std::vector<double>& RenderFrameData::Get_Residual_Values()
    {
        return _residual_values;
    }

    int RenderFrameData::GetRows()
    {
        return _rows;
    }

    int RenderFrameData::GetCols()
    {
        return _cols;
    }

    double RenderFrameData::Get_X_Min()
    {
        return _x_min;
    }

    double RenderFrameData::Get_X_Max()
    {
        return _x_max;
    }

    double RenderFrameData::Get_Y_Min()
    {
        return _y_min;
    }

    double RenderFrameData::Get_Y_Max()
    {
        return _y_max;
    }

    double RenderFrameData::Get_Temperature_Min()
    {
        return _temperature_min;
    }

    double RenderFrameData::Get_Temperature_Max()
    {
        return _temperature_max;
    }

    double RenderFrameData::Get_Residual_Min()
    {
        return _residual_min;
    }

    double RenderFrameData::Get_Residual_Max()
    {
        return _residual_max;
    }

    const std::vector<double>& RenderFrameData::Get_Iterations()
    {
        return _iterations;
    }

    const std::vector<double>& RenderFrameData::Get_MaxErrors()
    {
        return _maxErrors;
    }

    const std::vector<double>& RenderFrameData::Get_MeanErrors()
    {
        return _meanErrors;
    }

    const std::vector<double>& RenderFrameData::Get_RMS_Errors()
    {
        return _RMS_Errors;
    }

    const std::vector<double>& RenderFrameData::Get_MaxResiduals()
    {
        return _maxResiduals;
    }

    const std::vector<double>& RenderFrameData::Get_MeanResiduals()
    {
        return _meanResiduals;
    }

    const std::vector<double>& RenderFrameData::Get_RMS_Residuals()
    {
        return _RMS_Residuals;
    }

    double RenderFrameData::Get_Iterations_Min()
    {
        return _iterations_min;
    }

    double RenderFrameData::Get_Iterations_Max()
    {
        return _iterations_max;
    }

    double RenderFrameData::Get_MaxErrors_Min()
    {
        return _maxErrors_min;
    }

    double RenderFrameData::Get_MaxErrors_Max()
    {
        return _maxErrors_max;
    }

    double RenderFrameData::Get_MeanErrors_Min()
    {
        return _meanErrors_min;
    }

    double RenderFrameData::Get_MeanErrors_Max()
    {
        return _meanErrors_max;
    }

    double RenderFrameData::Get_RMS_Errors_Min()
    {
        return _RMS_Errors_min;
    }

    double RenderFrameData::Get_RMS_Errors_Max()
    {
        return _RMS_Errors_max;
    }

    double RenderFrameData::Get_MaxResiduals_Min()
    {
        return _maxResiduals_min;
    }

    double RenderFrameData::Get_MaxResiduals_Max()
    {
        return _maxResiduals_max;
    }

    double RenderFrameData::Get_MeanResiduals_Min()
    {
        return _meanResiduals_min;
    }

    double RenderFrameData::Get_MeanResiduals_Max()
    {
        return _meanResiduals_max;
    }

    double RenderFrameData::Get_RMS_Residuals_Min()
    {
        return _RMS_Residuals_min;
    }

    double RenderFrameData::Get_RMS_Residuals_Max()
    {
        return _RMS_Residuals_max;
    }
}