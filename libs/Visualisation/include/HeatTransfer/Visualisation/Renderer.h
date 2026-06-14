#pragma once

#include "HeatTransfer/SimulationRunner/SimulationState.h"

namespace HeatTransfer::Visualisation
{
    class Renderer
    {
    public:
        Renderer();
        void Init();
        void Render(const SimulationRunner::SimulationState& state);
        void Shutdown();

    private:
        // Implementation
    };
}