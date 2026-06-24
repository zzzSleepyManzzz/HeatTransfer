#include "HeatTransfer/Orchestration/Application.h"

namespace HeatTransfer::Orchestration
{
    Application::Application() {}

    void Application::Run()
    {
        using enum HeatTransfer::SimulationRunner::Method;

        HeatTransfer::Core::BoundaryConditions boundaryCondition = {.topEdge = 100.0,
                                                                    .bottomEdge = 0.0,
                                                                    .leftEdge = 100.0,
                                                                    .rightEdge = 0.0,
                                                                    .innerSquare = 50.0,
                                                                    .centerPoint = -100.0};

        HeatTransfer::Core::SimulationParameters parameters = {.rows = 2000,
                                                               .columns = 2000,
                                                               .expansion = 2,
                                                               .numExpansions = 5,
                                                               .initialTolerance = 1e-3,
                                                               .expandedTolerance = 1e-1,
                                                               .relaxationFactor = 1.7,
                                                               .maxInitialIterations = 1000,
                                                               .maxExpandedIterations = 1000};

        auto simulations = std::make_shared<HeatTransfer::SimulationRunner::Simulations>(
            parameters, boundaryCondition);

        simulations->Run(SUCCESSIVE_OVER_RELAXATION);

        auto simulationOutput = simulations->GetOutput();
        auto rendererModel =
            std::make_shared<HeatTransfer::Visualisation::RendererModel>(simulationOutput);

        auto renderer = std::make_shared<HeatTransfer::Visualisation::Renderer>();

        while (!renderer->ShouldClose())
        {
            renderer->Render(rendererModel);
            renderer->PollEvents();
        }
    }
}