#include <catch2/catch_all.hpp>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/Method.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/SimulationRunner/Simulations.h"

namespace
{
    int NumIterations(HeatTransfer::Core::Method method,
                      HeatTransfer::Core::SimulationParameters parameters,
                      HeatTransfer::Core::BoundaryConditions boundaryCondition)
    {
        using enum HeatTransfer::Core::Method;

        std::shared_ptr<HeatTransfer::Solvers::ISolver> solver;

        switch (method)
        {
            case JACOBI:
                solver = std::make_shared<HeatTransfer::Solvers::JacobiMethod>(parameters,
                                                                               boundaryCondition);
                break;
            case GAUSS_SEIDEL:
                solver = std::make_shared<HeatTransfer::Solvers::GaussSeidelMethod>(
                    parameters, boundaryCondition);
                break;
            case SUCCESSIVE_OVER_RELAXATION:
                solver = std::make_shared<HeatTransfer::Solvers::SuccessiveOverRelaxation>(
                    parameters, boundaryCondition);
                break;
            default:
                throw std::runtime_error("Unknown method");
        }

        solver->ComputeSimulation();
        return solver->GetErrors().size();
    }
}

namespace HeatTransfer::Tests
{
    TEST_CASE("SOR converges faster than GS and Jacobi")
    {
        HeatTransfer::Core::BoundaryConditions boundaryCondition = {.topEdge = 100.0,
                                                                    .bottomEdge = 0.0,
                                                                    .leftEdge = 100.0,
                                                                    .rightEdge = 0.0,
                                                                    .innerSquare = 50.0,
                                                                    .centerPoint = -100.0};

        HeatTransfer::Core::SimulationParameters parameters = {.rows = 100,
                                                               .columns = 100,
                                                               .expansion = 2,
                                                               .numExpansions = 2,
                                                               .initialTolerance = 1e-3,
                                                               .expandedTolerance = 1e-1,
                                                               .relaxationFactor = 1.7,
                                                               .maxInitialIterations = 1000,
                                                               .maxExpandedIterations = 1000};

        auto numIterationsJacobi =
            NumIterations(HeatTransfer::Core::Method::JACOBI, parameters, boundaryCondition);

        auto numIterationsGaussSeidel =
            NumIterations(HeatTransfer::Core::Method::GAUSS_SEIDEL, parameters, boundaryCondition);

        auto numIterationsSuccessiveOverRelaxation = NumIterations(
            HeatTransfer::Core::Method::SUCCESSIVE_OVER_RELAXATION, parameters, boundaryCondition);

        REQUIRE(numIterationsGaussSeidel < numIterationsJacobi);
        REQUIRE(numIterationsSuccessiveOverRelaxation < numIterationsGaussSeidel);
    }
}
