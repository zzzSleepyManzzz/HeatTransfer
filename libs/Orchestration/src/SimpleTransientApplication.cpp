#include "HeatTransfer/Orchestration/SimpleTransientApplication.h"

namespace HeatTransfer::Orchestration
{
    SimpleTransientApplication::SimpleTransientApplication() {}

    void SimpleTransientApplication::Run()
    {
        auto start = std::chrono::steady_clock::now();

        HeatTransfer::Core::BoundaryConditions boundaryCondition =
            HeatTransfer::Core::BoundaryConditions::Default();

        HeatTransfer::Core::InitialConditions initialConditions =
            HeatTransfer::Core::InitialConditions::Default();

        HeatTransfer::Core::MaterialProperties materialProperties =
            HeatTransfer::Core::MaterialProperties::Default();

        HeatTransfer::Core::SimulationParameters parameters =
            HeatTransfer::Core::SimulationParameters::Default();

        auto simulationConfig = std::make_shared<HeatTransfer::SimulationRunner::SimulationConfig>(
            parameters, materialProperties, boundaryCondition, initialConditions);

        auto simulation =
            std::make_shared<HeatTransfer::SimulationRunner::Simulation>(simulationConfig);

        simulation->Run();
        simulation->Print();

        auto end = std::chrono::steady_clock::now();
        auto duration = end - start;

        auto durationInSeconds = std::chrono::duration<double>(duration).count();
        auto durationInMilliseconds = std::chrono::duration<double, std::milli>(duration).count();

        std::cout << std::format("Elapsed time: {} seconds", durationInSeconds) << std::endl;
        std::cout << std::format("Elapsed time: {} milliseconds", durationInMilliseconds)
                  << std::endl;
    }
}