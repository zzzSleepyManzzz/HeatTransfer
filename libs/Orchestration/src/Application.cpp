#include "HeatTransfer/Orchestration/Application.h"

#include <tracy/Tracy.hpp>

namespace HeatTransfer::Orchestration
{
    Application::Application() {}

    void Application::Run()
    {
        HeatTransfer::Core::BoundaryConditions boundaryCondition =
            HeatTransfer::Core::BoundaryConditions::Default();

        HeatTransfer::Core::MaterialProperties materialProperties =
            HeatTransfer::Core::MaterialProperties::Default();

        HeatTransfer::Core::SimulationParameters parameters =
            HeatTransfer::Core::SimulationParameters::Default();

        auto simulationConfig = std::make_shared<HeatTransfer::SimulationRunner::SimulationConfig>(
            parameters, materialProperties, boundaryCondition);

        auto simulation =
            std::make_shared<HeatTransfer::SimulationRunner::Simulation>(simulationConfig);

        auto start = std::chrono::steady_clock::now();

        simulation->Run();

        auto end = std::chrono::steady_clock::now();
        auto durationInSeconds = std::chrono::duration<double>(end - start).count();

        auto simulationOutput = simulation->GetOutput();

        auto rendererModel = std::make_shared<HeatTransfer::Visualisation::RendererModel>(
            simulationConfig, simulationOutput, durationInSeconds);

        auto renderer = std::make_shared<HeatTransfer::Visualisation::Renderer>();

        auto asyncSimulation = std::make_shared<HeatTransfer::SimulationRunner::AsyncSimulation>();

        while (!renderer->ShouldClose())
        {
            // Update state

            if (rendererModel->NeedsSimulationUpdate())
            {
                asyncSimulation->Start(rendererModel->GetSimulationConfig());
                rendererModel->ClearSimulationUpdate();
            }

            if (asyncSimulation->IsComplete())
            {
                auto result = asyncSimulation->GetResult();

                rendererModel = std::make_shared<HeatTransfer::Visualisation::RendererModel>(
                    result.Config, result.Output, result.DurationInSeconds);
            }

            rendererModel->SetSimulationProgress(asyncSimulation->IsRunning(),
                                                 asyncSimulation->GetCurrentIteration());

            renderer->Render(rendererModel);
            renderer->PollEvents();

            FrameMark;
        }
    }
}