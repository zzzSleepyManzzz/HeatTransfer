#include "HeatTransfer/SimulationRunner/AsyncSimulation.h"

#include <chrono>
#include <stdexcept>
#include <utility>

#include <tracy/Tracy.hpp>

namespace HeatTransfer::SimulationRunner
{
    void AsyncSimulation::Start(std::shared_ptr<SimulationConfig> config)
    {
        ZoneScoped;

        //  Stop previous thread and reset result

        _thread.request_stop();

        if (_thread.joinable())
            _thread.join();

        {
            std::scoped_lock lock(_resultMutex);
            _result.reset();
        }

        // Start new thread

        auto configCopy = std::make_shared<SimulationConfig>(*config);
        _simulation = std::make_shared<Simulation>(configCopy);
        _isRunning = true;

        _thread = std::jthread(
            [this, simulation = _simulation, configCopy](std::stop_token stopToken)
            {
                const auto start = std::chrono::steady_clock::now();

                simulation->Run(stopToken);

                if (stopToken.stop_requested())
                    return;

                const auto end = std::chrono::steady_clock::now();
                const double durationInSeconds = std::chrono::duration<double>(end - start).count();

                auto output = simulation->GetOutput();

                {
                    std::scoped_lock lock(_resultMutex);
                    _result = SimulationResult{configCopy, output, durationInSeconds};
                }

                _isRunning = false;
            });
    }

    bool AsyncSimulation::IsRunning() const
    {
        return _isRunning;
    }

    int AsyncSimulation::GetCurrentIteration() const
    {
        return _simulation ? _simulation->GetCurrentIteration() : 0;
    }

    bool AsyncSimulation::IsComplete()
    {
        std::scoped_lock lock(_resultMutex);
        return _result.has_value();
    }

    SimulationResult AsyncSimulation::GetResult()
    {
        std::scoped_lock lock(_resultMutex);

        if (!_result.has_value())
            throw std::logic_error("GetResult() called with no completed simulation");

        SimulationResult result = _result.value();
        _result.reset();

        return result;
    }
}
