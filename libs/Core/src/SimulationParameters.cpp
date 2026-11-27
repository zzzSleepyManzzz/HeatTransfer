#include "HeatTransfer/Core/SimulationParameters.h"

namespace HeatTransfer::Core
{
    SimulationParameters SimulationParameters::Default()
    {
        return {.Rows = 100,
                .Columns = 100,
                .Layers = 100,
                .ResidualTolerance = 1e-3,
                .ErrorTolerance = 1e-6,
                .RelaxationFactor = 1.7,
                .MaxIterations = 1000,
                .ResidualTrackingMetric = Core::TrackingMetric::MAX,
                .ErrorTrackingMetric = Core::TrackingMetric::MAX,
                .Smoother = Core::Smoother::RED_BLACK_SOR,
                .GridSpacing = 0.01,
                .TimeStep = 100.0,
                .NumberOfTimeSteps = 20};
    }
}
