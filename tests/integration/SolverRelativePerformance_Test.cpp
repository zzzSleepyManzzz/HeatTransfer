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
        return solver->GetResidualMetrics().size();
    }
}

namespace HeatTransfer::Tests
{
    TEST_CASE("SOR converges faster than GS and Jacobi")
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
