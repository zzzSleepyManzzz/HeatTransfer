#include <catch2/catch_all.hpp>
#include <cmath>
#include <iostream>
#include <memory>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/Solvers/GaussSeidelMethod.h"

namespace HeatTransfer::Tests
{
    TEST_CASE("Gauss-Seidel Method converges to requested tolerance")
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

        auto solver = std::make_shared<HeatTransfer::Solvers::GaussSeidelMethod>(parameters,
                                                                                 boundaryCondition);
        solver->ComputeSimulation();

        const auto& errors = solver->GetErrors();

        REQUIRE_FALSE(errors.empty());

        SECTION("Sanity check for errors array")
        {
            for (auto i = 1u; i < errors.size(); i++)
            {
                REQUIRE(errors[i].iteration > errors[i - 1].iteration);
                REQUIRE(errors[i].errorMax >= 0.0);
                REQUIRE(errors[i].errorMean >= 0.0);
                REQUIRE(errors[i].errorRMS >= 0.0);
            }
        }

        SECTION("Max error should be below tolerance")
        {
            REQUIRE(errors.back().errorMax <= parameters.expandedTolerance);
        }
    }

    TEST_CASE("Gauss-Seidel Method with Dirichlet Conditions compared with analytical solution")
    {
        // Not implemented yet
    }
}
