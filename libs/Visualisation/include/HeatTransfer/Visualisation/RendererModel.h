#pragma once

#include <memory>

#include "HeatTransfer/SimulationRunner/SimulationOutput.h"

#include "HeatTransfer/Visualisation/RenderFrameData.h"
#include "HeatTransfer/Visualisation/StatisticsData.h"

namespace HeatTransfer::Visualisation
{
    class RendererModel
    {
    public:
        RendererModel(std::shared_ptr<HeatTransfer::SimulationRunner::SimulationOutput> output);

        std::shared_ptr<RenderFrameData> GetRenderFrameData();
        std::shared_ptr<StatisticsData> GetStatisticsData();

    private:
        std::shared_ptr<RenderFrameData> _renderFrameData;
        std::shared_ptr<StatisticsData> _statisticsData;
    };
}