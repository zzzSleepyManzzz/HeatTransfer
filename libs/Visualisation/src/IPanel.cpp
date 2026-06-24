#include "HeatTransfer/Visualisation/IPanel.h"

namespace HeatTransfer::Visualisation
{
    IPanel::IPanel(std::shared_ptr<ConsoleLogger> logger) : _logger(logger) {}
}
