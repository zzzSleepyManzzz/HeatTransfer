#include <catch2/catch_all.hpp>
#include <cmath>
#include <iostream>
#include <memory>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/Solvers/SuccessiveOverRelaxation.h"

namespace HeatTransfer::Tests
{
    TEST_CASE("Successive over relaxation converges to requested tolerance")
    {
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

        auto solver = std::make_shared<HeatTransfer::Solvers::SuccessiveOverRelaxation>(
            parameters, boundaryCondition);
        solver->ComputeSimulation();

        const auto& errors = solver->GetErrors();

        REQUIRE_FALSE(errors.empty());

        SECTION("Sanity check for errors array")
        {
            for (auto i = 1u; i < errors.size(); i++)
            {
                REQUIRE(errors[i]->Iteration > errors[i - 1]->Iteration);
                REQUIRE(errors[i]->ErrorMax >= 0.0);
                REQUIRE(errors[i]->ErrorMean >= 0.0);
                REQUIRE(errors[i]->ErrorRMS >= 0.0);
            }
        }

        SECTION("Max error should be below tolerance")
        {
            REQUIRE(errors.back()->ErrorMax <= parameters.ExpandedTolerance);
        }
    }

    TEST_CASE(
        "Successive over relaxation with Dirichlet Conditions compared with analytical solution")
    {
        // Not implemented yet
    }
}
