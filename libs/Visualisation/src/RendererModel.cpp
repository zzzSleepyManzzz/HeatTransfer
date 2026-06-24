#include "HeatTransfer/Visualisation/RendererModel.h"

namespace HeatTransfer::Visualisation
{
    RendererModel::RendererModel(
        std::shared_ptr<HeatTransfer::SimulationRunner::SimulationConfig> config,
        std::shared_ptr<HeatTransfer::SimulationRunner::SimulationOutput> output,
        double durationInSeconds)
        : _simulationConfig(config)
        , _renderFrameData(std::make_shared<RenderFrameData>(output))
        , _statisticsData(std::make_shared<StatisticsData>(output, durationInSeconds))
        , _needsSimulationUpdate(false)
    {
    }

    std::shared_ptr<HeatTransfer::SimulationRunner::SimulationConfig>
    RendererModel::GetSimulationConfig()
    {
        return _simulationConfig;
    }

    std::shared_ptr<RenderFrameData> RendererModel::GetRenderFrameData()
    {
        return _renderFrameData;
    }

    std::shared_ptr<StatisticsData> RendererModel::GetStatisticsData()
    {
        return _statisticsData;
    }

    void RendererModel::UpdateModel()
    {
        _needsSimulationUpdate = true;
    }

    bool RendererModel::NeedsSimulationUpdate()
    {
        return _needsSimulationUpdate;
    }
}