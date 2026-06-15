#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include <glad/gl.h> // Must be before glfw include

#include <GLFW/glfw3.h>

#include "HeatTransfer/SimulationRunner/SimulationState.h"

namespace HeatTransfer::Visualisation
{
    class Renderer
    {
    public:
        Renderer(int width = 1200, int height = 800);
        ~Renderer() = default;

        void Init();
        void Render(const SimulationRunner::SimulationState& state);
        void Shutdown();

        bool ShouldClose() const;
        void PollEvents();

    private:
        GLFWwindow* _window = nullptr;

        unsigned int _texture = 0;
        unsigned int _vao = 0;
        unsigned int _vbo = 0;
        unsigned int _shader = 0;

        int _width;
        int _height;

        void InitGLFW();
        void InitGLAD();
        void InitTexture();
        void InitQuad();
        void InitShader();

        std::vector<float> Normalize(const SimulationRunner::SimulationState& state);
    };
}