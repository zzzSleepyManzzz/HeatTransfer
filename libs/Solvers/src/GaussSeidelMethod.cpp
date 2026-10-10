#include "HeatTransfer/Solvers/GaussSeidelMethod.h"

namespace HeatTransfer::Solvers
{
    GaussSeidelMethod::GaussSeidelMethod(const Core::SimulationParameters& parameters,
                                         const Core::MaterialProperties& materialProperties,
                                         const Core::BoundaryConditions& boundaryCondition)
        : ISolver(parameters, materialProperties, boundaryCondition)
    {
    }

    void GaussSeidelMethod::UpdateTemperatureMatrix(const Eigen::MatrixXd& oldTemperature)
    {
        const Eigen::Index rows = _temperatureMatrix->rows();
        const Eigen::Index cols = _temperatureMatrix->cols();

        auto q_g = _materialProperties.InternalHeatGeneration;
        auto k = _materialProperties.ThermalConductivity;

        auto& T = *_temperatureMatrix;

        for (auto i = 0u; i < rows; i++)
        {
            for (auto j = 0u; j < cols; j++)
            {
                if (IsInsulated(i, j))
                {
                    T(i, j) = InsulatedValue(i, j);
                }
                else
                {
                    T(i, j) =
                        0.25f * (T(i - 1, j) + T(i + 1, j) + T(i, j - 1) + T(i, j + 1) + q_g / k);
                }
            }
        }
    }
}
