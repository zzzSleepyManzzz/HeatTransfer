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

#include "HeatTransfer/Visualisation/ConsoleLogger.h"
#include "HeatTransfer/Visualisation/ConsolePanel.h"
#include "HeatTransfer/Visualisation/PlotsPanel.h"
#include "HeatTransfer/Visualisation/RendererModel.h"
#include "HeatTransfer/Visualisation/SidePanel.h"

namespace HeatTransfer::Visualisation
{
    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        void Render(std::shared_ptr<RendererModel> model);

        bool ShouldClose() const;
        void PollEvents();

    private:
        GLFWwindow* _window = nullptr;

        std::shared_ptr<SidePanel> _sidePanel;
        std::shared_ptr<PlotsPanel> _plotsPanel;
        std::shared_ptr<ConsolePanel> _consolePanel;

        void Init();
        void Shutdown();
    };
}