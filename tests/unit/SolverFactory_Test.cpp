#include <catch2/catch_all.hpp>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/MaterialProperties.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/SimulationRunner/SolverFactory.h"

#include "HeatTransfer/Solvers/IMultigrid.h"
#include "HeatTransfer/Solvers/Smoothers/GaussSeidel.h"
#include "HeatTransfer/Solvers/Smoothers/Jacobi.h"
#include "HeatTransfer/Solvers/Smoothers/RedBlackSOR.h"
#include "HeatTransfer/Solvers/Smoothers/SuccessiveOverRelaxation.h"

namespace HeatTransfer::Tests
{
    TEST_CASE("Solver factory should give correct solver")
    {
        using enum HeatTransfer::Core::Smoother;

        HeatTransfer::Core::BoundaryConditions boundaryCondition =
            HeatTransfer::Core::BoundaryConditions::Default();

        HeatTransfer::Core::MaterialProperties materialProperties =
            HeatTransfer::Core::MaterialProperties::Default();

        HeatTransfer::Core::SimulationParameters parameters = {.Rows = 100,
                                                               .Columns = 100,
                                                               .ResidualTolerance = 1e-1,
                                                               .ErrorTolerance = 1e-6,
                                                               .RelaxationFactor = 1.7,
                                                               .MaxIterations = 1000};

        auto smootherFor = [&](HeatTransfer::Core::Smoother type)
        {
            parameters.Smoother = type;

            HeatTransfer::SimulationRunner::SolverFactory solverFactory(
                parameters, materialProperties, boundaryCondition);

            auto solver = std::dynamic_pointer_cast<HeatTransfer::Solvers::IMultigrid>(
                solverFactory.Create());
            REQUIRE(solver);
            return solver->GetSmoother();
        };

        SECTION("Correct smoother for Jacobi Method")
        {
            auto smoother =
                std::dynamic_pointer_cast<const HeatTransfer::Solvers::Smoothers::Jacobi>(
                    smootherFor(JACOBI));
            REQUIRE(smoother);
        }

        SECTION("Correct smoother for Gauss-Seidel Method")
        {
            auto smoother =
                std::dynamic_pointer_cast<const HeatTransfer::Solvers::Smoothers::GaussSeidel>(
                    smootherFor(GAUSS_SEIDEL));
            REQUIRE(smoother);
        }

        SECTION("Correct smoother for successive over relaxation")
        {
            auto smoother = std::dynamic_pointer_cast<
                const HeatTransfer::Solvers::Smoothers::SuccessiveOverRelaxation>(
                smootherFor(SUCCESSIVE_OVER_RELAXATION));
            REQUIRE(smoother);
        }

        SECTION("Correct smoother for Red-Black SOR")
        {
            auto smoother =
                std::dynamic_pointer_cast<const HeatTransfer::Solvers::Smoothers::RedBlackSOR>(
                    smootherFor(RED_BLACK_SOR));
            REQUIRE(smoother);
        }
    }
}
