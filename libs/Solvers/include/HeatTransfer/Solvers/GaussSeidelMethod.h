#pragma once

#include <Eigen/Dense>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/Solvers/ISolver.h"

namespace HeatTransfer::Solvers
{
    class GaussSeidelMethod : public ISolver
    {
    public:
        GaussSeidelMethod(const Core::SimulationParameters& parameters,
                          const Core::BoundaryConditions& boundaryCondition);

    private:
        void UpdateTemperatureMatrix(const Eigen::MatrixXd& oldTemperature) override;
    };
}