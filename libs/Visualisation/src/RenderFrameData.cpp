#include "HeatTransfer/Visualisation/RenderFrameData.h"

#include "HeatTransfer/Core/Tensor3dMapper.h"

namespace HeatTransfer::Visualisation
{
    RenderFrameData::RenderFrameData(
        std::shared_ptr<HeatTransfer::SimulationRunner::SimulationOutput> output)
    {
        _temperature = output->TemperatureTensor;
        _residual = output->ResidualTensor;

        const auto& errorMetrics = output->ErrorMetrics;
        const auto& residualMetrics = output->ResidualMetrics;

        const auto T = Core::Tensor3dMapper::AsVector(*_temperature);
        const auto R = Core::Tensor3dMapper::AsVector(*_residual);

        // Min and max over the whole field
        _temperature_min = T.minCoeff();
        _temperature_max = T.maxCoeff();

        _residual_min = R.minCoeff();
        _residual_max = R.maxCoeff();

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

    const Core::Tensor3d& RenderFrameData::GetTemperature()
    {
        return *_temperature;
    }

    const Core::Tensor3d& RenderFrameData::GetResidual()
    {
        return *_residual;
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