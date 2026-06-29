#pragma once

#include "HeatTransfer/Visualisation/ErrorPlotsState.h"
#include "HeatTransfer/Visualisation/HeatMapState.h"
#include "HeatTransfer/Visualisation/IPanel.h"
#include "HeatTransfer/Visualisation/ResidualConvergencePlotsState.h"
#include "HeatTransfer/Visualisation/ResidualSurfacePlotState.h"
#include "HeatTransfer/Visualisation/SurfacePlotState.h"

namespace HeatTransfer::Visualisation
{
    class PlotsPanel : public IPanel
    {
    public:
        PlotsPanel(std::shared_ptr<ConsoleLogger> logger);
        void Render(std::shared_ptr<RendererModel> model) override;

    private:
        static constexpr const char* COLOR_MAP_OPTIONS[16] = {
            "Deep",
            "Dark",
            "Pastel",
            "Paired",
            "Viridis",
            "Plasma",
            "Hot",
            "Cool",
            "Pink",
            "Jet",
            "Twilight",
            "RdBu",
            "BrBG",
            "PiYG",
            "Spectral",
            "Greys",
        };

        std::shared_ptr<SurfacePlotState> _surfacePlotState;
        std::shared_ptr<ResidualSurfacePlotState> _residualSurfacePlotState;
        std::shared_ptr<HeatMapState> _heatMapState;
        std::shared_ptr<ErrorPlotsState> _errorPlotsState;
        std::shared_ptr<ResidualConvergencePlotsState> _residualConvergencePlotsState;

        void ShowPlotsWindow();
        void CreateSurfacePlot();
        void CreateResidualSurfacePlot();
        void CreateHeatMap();
        void CreateErrorPlots();
        void CreateResidualConvergencePlots();
    };
}