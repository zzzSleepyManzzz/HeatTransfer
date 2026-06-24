#include <catch2/catch_all.hpp>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/SimulationRunner/SolverFactory.h"

namespace HeatTransfer::Tests
{
    TEST_CASE("Solver factory should give correct solver")
    {
        using enum HeatTransfer::Core::Method;

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

        auto solverFactory = std::make_shared<HeatTransfer::SimulationRunner::SolverFactory>(
            parameters, boundaryCondition);
        std::shared_ptr<HeatTransfer::Solvers::ISolver> solver;

        SECTION("Correct pointer for Jacobi Method")
        {
            solver = solverFactory->Create(JACOBI);
            auto jacobi = std::dynamic_pointer_cast<HeatTransfer::Solvers::JacobiMethod>(solver);
            REQUIRE(jacobi);
        }

        SECTION("Correct pointer for Gauss-Seidel Method")
        {
            solver = solverFactory->Create(GAUSS_SEIDEL);
            auto gaussSeidel =
                std::dynamic_pointer_cast<HeatTransfer::Solvers::GaussSeidelMethod>(solver);
            REQUIRE(gaussSeidel);
        }

        SECTION("Correct pointer for successive over relaxation")
        {
            solver = solverFactory->Create(SUCCESSIVE_OVER_RELAXATION);
            auto successiveOverRelaxation =
                std::dynamic_pointer_cast<HeatTransfer::Solvers::SuccessiveOverRelaxation>(solver);
            REQUIRE(successiveOverRelaxation);
        }
    }
}
