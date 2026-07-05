#pragma once

#include <Eigen/Dense>

#include "HeatTransfer/Core/ErrorMetric.h"
#include "HeatTransfer/Core/ResidualMetric.h"

namespace HeatTransfer::SimulationRunner
{
    struct SimulationOutput
    {
        std::shared_ptr<Eigen::MatrixXd> TemperatureMatrix;
        std::vector<std::shared_ptr<HeatTransfer::Core::ErrorMetric>> ErrorMetrics;
        std::shared_ptr<Eigen::MatrixXd> ResidualMatrix;
        std::vector<std::shared_ptr<HeatTransfer::Core::ResidualMetric>> ResidualMetrics;
        bool IsConverged;
    };
}
