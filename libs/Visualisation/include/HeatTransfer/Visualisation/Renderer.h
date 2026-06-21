#pragma once

#include <algorithm>
#include <cmath>
#include <format>
#include <iostream>
#include <memory>
#include <vector>

#include <glad/gl.h> // Must be before glfw include

#include <GLFW/glfw3.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <imgui.h>
#include <implot.h>
#include <implot3d.h>
#include <implot3d_internal.h>

#include "HeatTransfer/SimulationRunner/SimulationState.h"

#include "HeatTransfer/Visualisation/Console.h"

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

        std::shared_ptr<Console> _console;

        void Init();
        void Shutdown();
        void ShowSideBar();
        void ShowPlotsWindow(const SimulationRunner::SimulationState& state);
        void ShowConsole();
    };
}