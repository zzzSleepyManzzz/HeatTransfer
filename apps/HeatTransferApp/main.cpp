#include <chrono>
#include <format>
#include <iostream>
#include <memory>

#include "HeatTransfer/SimulationRunner/Simulations.h"

int main()
{
    auto start = std::chrono::steady_clock::now();

    using enum HeatTransfer::SimulationRunner::Method;

    using namespace HeatTransfer::Core;
    using namespace HeatTransfer::SimulationRunner;

    BoundaryConditions boundaryCondition = {.topEdge = 100.0,
                                            .bottomEdge = 0.0,
                                            .leftEdge = 100.0,
                                            .rightEdge = 0.0,
                                            .innerSquare = 50.0,
                                            .centerPoint = -100.0};

    SimulationParameters parameters = {.rows = 2000,
                                       .columns = 2000,
                                       .expansion = 2,
                                       .numExpansions = 5,
                                       .initialTolerance = 1e-3,
                                       .expandedTolerance = 1e-1,
                                       .relaxationFactor = 1.7,
                                       .maxInitialIterations = 1000,
                                       .maxExpandedIterations = 1000};

    auto simulations = std::make_shared<Simulations>(parameters, boundaryCondition);
    simulations->Run(SUCCESSIVE_OVER_RELAXATION);
    simulations->Print();

    auto end = std::chrono::steady_clock::now();
    auto duration = end - start;

    auto durationInSeconds = std::chrono::duration<double>(duration).count();
    auto durationInMilliseconds = std::chrono::duration<double, std::milli>(duration).count();

    std::cout << std::format("Elasped time: {} seconds", durationInSeconds) << std::endl;
    std::cout << std::format("Elasped time: {} milliseconds", durationInMilliseconds) << std::endl;

    system("pause > 0");
    return 0;
}