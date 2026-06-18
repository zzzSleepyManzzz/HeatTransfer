#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace HeatTransfer::Visualisation
{
    class ShaderFileSource
    {
    public:
        ShaderFileSource();
        static std::string LoadFragmentShaderSource();
        static std::string LoadVertexShaderSource();

    private:
        static std::filesystem::path ModulePath();
        static std::string LoadShaderFileSource(std::filesystem::path filePath);
    };
}