#pragma once

#include <Eigen/Dense>

namespace HeatTransfer::SimulationRunner
{
    struct SimulationState
    {
        Eigen::MatrixXd TemperatureMatrix;
    };
}
