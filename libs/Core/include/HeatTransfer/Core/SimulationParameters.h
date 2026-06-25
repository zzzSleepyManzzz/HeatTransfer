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

        constexpr static const SimulationParameters Default()
        {
            return {.rows = 2000,
                    .columns = 2000,
                    .expansion = 2,
                    .numExpansions = 5,
                    .initialTolerance = 1e-3,
                    .expandedTolerance = 1e-1,
                    .relaxationFactor = 1.7,
                    .maxInitialIterations = 1000,
                    .maxExpandedIterations = 1000};
        }
    };
}