#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include <glad/gl.h> // Must be before glfw include

#include <GLFW/glfw3.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <imgui.h>
#include <implot.h>
#include <implot3d.h>

#include "HeatTransfer/SimulationRunner/SimulationState.h"

namespace HeatTransfer::Visualisation
{
    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        void Render(const SimulationRunner::SimulationState& state);

        bool ShouldClose() const;
        void PollEvents();

    private:
        GLFWwindow* _window = nullptr;

        void Init();
        void Shutdown();
        void ShowSideBar();
        void ShowPlotsWindow(const SimulationRunner::SimulationState& state);
        void ShowConsole();
    };
}