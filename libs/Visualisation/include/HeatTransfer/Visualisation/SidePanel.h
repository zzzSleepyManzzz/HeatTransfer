#pragma once

#include "HeatTransfer/Visualisation/IPanel.h"
#include "HeatTransfer/Visualisation/SettingsState.h"

namespace HeatTransfer::Visualisation
{
    class SidePanel : public IPanel
    {
    public:
        SidePanel(GLFWwindow* window, std::shared_ptr<ConsoleLogger> logger);
        void Render(std::shared_ptr<RendererModel> model) override;

    private:
        std::shared_ptr<SettingsState> _settingsState;

        void ShowSideBar();
        void AddSettings();
        void AddStatistics();
    };
}