#pragma once

#include "HeatTransfer/Core/Method.h"
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
        static constexpr const char* METHOD_OPTIONS[3] = {
            "Jacobi", "Gauss-Seidel", "Successive over relaxation"};

        static constexpr const HeatTransfer::Core::Method METHOD_ARRAY[3] = {
            HeatTransfer::Core::Method::JACOBI,
            HeatTransfer::Core::Method::GAUSS_SEIDEL,
            HeatTransfer::Core::Method::SUCCESSIVE_OVER_RELAXATION};

        static constexpr const char* TRACKER_OPTIONS[3] = {"Max", "Mean", "RMS"};

        static constexpr const HeatTransfer::Core::TrackingMetric TRACKER_ARRAY[3] = {
            HeatTransfer::Core::TrackingMetric::MAX,
            HeatTransfer::Core::TrackingMetric::MEAN,
            HeatTransfer::Core::TrackingMetric::RMS};

        std::shared_ptr<SettingsState> _settingsState;

        void ShowSideBar();

        void AddSettings();
        void AddParametersButton();
        void AddBoundaryConditionsButton();

        void AddStatistics();
    };
}