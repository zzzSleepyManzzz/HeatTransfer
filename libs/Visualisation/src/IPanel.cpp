#include "HeatTransfer/Visualisation/IPanel.h"

namespace HeatTransfer::Visualisation
{
    IPanel::IPanel(GLFWwindow* window, std::shared_ptr<ConsoleLogger> logger)
        : _window(window), _logger(logger)
    {
    }
}
