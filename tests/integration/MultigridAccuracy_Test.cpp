#include <catch2/catch_all.hpp>

#include <cmath>
#include <numbers>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/MaterialProperties.h"
#include "HeatTransfer/Core/Method.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/SimulationRunner/SolverFactory.h"

namespace
{
    // Independent reference: plain SOR with the optimal factor, written out in full and run
    // until the scaled residual is below 1e-11
    Eigen::MatrixXd ReferenceSolution(int rows,
                                      int cols,
                                      const HeatTransfer::Core::MaterialProperties& material,
                                      const HeatTransfer::Core::BoundaryConditions& bc)
    {
        const int h1 = rows / 4, h2 = 3 * rows / 4, h3 = cols / 4, h4 = 3 * cols / 4;
        const int c1 = rows / 2 - 1, c2 = cols / 2 - 1;

        auto fixed = [&](int i, int j)
        {
            if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1)
                return true;
            if ((i == h1 || i == h2) && j >= h3 && j <= h4)
                return true;
            if ((j == h3 || j == h4) && i >= h1 && i <= h2)
                return true;
            return i == c1 && j == c2;
        };

        Eigen::MatrixXd T = Eigen::MatrixXd::Zero(rows, cols);
        T.row(0).setConstant(bc.TopEdge);
        T.row(rows - 1).setConstant(bc.BottomEdge);
        T.col(0).setConstant(bc.LeftEdge);
        T.col(cols - 1).setConstant(bc.RightEdge);
        for (int j = 1; j < cols - 1; j++)
            for (int i = 1; i < rows - 1; i++)
                if (fixed(i, j))
                    T(i, j) = (i == c1 && j == c2) ? bc.CenterPoint : bc.InnerSquare;

        const double h = 1.0 / std::max(rows, cols);
        const double source =
            0.25 * h * h * material.InternalHeatGeneration / material.ThermalConductivity;
        const double w = 2.0 / (1.0 + std::sin(std::numbers::pi / std::max(rows, cols)));

        for (int sweep = 0; sweep < 100000; sweep++)
        {
            double maxResidual = 0.0;

            for (int j = 1; j < cols - 1; j++)
            {
                for (int i = 1; i < rows - 1; i++)
                {
                    if (fixed(i, j))
                        continue;

                    const double target =
                        0.25 * (T(i - 1, j) + T(i + 1, j) + T(i, j - 1) + T(i, j + 1)) + source;
                    maxResidual = std::max(maxResidual, std::abs(target - T(i, j)));
                    T(i, j) += w * (target - T(i, j));
                }
            }

            if (maxResidual < 1e-11)
                break;
        }

        return T;
    }
}

namespace HeatTransfer::Tests
{
    TEST_CASE("Every method matches an independent converged solution")
    {
        using enum HeatTransfer::Core::Method;

        const auto method =
            GENERATE(JACOBI, GAUSS_SEIDEL, SUCCESSIVE_OVER_RELAXATION, RED_BLACK_SOR);

        auto material = Core::MaterialProperties::Default();
        material.InternalHeatGeneration = GENERATE(0.0, 5e7); // 0 skips the source term

        auto [rows, cols] = GENERATE(table<int, int>({{64, 64}, {75, 75}, {48, 80}}));

        const auto bc = Core::BoundaryConditions::Default();

        auto parameters = Core::SimulationParameters::Default();
        parameters.Rows = rows;
        parameters.Columns = cols;
        parameters.ExpandedResidualTolerance = 1e-10;
        parameters.ExpandedErrorTolerance = 0.0; // stop on the residual only
        parameters.MaxExpandedIterations = 2000;

        SimulationRunner::SolverFactory factory(parameters, material, bc);
        auto solver = factory.Create(method);
        solver->ComputeSimulation();

        REQUIRE(solver->IsConverged());

        const Eigen::MatrixXd reference = ReferenceSolution(rows, cols, material, bc);
        REQUIRE((*solver->GetTemperatureMatrix() - reference).cwiseAbs().maxCoeff() < 1e-6);
    }
}
