#include <catch2/catch_all.hpp>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/MaterialProperties.h"
#include "HeatTransfer/Core/Method.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/SimulationRunner/Simulations.h"

namespace
{
    int NumIterations(HeatTransfer::Core::Method method,
                      HeatTransfer::Core::SimulationParameters parameters,
                      HeatTransfer::Core::MaterialProperties materialProperties,
                      HeatTransfer::Core::BoundaryConditions boundaryCondition)
    {
        using enum HeatTransfer::Core::Method;

        std::shared_ptr<HeatTransfer::Solvers::ISolver> solver;

        switch (method)
        {
            case JACOBI:
                solver = std::make_shared<HeatTransfer::Solvers::JacobiMethod>(
                    parameters, materialProperties, boundaryCondition);
                break;
            case GAUSS_SEIDEL:
                solver = std::make_shared<HeatTransfer::Solvers::GaussSeidelMethod>(
                    parameters, materialProperties, boundaryCondition);
                break;
            case SUCCESSIVE_OVER_RELAXATION:
                solver = std::make_shared<HeatTransfer::Solvers::SuccessiveOverRelaxation>(
                    parameters, materialProperties, boundaryCondition);
                break;
            default:
                throw std::runtime_error("Unknown method");
        }

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
                                                               .Expansion = 2,
                                                               .NumExpansions = 2,
                                                               .InitialResidualTolerance = 1e-3,
                                                               .ExpandedResidualTolerance = 1e-1,
                                                               .RelaxationFactor = 1.7,
                                                               .MaxInitialIterations = 1000,
                                                               .MaxExpandedIterations = 1000};

        auto numIterationsJacobi = NumIterations(
            HeatTransfer::Core::Method::JACOBI, parameters, materialProperties, boundaryCondition);

        auto numIterationsGaussSeidel = NumIterations(HeatTransfer::Core::Method::GAUSS_SEIDEL,
                                                      parameters,
                                                      materialProperties,
                                                      boundaryCondition);

        auto numIterationsSuccessiveOverRelaxation =
            NumIterations(HeatTransfer::Core::Method::SUCCESSIVE_OVER_RELAXATION,
                          parameters,
                          materialProperties,
                          boundaryCondition);

        REQUIRE(numIterationsGaussSeidel < numIterationsJacobi);
        REQUIRE(numIterationsSuccessiveOverRelaxation < numIterationsGaussSeidel);
    }
}
