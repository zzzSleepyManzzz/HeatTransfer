#include <catch2/catch_all.hpp>
#include <cmath>
#include <iostream>
#include <memory>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/Solvers/JacobiMethod.h"

namespace
{
    class DummySolver : public HeatTransfer::Solvers::ISolver
    {
    public:
        DummySolver(const HeatTransfer::Core::SimulationParameters& parameters,
                    const HeatTransfer::Core::BoundaryConditions& boundaryCondition)
            : ISolver(parameters, boundaryCondition)
        {
        }

        void ComputeSimulation() override {}

        void ExpandDummyMatrix()
        {
            ExpandMatrix();
        }

        void SetTemperatureMatrix(const Eigen::MatrixXd& matrix)
        {
            _temperatureMatrix = std::make_shared<Eigen::MatrixXd>(matrix);
        }

        std::shared_ptr<Eigen::MatrixXd> GetTemperatureMatrix()
        {
            return _temperatureMatrix;
        }
    };
}

namespace HeatTransfer::Tests
{
    TEST_CASE("Verifying ExpandMatrix in ISolver")
    {
        HeatTransfer::Core::BoundaryConditions boundaryCondition = {.topEdge = 100.0,
                                                                    .bottomEdge = 0.0,
                                                                    .leftEdge = 100.0,
                                                                    .rightEdge = 0.0,
                                                                    .innerSquare = 50.0,
                                                                    .centerPoint = -100.0};

        HeatTransfer::Core::SimulationParameters parameters = {.rows = 2,
                                                               .columns = 2,
                                                               .expansion = 2,
                                                               .numExpansions = 2,
                                                               .initialTolerance = 1e-3,
                                                               .expandedTolerance = 1e-1,
                                                               .relaxationFactor = 1.7,
                                                               .maxInitialIterations = 1000,
                                                               .maxExpandedIterations = 1000};

        auto solver = std::make_shared<DummySolver>(parameters, boundaryCondition);

        auto dummyMatrix = (Eigen::MatrixXd(2, 2) << 1, 2, 3, 4).finished();

        solver->SetTemperatureMatrix(dummyMatrix);

        auto originalMatrix = solver->GetTemperatureMatrix();

        REQUIRE(originalMatrix->rows() == 2);
        REQUIRE(originalMatrix->cols() == 2);

        REQUIRE(originalMatrix->isApprox((Eigen::MatrixXd(2, 2) << 1, 2, 3, 4).finished()));

        solver->ExpandDummyMatrix();

        auto newMatrix = solver->GetTemperatureMatrix();

        REQUIRE(newMatrix->rows() == 4);
        REQUIRE(newMatrix->cols() == 4);

        REQUIRE(newMatrix->isApprox(
            (Eigen::MatrixXd(4, 4) << 1, 1, 2, 2, 1, 1, 2, 2, 3, 3, 4, 4, 3, 3, 4, 4).finished()));
    }
}
