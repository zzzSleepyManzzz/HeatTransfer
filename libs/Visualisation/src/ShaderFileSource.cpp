#include "HeatTransfer/Visualisation/ShaderFileSource.h"

namespace HeatTransfer::Visualisation
{
    ShaderFileSource::ShaderFileSource() {}

    std::string ShaderFileSource::LoadVertexShaderSource()
    {
        std::filesystem::path path = ModulePath() / "assets" / "shaders" / "vert.glsl";
        return LoadShaderFileSource(path);
    }

    std::string ShaderFileSource::LoadFragmentShaderSource()
    {
        std::filesystem::path path = ModulePath() / "assets" / "shaders" / "frag.glsl";
        return LoadShaderFileSource(path);
    }

    std::filesystem::path ShaderFileSource::ModulePath()
    {
        return std::filesystem::path(VISUALISATION_ROOT_DIR);
    }

    std::string ShaderFileSource::LoadShaderFileSource(std::filesystem::path filePath)
    {
        std::ifstream shaderFile(filePath);
        if (!shaderFile.is_open())
        {
            throw std::runtime_error("Failed to open shader file: " + filePath.string());
        }

        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();

        return shaderStream.str();
    }
}
