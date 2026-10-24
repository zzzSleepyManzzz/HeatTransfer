#pragma once

#include <memory>

#include <glad/gl.h> // Must be before glfw include

#include <GLFW/glfw3.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <imgui.h>
#include <implot.h>
#include <implot3d.h>
#include <implot3d_internal.h>

#include "HeatTransfer/Visualisation/ConsoleLogger.h"
#include "HeatTransfer/Visualisation/RendererModel.h"

namespace HeatTransfer::Visualisation
{
    class IPanel
    {
    public:
        IPanel() = delete;
        ~IPanel() = default;

        virtual void Render(std::shared_ptr<RendererModel> model) = 0;

    protected:
        std::shared_ptr<ConsoleLogger> _logger;

        std::shared_ptr<RendererModel> _model; // Attached during Render

        IPanel(std::shared_ptr<ConsoleLogger> logger);
    };
}