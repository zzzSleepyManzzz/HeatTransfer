#include "HeatTransfer/Orchestration/SimpleApplication.h"

namespace HeatTransfer::Orchestration
{
    SimpleApplication::SimpleApplication() {}

    void SimpleApplication::Run()
    {
        auto start = std::chrono::steady_clock::now();

        HeatTransfer::Core::BoundaryConditions boundaryCondition =
            HeatTransfer::Core::BoundaryConditions::Default();

        HeatTransfer::Core::SimulationParameters parameters =
            HeatTransfer::Core::SimulationParameters::Default();

        auto method = HeatTransfer::Core::Method::SUCCESSIVE_OVER_RELAXATION;

        auto simulationConfig = std::make_shared<HeatTransfer::SimulationRunner::SimulationConfig>(
            parameters, boundaryCondition, method);

        auto simulations =
            std::make_shared<HeatTransfer::SimulationRunner::Simulations>(simulationConfig);

        simulations->Run();
        simulations->Print();

        auto end = std::chrono::steady_clock::now();
        auto duration = end - start;

        auto durationInSeconds = std::chrono::duration<double>(duration).count();
        auto durationInMilliseconds = std::chrono::duration<double, std::milli>(duration).count();

        std::cout << std::format("Elasped time: {} seconds", durationInSeconds) << std::endl;
        std::cout << std::format("Elasped time: {} milliseconds", durationInMilliseconds)
                  << std::endl;

        system("pause > 0");
    }
}