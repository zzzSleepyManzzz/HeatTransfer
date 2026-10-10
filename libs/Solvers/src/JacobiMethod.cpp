#include "HeatTransfer/Solvers/JacobiMethod.h"

namespace HeatTransfer::Solvers
{
    JacobiMethod::JacobiMethod(const Core::SimulationParameters& parameters,
                               const Core::MaterialProperties& materialProperties,
                               const Core::BoundaryConditions& boundaryCondition)
        : ISolver(parameters, materialProperties, boundaryCondition)
    {
    }

    void JacobiMethod::UpdateTemperatureMatrix(const Eigen::MatrixXd& oldTemperature)
    {
        const Eigen::Index rows = _temperatureMatrix->rows();
        const Eigen::Index cols = _temperatureMatrix->cols();

        auto q_g = _materialProperties.InternalHeatGeneration;
        auto k = _materialProperties.ThermalConductivity;
        auto heating = Eigen::MatrixXd::Constant(rows - 2, cols - 2, q_g / k);

        _temperatureMatrix->block(1, 1, rows - 2, cols - 2) =
            (oldTemperature.block(0, 1, rows - 2, cols - 2) +
             oldTemperature.block(2, 1, rows - 2, cols - 2) +
             oldTemperature.block(1, 0, rows - 2, cols - 2) +
             oldTemperature.block(1, 2, rows - 2, cols - 2) + heating) *
            0.25f;

        ApplyBoundaryConditions();
    }
}
