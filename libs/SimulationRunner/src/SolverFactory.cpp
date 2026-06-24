#include "HeatTransfer/SimulationRunner/SolverFactory.h"

namespace HeatTransfer::SimulationRunner
{
    SolverFactory::SolverFactory(const Core::SimulationParameters& parameters,
                                 const Core::BoundaryConditions& boundaryCondition)
        : _parameters(parameters), _boundaryCondition(boundaryCondition)
    {
    }

    std::shared_ptr<Solvers::ISolver> SolverFactory::Create(HeatTransfer::Core::Method method)
    {
        using enum HeatTransfer::Core::Method;

        std::shared_ptr<Solvers::ISolver> solver;

        switch (method)
        {
            case JACOBI:
                solver = std::make_shared<Solvers::JacobiMethod>(_parameters, _boundaryCondition);
                break;
            case GAUSS_SEIDEL:
                solver =
                    std::make_shared<Solvers::GaussSeidelMethod>(_parameters, _boundaryCondition);
                break;
            case SUCCESSIVE_OVER_RELAXATION:
                solver = std::make_shared<Solvers::SuccessiveOverRelaxation>(_parameters,
                                                                             _boundaryCondition);
                break;
            default:
                throw std::runtime_error("Provided non-existant iterative method");
        };

        return solver;
    }
}
