#pragma once

namespace HeatTransfer::Core
{
    struct SimulationParameters
    {
        int rows;
        int columns;
        int expansion;
        int numExpansions;
        double initialTolerance;
        double expandedTolerance;
        double relaxationFactor;
        int maxInitialIterations;
        int maxExpandedIterations;
    };
}