#pragma once

#include <string>
#include <vector>

namespace HeatTransfer::Visualisation
{
    class Console
    {
    public:
        Console();
        void Add(const std::string& message);
        void AddSpace();
        void Clear();

        const std::vector<std::string>& GetLines();

    private:
        std::vector<std::string> _lines = {};
    };
}