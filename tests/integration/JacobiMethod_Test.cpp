#include <catch2/catch_all.hpp>
#include <cmath>
#include <iostream>
#include <memory>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/Solvers/JacobiMethod.h"

namespace HeatTransfer::Tests
{
    TEST_CASE("Jacobi Method converges to requested tolerance")
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
                                                               .InitialResidualTolerance = 1e-3,
                                                               .ExpandedResidualTolerance = 1e-1,
                                                               .InitialErrorTolerance = 1e-6,
                                                               .ExpandedErrorTolerance = 1e-6,
                                                               .RelaxationFactor = 1.7,
                                                               .MaxInitialIterations = 1000,
                                                               .MaxExpandedIterations = 1000};

        auto solver =
            std::make_shared<HeatTransfer::Solvers::JacobiMethod>(parameters, boundaryCondition);

        solver->ComputeSimulation();

        const auto& residuals = solver->GetResidualMetrics();

        REQUIRE_FALSE(residuals.empty());

        SECTION("Sanity check for residuals array")
        {
            for (auto i = 1u; i < residuals.size(); i++)
            {
                REQUIRE(residuals[i]->Iteration > residuals[i - 1]->Iteration);
                REQUIRE(residuals[i]->ResidualMax >= 0.0);
                REQUIRE(residuals[i]->ResidualMean >= 0.0);
                REQUIRE(residuals[i]->ResidualRMS >= 0.0);
            }
        }

        SECTION("Max residual should be below tolerance")
        {
            REQUIRE(residuals.back()->ResidualMax <= parameters.ExpandedResidualTolerance);
        }
    }

    TEST_CASE("Jacobi Method with Dirichlet Conditions compared with analytical solution")
    {
        // Not implemented yet
    }
}
