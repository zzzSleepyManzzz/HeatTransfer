#include "HeatTransfer/SimulationRunner/Simulations.h"

namespace HeatTransfer::SimulationRunner
{
    Simulations::Simulations(std::shared_ptr<SimulationConfig> config)
        : _parameters(config->parameters)
        , _boundaryCondition(config->boundaryConditions)
        , _method(config->method)
    {
    }

    void Simulations::Run()
    {
        auto solverFactory = std::make_shared<SolverFactory>(_parameters, _boundaryCondition);
        _solver = solverFactory->Create(_method);
        _solver->ComputeSimulation();
    }

    void Simulations::Print()
    {
        _solver->PrintError();
    }

    std::shared_ptr<SimulationOutput> Simulations::GetOutput()
    {
        auto temperatureMatrix = _solver->GetTemperatureMatrix();
        const auto& errors = _solver->GetErrors();

        return std::make_shared<SimulationOutput>(temperatureMatrix, errors);
    }
}
