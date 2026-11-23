#pragma once

#include <string>
#include <vector>

#include "HeatTransfer/Visualisation/ConvergencePlotsState.h"
#include "HeatTransfer/Visualisation/IPanel.h"
#include "HeatTransfer/Visualisation/SliceData.h"
#include "HeatTransfer/Visualisation/SliceState.h"
#include "HeatTransfer/Visualisation/SurfacePlotState.h"
#include "HeatTransfer/Visualisation/VolumeScaleState.h"
#include "HeatTransfer/Visualisation/VolumeViewState.h"

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

        static constexpr const char* AXIS_OPTIONS[3] = {"X (length)", "Y (width)", "Z (height)"};

        static constexpr const char* PLANE_LABELS[3] = {"X slice", "Y slice", "Z slice"};

        static constexpr const char* FACE_LABELS[6] = {
            "##Left", "##Right", "##Top", "##Bottom", "##Front", "##Back"};

        // Each plane of the 3D view, and each slice plot, is downsampled to about this many points
        static constexpr int VOLUME_PLANE_POINTS = 2500;
        static constexpr int SLICE_PLOT_POINTS = 10000;

        std::shared_ptr<RenderFrameData> _shownRenderFrameData;

        std::shared_ptr<VolumeViewState> _temperatureVolumeViewState;
        std::shared_ptr<VolumeViewState> _residualVolumeViewState;
        std::shared_ptr<VolumeScaleState> _volumeScaleState;

        std::shared_ptr<SurfacePlotState> _temperatureSurfacePlotState;
        std::shared_ptr<SurfacePlotState> _residualSurfacePlotState;
        std::shared_ptr<SliceState> _sliceState;

        std::shared_ptr<ConvergencePlotsState> _errorConvergencePlotsState;
        std::shared_ptr<ConvergencePlotsState> _residualConvergencePlotsState;

        void ResetViewsIfNeeded();
        void ShowPlotsWindow();
        void CreateVolumeView(VolumeViewState& state);
        void RebuildVolumeMeshIfNeeded(VolumeViewState& state);
        static void SetupLengthAxisTicks(ImAxis3D axis, double maxPoint, double gridSpacing);
        double GetGridSpacing();
        void CreateSurfacePlot(SurfacePlotState& state);
        SliceData CreateSliceSelector(const Core::Tensor3d& field);
        void CreateConvergencePlots(ConvergencePlotsState& state);
        void CreateConvergencePlot(const ConvergencePlotsState& state,
                                   const std::string& statistic,
                                   const std::string& metric,
                                   const std::vector<double>& values,
                                   double valuesMin,
                                   double valuesMax);
    };
}