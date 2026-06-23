#include "HeatTransfer/Visualisation/RenderFrameData.h"

namespace HeatTransfer::Visualisation
{
    RenderFrameData::RenderFrameData(
        std::shared_ptr<HeatTransfer::SimulationRunner::SimulationOutput> output)
    {
        auto temperatureMatrix = output->TemperatureMatrix;
        const auto& errors = output->Errors;

        auto& T = *temperatureMatrix;

        // ImGui by default uses 16-bit indexing, which limits vertex indices to 65,535
        // Target ~5,000 max vertices instead of 65,535 to account for the index multiplier
        int total_elements = (int)temperatureMatrix->rows() * (int)temperatureMatrix->cols();
        int target_num_elements = 5000;
        int stride = std::max(
            1, (int)std::ceil(std::sqrt((double)total_elements / (double)target_num_elements)));

        _rows = (temperatureMatrix->rows() + stride - 1) / stride;
        _cols = (temperatureMatrix->cols() + stride - 1) / stride;

        _z_values.reserve(_rows * _cols);

        for (auto i = 0u; i < temperatureMatrix->rows(); i += stride)
        {
            for (auto j = 0u; j < temperatureMatrix->cols(); j += stride)
            {
                _z_values.push_back(T(i, j));
            }
        }

        // Populate other values

        int N = _cols * _rows;

        _x_values = std::vector<double>(N, 0);
        _y_values = std::vector<double>(N, 0);

        _x_min = 0;
        _x_max = temperatureMatrix->cols();

        _y_min = 0;
        _y_max = temperatureMatrix->rows();

        _z_min = *std::min_element(_z_values.begin(), _z_values.end());
        _z_max = *std::max_element(_z_values.begin(), _z_values.end());

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

        // Populate error related information

        for (auto error : errors)
        {
            _iterations.push_back(error->iteration);
            _maxErrors.push_back(error->errorMax);
            _meanErrors.push_back(error->errorMean);
            _RMS_Errors.push_back(error->errorRMS);
        }
    }

    const std::vector<double>& RenderFrameData::Get_X_Values()
    {
        return _x_values;
    }

    const std::vector<double>& RenderFrameData::Get_Y_Values()
    {
        return _y_values;
    }

    const std::vector<double>& RenderFrameData::Get_Z_Values()
    {
        return _z_values;
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

    double RenderFrameData::Get_Z_Min()
    {
        return _z_min;
    }

    double RenderFrameData::Get_Z_Max()
    {
        return _z_max;
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
}