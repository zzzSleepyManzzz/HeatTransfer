#include "HeatTransfer/SimulationRunner/Simulations.h"

namespace HeatTransfer::SimulationRunner
{
    Simulations::Simulations(const Core::SimulationParameters& parameters,
                             const Core::BoundaryConditions& boundaryCondition)
        : _parameters(parameters), _boundaryCondition(boundaryCondition)
    {
    }

    void Simulations::Run(Method method)
    {
        auto solverFactory = std::make_shared<SolverFactory>(_parameters, _boundaryCondition);
        _solver = solverFactory->Create(method);
        _solver->ComputeSimulation();
    }

    void Simulations::Print()
    {
        _solver->PrintError();
    }

    SimulationState Simulations::GetState()
    {
        auto temperatureMatrix = _solver->GetTemperatureMatrix();

        int stride = 20;

        size_t rows = (temperatureMatrix.rows() + stride - 1) / stride;
        size_t cols = (temperatureMatrix.cols() + stride - 1) / stride;

        auto field = std::vector<double>{};
        field.reserve(rows * cols);

        for (auto i = 0u; i < temperatureMatrix.rows(); i += stride)
        {
            for (auto j = 0u; j < temperatureMatrix.cols(); j += stride)
            {
                field.push_back(temperatureMatrix(i, j));
            }
        }

        return SimulationState{
            .TemperatureMatrix = temperatureMatrix, .field = field, .rows = rows, .cols = cols};
    }
}
