#pragma once

#include <format>
#include <string>
#include <vector>

namespace HeatTransfer::Visualisation
{
    class ConsoleLogger
    {
    public:
        ConsoleLogger();
        void Add(const std::string& message);
        void AddSpace();
        void Clear();

        const std::vector<std::string>& GetLines();

    private:
        std::vector<std::string> _lines = {};
    };
}