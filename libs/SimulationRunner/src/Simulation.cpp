#include "HeatTransfer/SimulationRunner/Simulation.h"

#include <tracy/Tracy.hpp>

#include "HeatTransfer/Solver/MultigridPreconditionedConjugateGradientSolver.h"

namespace HeatTransfer::SimulationRunner
{
    Simulation::Simulation(std::shared_ptr<SimulationConfig> config)
        : _parameters(config->Parameters)
        , _materialProperties(config->MaterialProperties)
        , _boundaryCondition(config->BoundaryConditions)
        , _solver(std::make_shared<Solver::MultigridPreconditionedConjugateGradientSolver>(
              _parameters, _materialProperties, _boundaryCondition))
    {
    }

    void Simulation::Run(std::stop_token stopToken)
    {
        ZoneScoped;

        _solver->ComputeSimulation(stopToken);
    }

    int Simulation::GetCurrentIteration() const
    {
        return _solver->GetCurrentIteration();
    }

    void Simulation::Print()
    {
        _solver->PrintResidual();
    }

    std::shared_ptr<SimulationOutput> Simulation::GetOutput()
    {
        auto temperatureMatrix = _solver->GetTemperatureMatrix();
        const auto& errorMetrics = _solver->GetErrorMetrics();

        auto residualMatrix = _solver->GetResidualMatrix();
        const auto& residualMetrics = _solver->GetResidualMetrics();

        bool isConverged = _solver->IsConverged();

        return std::make_shared<SimulationOutput>(
            temperatureMatrix, errorMetrics, residualMatrix, residualMetrics, isConverged);
    }
}
