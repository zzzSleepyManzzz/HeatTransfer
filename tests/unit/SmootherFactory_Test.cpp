#include <catch2/catch_all.hpp>

#include "HeatTransfer/Solver/Smoothers/GaussSeidel.h"
#include "HeatTransfer/Solver/Smoothers/Jacobi.h"
#include "HeatTransfer/Solver/Smoothers/RedBlackSOR.h"
#include "HeatTransfer/Solver/Smoothers/SmootherFactory.h"
#include "HeatTransfer/Solver/Smoothers/SuccessiveOverRelaxation.h"

namespace HeatTransfer::Tests
{
    TEST_CASE("Smoother factory should give correct smoother")
    {
        using enum HeatTransfer::Core::Smoother;
        using namespace HeatTransfer::Solver::Smoothers;

        auto create = [](HeatTransfer::Core::Smoother type)
        { return SmootherFactory(type, 1.7).Create(); };

        SECTION("Correct smoother for Jacobi Method")
        {
            REQUIRE(std::dynamic_pointer_cast<const Jacobi>(create(JACOBI)));
        }

        SECTION("Correct smoother for Gauss-Seidel Method")
        {
            REQUIRE(std::dynamic_pointer_cast<const GaussSeidel>(create(GAUSS_SEIDEL)));
        }

        SECTION("Correct smoother for successive over relaxation")
        {
            REQUIRE(std::dynamic_pointer_cast<const SuccessiveOverRelaxation>(
                create(SUCCESSIVE_OVER_RELAXATION)));
        }

        SECTION("Correct smoother for Red-Black SOR")
        {
            REQUIRE(std::dynamic_pointer_cast<const RedBlackSOR>(create(RED_BLACK_SOR)));
        }
    }
}
