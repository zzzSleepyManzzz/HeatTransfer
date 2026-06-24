#include "HeatTransfer/Orchestration/Application.h"

namespace HeatTransfer::Orchestration
{
    Application::Application() {}

    void Application::Run()
    {
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

        auto method = HeatTransfer::Core::Method::SUCCESSIVE_OVER_RELAXATION;

        auto simulationConfig = std::make_shared<HeatTransfer::SimulationRunner::SimulationConfig>(
            parameters, boundaryCondition, method);

        auto simulations =
            std::make_shared<HeatTransfer::SimulationRunner::Simulations>(simulationConfig);

        auto start = std::chrono::steady_clock::now();

        simulations->Run();

        auto end = std::chrono::steady_clock::now();
        auto durationInSeconds = std::chrono::duration<double>(end - start).count();

        auto simulationOutput = simulations->GetOutput();

        auto rendererModel = std::make_shared<HeatTransfer::Visualisation::RendererModel>(
            simulationConfig, simulationOutput, durationInSeconds);

        auto renderer = std::make_shared<HeatTransfer::Visualisation::Renderer>();

        while (!renderer->ShouldClose())
        {
            // Update state

            if (rendererModel->NeedsSimulationUpdate())
            {
                simulationConfig = rendererModel->GetSimulationConfig();

                simulations =
                    std::make_shared<HeatTransfer::SimulationRunner::Simulations>(simulationConfig);

                start = std::chrono::steady_clock::now();

                simulations->Run();

                end = std::chrono::steady_clock::now();
                durationInSeconds = std::chrono::duration<double>(end - start).count();

                simulationOutput = simulations->GetOutput();

                rendererModel = std::make_shared<HeatTransfer::Visualisation::RendererModel>(
                    simulationConfig, simulationOutput, durationInSeconds);
            }

            renderer->Render(rendererModel);
            renderer->PollEvents();
        }
    }
}