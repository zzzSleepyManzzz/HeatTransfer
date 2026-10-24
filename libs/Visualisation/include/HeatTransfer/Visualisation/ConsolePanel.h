#pragma once

#include "HeatTransfer/Visualisation/IPanel.h"

namespace HeatTransfer::Visualisation
{
    class ConsolePanel : public IPanel
    {
    public:
        ConsolePanel(std::shared_ptr<ConsoleLogger> logger);
        void Render(std::shared_ptr<RendererModel> model) override;

    private:
        void ShowConsole();
    };
}