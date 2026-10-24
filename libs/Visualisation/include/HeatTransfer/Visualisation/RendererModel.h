#pragma once

#include <memory>

#include "HeatTransfer/SimulationRunner/SimulationConfig.h"
#include "HeatTransfer/SimulationRunner/SimulationOutput.h"

#include "HeatTransfer/Visualisation/RenderFrameData.h"
#include "HeatTransfer/Visualisation/StatisticsData.h"

namespace HeatTransfer::Visualisation
{
    class RendererModel
    {
    public:
        RendererModel(std::shared_ptr<HeatTransfer::SimulationRunner::SimulationConfig> config,
                      std::shared_ptr<HeatTransfer::SimulationRunner::SimulationOutput> output,
                      double durationInSeconds);

        std::shared_ptr<HeatTransfer::SimulationRunner::SimulationConfig> GetSimulationConfig();
        std::shared_ptr<RenderFrameData> GetRenderFrameData();
        std::shared_ptr<StatisticsData> GetStatisticsData();

        void UpdateModel();
        bool NeedsSimulationUpdate();

    private:
        std::shared_ptr<HeatTransfer::SimulationRunner::SimulationConfig> _simulationConfig;
        std::shared_ptr<RenderFrameData> _renderFrameData;
        std::shared_ptr<StatisticsData> _statisticsData;

        bool _needsSimulationUpdate;
    };
}