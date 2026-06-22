#include "HeatTransfer/Visualisation/ConsoleLogger.h"

namespace HeatTransfer::Visualisation
{
    ConsoleLogger::ConsoleLogger() {}

    void ConsoleLogger::Add(const std::string& message)
    {
        _lines.push_back(message);
    }

    void ConsoleLogger::AddSpace()
    {
        _lines.push_back("");
    }

    void ConsoleLogger::Clear()
    {
        _lines.clear();
    }

    const std::vector<std::string>& ConsoleLogger::GetLines()
    {
        return _lines;
    }
}