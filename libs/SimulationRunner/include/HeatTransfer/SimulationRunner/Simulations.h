#pragma once

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/Solvers/GaussSeidelMethod.h"
#include "HeatTransfer/Solvers/JacobiMethod.h"
#include "HeatTransfer/Solvers/SuccessiveOverRelaxation.h"

#include "HeatTransfer/SimulationRunner/Method.h"
#include "HeatTransfer/SimulationRunner/SimulationOutput.h"
#include "HeatTransfer/SimulationRunner/SolverFactory.h"

namespace HeatTransfer::SimulationRunner
{
    class Simulations
    {
    public:
        Simulations(const Core::SimulationParameters& parameters,
                    const Core::BoundaryConditions& boundaryCondition);
        void Run(Method method);
        void Print();

        std::shared_ptr<SimulationOutput> GetOutput();

    private:
        Core::SimulationParameters _parameters;
        Core::BoundaryConditions _boundaryCondition;
        std::shared_ptr<Solvers::ISolver> _solver;
    };
}