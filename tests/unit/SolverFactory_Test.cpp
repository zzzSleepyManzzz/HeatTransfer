#include <catch2/catch_all.hpp>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/SimulationRunner/SolverFactory.h"

namespace HeatTransfer::Tests
{
    TEST_CASE("Solver factory should give correct solver")
    {
        using enum HeatTransfer::Core::Method;

        HeatTransfer::Core::BoundaryConditions boundaryCondition = {.TopEdge = 100.0,
                                                                    .BottomEdge = 0.0,
                                                                    .LeftEdge = 100.0,
                                                                    .RightEdge = 0.0,
                                                                    .InnerSquare = 50.0,
                                                                    .CenterPoint = -100.0};

        HeatTransfer::Core::SimulationParameters parameters = {.Rows = 100,
                                                               .Columns = 100,
                                                               .Expansion = 2,
                                                               .NumExpansions = 2,
                                                               .InitialTolerance = 1e-3,
                                                               .ExpandedTolerance = 1e-1,
                                                               .RelaxationFactor = 1.7,
                                                               .MaxInitialIterations = 1000,
                                                               .MaxExpandedIterations = 1000};

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
