#include "HeatTransfer/Visualisation/RendererModel.h"

namespace HeatTransfer::Visualisation
{
    RendererModel::RendererModel(
        std::shared_ptr<HeatTransfer::SimulationRunner::SimulationOutput> output)
        : _renderFrameData(std::make_shared<RenderFrameData>(output))
        , _statisticsData(std::make_shared<StatisticsData>(output))
    {
    }

    std::shared_ptr<RenderFrameData> RendererModel::GetRenderFrameData()
    {
        return _renderFrameData;
    }

    std::shared_ptr<StatisticsData> RendererModel::GetStatisticsData()
    {
        return _statisticsData;
    }
}