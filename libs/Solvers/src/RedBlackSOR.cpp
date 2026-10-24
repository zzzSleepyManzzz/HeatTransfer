#include "HeatTransfer/Solvers/RedBlackSOR.h"

#include <tracy/Tracy.hpp>

namespace HeatTransfer::Solvers
{
    RedBlackSOR::RedBlackSOR(const Core::SimulationParameters& parameters,
                             const Core::MaterialProperties& materialProperties,
                             const Core::BoundaryConditions& boundaryCondition)
        : ISolver(parameters, materialProperties, boundaryCondition)
    {
    }

    void RedBlackSOR::UpdateTemperatureMatrix(const Eigen::MatrixXd& oldTemperature)
    {
        ZoneScoped;

        const Eigen::Index rows = _temperatureMatrix->rows();
        const Eigen::Index cols = _temperatureMatrix->cols();

        auto& T = *_temperatureMatrix;

        auto relaxationFactor = _parameters.RelaxationFactor;

        auto q_g = _materialProperties.InternalHeatGeneration;
        auto k = _materialProperties.ThermalConductivity;
        auto h = 1.0f / std::max(rows, cols);

        auto RED = 0;
        auto BLACK = 1;

        // Red squares

#pragma omp parallel for schedule(static)
        for (auto i = 0u; i < rows; i++)
        {
            for (auto j = 0u; j < cols; j++)
            {
                if ((i + j) % 2 == BLACK)
                    continue;

                if (IsInsulated(i, j))
                {
                    T(i, j) = InsulatedValue(i, j);
                }
                else
                {
                    T(i, j) = (1 - relaxationFactor) * T(i, j) +
                              relaxationFactor * 0.25f *
                                  (T(i - 1, j) + T(i + 1, j) + T(i, j - 1) + T(i, j + 1) +
                                   pow(h, 2) * q_g / k);
                }
            }
        }

        // Black squares

#pragma omp parallel for schedule(static)
        for (auto i = 0u; i < rows; i++)
        {
            for (auto j = 0u; j < cols; j++)
            {
                if ((i + j) % 2 == RED)
                    continue;

                if (IsInsulated(i, j))
                {
                    T(i, j) = InsulatedValue(i, j);
                }
                else
                {
                    T(i, j) = (1 - relaxationFactor) * T(i, j) +
                              relaxationFactor * 0.25f *
                                  (T(i - 1, j) + T(i + 1, j) + T(i, j - 1) + T(i, j + 1) +
                                   pow(h, 2) * q_g / k);
                }
            }
        }
    }
}
