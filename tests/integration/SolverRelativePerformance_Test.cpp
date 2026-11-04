#include <catch2/catch_all.hpp>
#include <memory>
#include <stdexcept>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/MaterialProperties.h"
#include "HeatTransfer/Core/SimulationParameters.h"
#include "HeatTransfer/Core/Smoother.h"

#include "HeatTransfer/Solvers/MultigridPreconditionedConjugateGradientSolver.h"
#include "HeatTransfer/Solvers/Smoothers/GaussSeidel.h"
#include "HeatTransfer/Solvers/Smoothers/Jacobi.h"
#include "HeatTransfer/Solvers/Smoothers/RedBlackSOR.h"
#include "HeatTransfer/Solvers/Smoothers/SuccessiveOverRelaxation.h"

namespace
{
    int NumIterations(HeatTransfer::Core::Smoother type,
                      HeatTransfer::Core::SimulationParameters parameters,
                      HeatTransfer::Core::MaterialProperties materialProperties,
                      HeatTransfer::Core::BoundaryConditions boundaryCondition)
    {
        using enum HeatTransfer::Core::Smoother;

        std::shared_ptr<const HeatTransfer::Solvers::Smoothers::ISmoother> smoother;

        switch (type)
        {
            case JACOBI:
                smoother = std::make_shared<HeatTransfer::Solvers::Smoothers::Jacobi>();
                break;
            case GAUSS_SEIDEL:
                smoother = std::make_shared<HeatTransfer::Solvers::Smoothers::GaussSeidel>();
                break;
            case SUCCESSIVE_OVER_RELAXATION:
                smoother =
                    std::make_shared<HeatTransfer::Solvers::Smoothers::SuccessiveOverRelaxation>(
                        parameters.RelaxationFactor);
                break;
            case RED_BLACK_SOR:
                smoother = std::make_shared<HeatTransfer::Solvers::Smoothers::RedBlackSOR>(
                    parameters.RelaxationFactor);
                break;
            default:
                throw std::runtime_error("Unknown smoother");
        }

        auto solver =
            std::make_shared<HeatTransfer::Solvers::MultigridPreconditionedConjugateGradientSolver>(
                parameters, materialProperties, boundaryCondition, smoother);

        solver->ComputeSimulation();
        return solver->GetResidualMetrics().size();
    }
}

namespace HeatTransfer::Tests
{
    TEST_CASE("SOR converges faster than GS and Jacobi")
    {
        HeatTransfer::Core::BoundaryConditions boundaryCondition =
            HeatTransfer::Core::BoundaryConditions::Default();

        HeatTransfer::Core::MaterialProperties materialProperties =
            HeatTransfer::Core::MaterialProperties::Default();

        HeatTransfer::Core::SimulationParameters parameters = {.Rows = 100,
                                                               .Columns = 100,
                                                               .ResidualTolerance = 1e-1,
                                                               .RelaxationFactor = 1.7,
                                                               .MaxIterations = 1000};

        auto numIterationsJacobi = NumIterations(HeatTransfer::Core::Smoother::JACOBI,
                                                 parameters,
                                                 materialProperties,
                                                 boundaryCondition);

        auto numIterationsGaussSeidel = NumIterations(HeatTransfer::Core::Smoother::GAUSS_SEIDEL,
                                                      parameters,
                                                      materialProperties,
                                                      boundaryCondition);

        auto numIterationsSuccessiveOverRelaxation =
            NumIterations(HeatTransfer::Core::Smoother::SUCCESSIVE_OVER_RELAXATION,
                          parameters,
                          materialProperties,
                          boundaryCondition);

        REQUIRE(numIterationsGaussSeidel < numIterationsJacobi);
        REQUIRE(numIterationsSuccessiveOverRelaxation < numIterationsGaussSeidel);
    }
}
