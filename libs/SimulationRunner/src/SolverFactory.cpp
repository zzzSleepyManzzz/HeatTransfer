#include "HeatTransfer/SimulationRunner/SolverFactory.h"

#include <stdexcept>

#include "HeatTransfer/Solvers/MultigridPreconditionedConjugateGradientSolver.h"
#include "HeatTransfer/Solvers/Smoothers/GaussSeidel.h"
#include "HeatTransfer/Solvers/Smoothers/Jacobi.h"
#include "HeatTransfer/Solvers/Smoothers/RedBlackSOR.h"
#include "HeatTransfer/Solvers/Smoothers/SuccessiveOverRelaxation.h"

namespace HeatTransfer::SimulationRunner
{
    SolverFactory::SolverFactory(const Core::SimulationParameters& parameters,
                                 const Core::MaterialProperties& materialProperties,
                                 const Core::BoundaryConditions& boundaryCondition)
        : _parameters(parameters)
        , _materialProperties(materialProperties)
        , _boundaryCondition(boundaryCondition)
    {
    }

    std::shared_ptr<Solvers::ISolver> SolverFactory::Create()
    {
        using enum HeatTransfer::Core::Smoother;

        std::shared_ptr<const Solvers::Smoothers::ISmoother> smoother;

        switch (_parameters.Smoother)
        {
            case JACOBI:
                smoother = std::make_shared<Solvers::Smoothers::Jacobi>();
                break;
            case GAUSS_SEIDEL:
                smoother = std::make_shared<Solvers::Smoothers::GaussSeidel>();
                break;
            case SUCCESSIVE_OVER_RELAXATION:
                smoother = std::make_shared<Solvers::Smoothers::SuccessiveOverRelaxation>(
                    _parameters.RelaxationFactor);
                break;
            case RED_BLACK_SOR:
                smoother =
                    std::make_shared<Solvers::Smoothers::RedBlackSOR>(_parameters.RelaxationFactor);
                break;
            default:
                throw std::runtime_error("Provided non-existant smoother");
        };

        return std::make_shared<Solvers::MultigridPreconditionedConjugateGradientSolver>(
            _parameters, _materialProperties, _boundaryCondition, smoother);
    }
}
