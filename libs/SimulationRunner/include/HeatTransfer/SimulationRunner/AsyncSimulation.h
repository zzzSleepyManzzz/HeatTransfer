#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>

#include "HeatTransfer/SimulationRunner/Simulation.h"
#include "HeatTransfer/SimulationRunner/SimulationConfig.h"
#include "HeatTransfer/SimulationRunner/SimulationResult.h"

// Asynchronous wrapper for simulation class

namespace HeatTransfer::SimulationRunner
{
    class AsyncSimulation
    {
    public:
        AsyncSimulation() = default;
        ~AsyncSimulation() = default;

        AsyncSimulation(const AsyncSimulation&) = delete;
        AsyncSimulation& operator=(const AsyncSimulation&) = delete;

        void Start(std::shared_ptr<SimulationConfig> config);

        bool IsRunning() const;
        int GetCurrentIteration() const;

        bool IsComplete();
        SimulationResult GetResult();

    private:
        std::shared_ptr<Simulation> _simulation;

        std::atomic<bool> _isRunning = false;

        std::mutex _resultMutex;
        std::optional<SimulationResult> _result;

        std::jthread _thread; // Must be last
    };
}
