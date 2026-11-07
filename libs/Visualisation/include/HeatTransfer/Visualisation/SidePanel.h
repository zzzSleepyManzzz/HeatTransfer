#pragma once

#include <algorithm>

#include "HeatTransfer/Core/Smoother.h"
#include "HeatTransfer/Core/TrackingMetric.h"

#include "HeatTransfer/Visualisation/IPanel.h"
#include "HeatTransfer/Visualisation/SettingsState.h"

namespace HeatTransfer::Visualisation
{
    class SidePanel : public IPanel
    {
    public:
        SidePanel(std::shared_ptr<ConsoleLogger> logger);
        void Render(std::shared_ptr<RendererModel> model) override;

    private:
        static constexpr const char* SMOOTHER_OPTIONS[4] = {
            "Jacobi", "Gauss-Seidel", "Successive over relaxation", "Red-Black SOR"};

        static constexpr const HeatTransfer::Core::Smoother SMOOTHER_ARRAY[4] = {
            HeatTransfer::Core::Smoother::JACOBI,
            HeatTransfer::Core::Smoother::GAUSS_SEIDEL,
            HeatTransfer::Core::Smoother::SUCCESSIVE_OVER_RELAXATION,
            HeatTransfer::Core::Smoother::RED_BLACK_SOR};

        static constexpr const char* TRACKER_OPTIONS[3] = {"Max", "Mean", "RMS"};

        static constexpr const HeatTransfer::Core::TrackingMetric TRACKER_ARRAY[3] = {
            HeatTransfer::Core::TrackingMetric::MAX,
            HeatTransfer::Core::TrackingMetric::MEAN,
            HeatTransfer::Core::TrackingMetric::RMS};

        std::shared_ptr<SettingsState> _settingsState;

        void ShowSideBar();

        void AddProgress();
        void AddSettings();
        void AddParametersButton();
        void AddMaterialPropertiesButton();
        void AddBoundaryConditionsButton();

        void AddStatistics();
    };
}