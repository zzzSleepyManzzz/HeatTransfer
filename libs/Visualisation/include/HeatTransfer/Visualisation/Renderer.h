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
#include "HeatTransfer/Visualisation/HeatMapState.h"
#include "HeatTransfer/Visualisation/RendererModel.h"
#include "HeatTransfer/Visualisation/SettingsState.h"
#include "HeatTransfer/Visualisation/SurfacePlotState.h"

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
        static constexpr const char* COLOR_MAP_OPTIONS[16] = {
            "Deep",
            "Dark",
            "Pastel",
            "Paired",
            "Viridis",
            "Plasma",
            "Hot",
            "Cool",
            "Pink",
            "Jet",
            "Twilight",
            "RdBu",
            "BrBG",
            "PiYG",
            "Spectral",
            "Greys",
        };

        GLFWwindow* _window = nullptr;

        std::shared_ptr<RendererModel> _model;

        std::shared_ptr<ConsoleLogger> _console;

        std::shared_ptr<SettingsState> _settingsState;
        std::shared_ptr<SurfacePlotState> _surfacePlotState;
        std::shared_ptr<HeatMapState> _heatMapState;

        void Init();
        void Shutdown();

        void ShowSideBar();
        void AddSettings();
        void AddStatistics();

        void ShowPlotsWindow();
        void CreateSurfacePlot();
        void CreateHeatMap();
        void CreateErrorPlots();

        void ShowConsole();
    };
}