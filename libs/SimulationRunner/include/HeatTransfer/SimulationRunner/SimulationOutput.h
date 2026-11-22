#pragma once

#include <memory>
#include <vector>

#include "HeatTransfer/Core/ErrorMetric.h"
#include "HeatTransfer/Core/ResidualMetric.h"
#include "HeatTransfer/Core/Tensor3d.h"

namespace HeatTransfer::SimulationRunner
{
    struct SimulationOutput
    {
        std::shared_ptr<HeatTransfer::Core::Tensor3d> TemperatureTensor;
        std::vector<std::shared_ptr<HeatTransfer::Core::ErrorMetric>> ErrorMetrics;
        std::shared_ptr<HeatTransfer::Core::Tensor3d> ResidualTensor;
        std::vector<std::shared_ptr<HeatTransfer::Core::ResidualMetric>> ResidualMetrics;
        bool IsConverged;
    };
}
