#pragma once

#include <Eigen/Dense>

#include "HeatTransfer/Core/IterationAndError.h"

namespace HeatTransfer::SimulationRunner
{
    struct SimulationState
    {
        Eigen::MatrixXd TemperatureMatrix;
        std::vector<HeatTransfer::Core::IterationAndError> errors;
        std::vector<double> field;
        size_t rows;
        size_t cols;
    };
}
