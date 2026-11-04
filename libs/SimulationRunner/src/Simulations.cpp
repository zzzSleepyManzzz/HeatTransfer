#include "HeatTransfer/SimulationRunner/Simulations.h"

#include <tracy/Tracy.hpp>

#include "HeatTransfer/Solvers/MultigridPreconditionedConjugateGradientSolver.h"
#include "HeatTransfer/Solvers/Smoothers/SmootherFactory.h"

namespace HeatTransfer::SimulationRunner
{
    Simulations::Simulations(std::shared_ptr<SimulationConfig> config)
        : _parameters(config->Parameters)
        , _materialProperties(config->MaterialProperties)
        , _boundaryCondition(config->BoundaryConditions)
    {
    }

    void Simulations::Run()
    {
        ZoneScoped;

        Solvers::Smoothers::SmootherFactory smootherFactory(_parameters.Smoother,
                                                            _parameters.RelaxationFactor);
        auto smoother = smootherFactory.Create();

        _solver = std::make_shared<Solvers::MultigridPreconditionedConjugateGradientSolver>(
            _parameters, _materialProperties, _boundaryCondition, smoother);

        _solver->ComputeSimulation();
    }

    void Simulations::Print()
    {
        _solver->PrintResidual();
    }

    std::shared_ptr<SimulationOutput> Simulations::GetOutput()
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
