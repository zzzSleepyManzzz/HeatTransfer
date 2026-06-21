#include "HeatTransfer/Visualisation/Console.h"

namespace HeatTransfer::Visualisation
{
    Console::Console() {}

    void Console::Add(const std::string& message)
    {
        _lines.push_back(message);
    }

    void Console::AddSpace()
    {
        _lines.push_back("");
    }

    void Console::Clear()
    {
        _lines.clear();
    }

    const std::vector<std::string>& Console::GetLines()
    {
        return _lines;
    }
}