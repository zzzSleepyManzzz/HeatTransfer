#include <catch2/catch_all.hpp>
#include <memory>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/MaterialProperties.h"
#include "HeatTransfer/Core/SimulationParameters.h"
#include "HeatTransfer/Core/Smoother.h"

#include "HeatTransfer/Solvers/MultigridPreconditionedConjugateGradientSolver.h"

namespace
{
    int NumIterations(HeatTransfer::Core::Smoother type,
                      HeatTransfer::Core::SimulationParameters parameters,
                      HeatTransfer::Core::MaterialProperties materialProperties,
                      HeatTransfer::Core::BoundaryConditions boundaryCondition)
    {
        parameters.Smoother = type;

        auto solver =
            std::make_shared<HeatTransfer::Solvers::MultigridPreconditionedConjugateGradientSolver>(
                parameters, materialProperties, boundaryCondition);

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
