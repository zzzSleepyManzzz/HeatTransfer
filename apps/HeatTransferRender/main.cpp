#include <memory>

#include "HeatTransfer/SimulationRunner/Simulations.h"
#include "HeatTransfer/Visualisation/Renderer.h"

int main()
{
    using enum HeatTransfer::SimulationRunner::Method;

    using namespace HeatTransfer::Core;
    using namespace HeatTransfer::SimulationRunner;
    using namespace HeatTransfer::Visualisation;

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

    const auto& simulationState = simulations->GetState();

    auto renderer = std::make_shared<Renderer>();
    renderer->Init();
    renderer->Render(simulationState);
    renderer->Shutdown();

    return 0;
}