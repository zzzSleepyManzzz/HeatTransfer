#pragma once

namespace HeatTransfer::Core
{
    struct SimulationParameters
    {
        int Rows;
        int Columns;
        int Expansion;
        int NumExpansions;
        double InitialResidualTolerance;
        double ExpandedResidualTolerance;
        double RelaxationFactor;
        int MaxInitialIterations;
        int MaxExpandedIterations;

        constexpr static const SimulationParameters Default()
        {
            return {.Rows = 2000,
                    .Columns = 2000,
                    .Expansion = 2,
                    .NumExpansions = 5,
                    .InitialResidualTolerance = 1e-3,
                    .ExpandedResidualTolerance = 1e-1,
                    .RelaxationFactor = 1.7,
                    .MaxInitialIterations = 1000,
                    .MaxExpandedIterations = 1000};
        }
    };
}