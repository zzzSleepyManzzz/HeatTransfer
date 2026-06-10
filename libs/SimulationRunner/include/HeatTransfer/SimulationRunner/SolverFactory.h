#pragma once

#include <memory>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/Solvers/GaussSeidelMethod.h"
#include "HeatTransfer/Solvers/ISolver.h"
#include "HeatTransfer/Solvers/JacobiMethod.h"
#include "HeatTransfer/Solvers/SuccessiveOverRelaxation.h"

#include "HeatTransfer/SimulationRunner/Method.h"

namespace HeatTransfer::SimulationRunner
{
    class SolverFactory
    {
    public:
        SolverFactory(const Core::SimulationParameters& parameters,
                      const Core::BoundaryConditions& boundaryCondition);
        std::shared_ptr<Solvers::ISolver> Create(Method method);

    private:
        Core::SimulationParameters _parameters;
        Core::BoundaryConditions _boundaryCondition;
    };
}