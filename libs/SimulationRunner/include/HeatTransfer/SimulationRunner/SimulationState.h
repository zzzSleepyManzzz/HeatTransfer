#pragma once

#include <Eigen/Dense>

namespace HeatTransfer::SimulationRunner
{
    struct SimulationState
    {
        Eigen::MatrixXd TemperatureMatrix;
        std::vector<double> field;
        size_t rows;
        size_t cols;
    };
}
