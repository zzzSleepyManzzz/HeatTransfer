#include "HeatTransfer/Visualisation/Renderer.h"

namespace HeatTransfer::Visualisation
{
    Renderer::Renderer()
    {
        Init();
    }

    Renderer::~Renderer()
    {
        Shutdown();
    }

    void Renderer::Init()
    {
        // Initialize GLFW
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return;
        }

        // Setup OpenGL version
#if defined(__APPLE__)
        // GL 3.2 + GLSL 150 (MacOS)
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on MacOS
#else
        // GL 3.0 + GLSL 130 (Windows and Linux)
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

        // Create window
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

        int screenWidth = mode->width;
        int screenHeight = mode->height;

        _window = glfwCreateWindow(screenWidth, screenHeight, "Heat Transfer", nullptr, nullptr);

        if (!_window)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        glfwMaximizeWindow(_window);
        glfwMakeContextCurrent(_window);

        if (!gladLoadGL(glfwGetProcAddress))
        {
            std::cerr << "Failed to init GLAD" << std::endl;
            return;
        }
        glfwSwapInterval(1); // Enable vsync - easier CPU load

        // Setup context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImPlot3D::CreateContext();

        // Setup style
        ImGui::StyleColorsDark();

        // Enlarge font
        ImGuiIO& io = ImGui::GetIO();
        ImFontConfig config;

        config.SizePixels = 16.0f; // 13 (default) -> 16 pixels
        io.Fonts->AddFontDefault(&config);

        // Setup backend
        ImGui_ImplGlfw_InitForOpenGL(_window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        // Initialise renderer members
        _consoleLogger = std::make_shared<ConsoleLogger>();
        _settingsState = std::make_shared<SettingsState>();
        _surfacePlotState = std::make_shared<SurfacePlotState>();
        _heatMapState = std::make_shared<HeatMapState>();
    }

    void Renderer::Render(std::shared_ptr<RendererModel> model)
    {
        // Start frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Show windows

        _model = model;

        ShowSideBar();
        ShowPlotsWindow();
        ShowConsoleLogger();

        // Render
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(_window);
    }

    void Renderer::ShowSideBar()
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize;
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowSize(ImVec2(displaySize.x / 5.0f, displaySize.y));
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::Begin("Side Bar", nullptr, windowFlags);
        {
            AddSettings();
            AddStatistics();
        }
        ImGui::End();
    }

    void Renderer::AddSettings()
    {
        if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Text("Turn light mode on");

            ImGui::SameLine();

            if (ImGui::Checkbox("##Turn light mode on", &_settingsState->lightModeOn))
            {
                if (_settingsState->lightModeOn)
                    ImGui::StyleColorsLight();
                else
                    ImGui::StyleColorsDark();
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }
    }

    void Renderer::AddStatistics()
    {
        if (ImGui::CollapsingHeader("Statistics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // For now, statistics will use big Temperature matrix, not small field vector

            auto statisticsData = _model->GetStatisticsData();

            auto minTemperature = statisticsData->GetMinTemperature();
            auto maxTemperature = statisticsData->GetMaxTemperature();
            auto meanTemperature = statisticsData->GetMeanTemperature();

            auto numIterations = statisticsData->GetNumIterations();
            auto finalMaxError = statisticsData->GetFinalMaxError();
            auto finalMeanError = statisticsData->GetFinalMeanError();
            auto finalRMSError = statisticsData->GetFinal_RMS_Error();

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Indent(10.0f);

            ImGui::Text("FPS: %.1f FPS", ImGui::GetIO().Framerate);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Text("Max Temperature: %.2f °C", maxTemperature);
            ImGui::Text("Min Temperature: %.2f °C", minTemperature);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Text("Average Temperature: %.2f °C", meanTemperature);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Text("Number of iterations: %zu", numIterations);
            ImGui::Text("Final max  error:     %.4f", finalMaxError);
            ImGui::Text("Final mean error:     %.4f", finalMeanError);
            ImGui::Text("Final RMS  error:     %.4f", finalRMSError);

            ImGui::Unindent(10.0f);
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }
    }

    void Renderer::ShowPlotsWindow()
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize;
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowSize(ImVec2(displaySize.x * 4.0f / 5.0f, displaySize.y * 3.0f / 4.0f));
        ImGui::SetNextWindowPos(ImVec2(displaySize.x / 5.0f, 0));

        ImGui::Begin("Plots", nullptr, windowFlags);
        {
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            if (ImGui::BeginTabBar("Plot Tabs"))
            {
                if (ImGui::BeginTabItem("Surface Plot"))
                {
                    CreateSurfacePlot();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Heat Map"))
                {
                    CreateHeatMap();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Error plot"))
                {
                    CreateErrorPlots();

                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    void Renderer::CreateSurfacePlot()
    {
        auto plotData = _model->GetRenderFrameData();

        ImPlot3DSurfaceFlags surfacePlotFlags = ImPlot3DSurfaceFlags_None;

        // Reset zoom button

        if (ImGui::Button("Reset zoom"))
        {
            _surfacePlotState->resetZoom = true;
        }

        // ColorMap combo box, hide lines and remove lines checkboxes

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.25f);

        ImGui::Combo("Color map type",
                     &_surfacePlotState->selectedColorMap,
                     COLOR_MAP_OPTIONS,
                     IM_ARRAYSIZE(COLOR_MAP_OPTIONS));

        ImGui::SameLine();

        if (ImGui::Checkbox("Hide lines", &_surfacePlotState->hideLinesOn))
        {
        }
        if (_surfacePlotState->hideLinesOn)
        {
            surfacePlotFlags |= ImPlot3DSurfaceFlags_NoLines;
        }

        ImGui::SameLine();

        if (ImGui::Checkbox("Remove fill", &_surfacePlotState->removeFillOn))
        {
        }
        if (_surfacePlotState->removeFillOn)
        {
            surfacePlotFlags |= ImPlot3DSurfaceFlags_NoFill;
        }

        // Create surface plot

        ImPlot3D::PushColormap(_surfacePlotState->selectedColorMap);

        ImPlot3DFlags plot3DFlags = ImPlot3DFlags_NoPan;
        auto plotWindowWidth = ImGui::GetWindowSize().x;

        if (ImPlot3D::BeginPlot("## Temperature", ImVec2(plotWindowWidth * 0.93, -1), plot3DFlags))
        {
            ImPlot3D::SetupAxes("Width [pixels]", "Length [pixels]", "Temperature [°C]");

            if (_surfacePlotState->resetZoom)
            {
                ImPlot3D::SetupAxesLimits(plotData->Get_X_Min(),
                                          plotData->Get_X_Max(),
                                          plotData->Get_Y_Min(),
                                          plotData->Get_Y_Max(),
                                          plotData->Get_Z_Min(),
                                          plotData->Get_Z_Max(),
                                          ImPlot3DCond_Always);
                _surfacePlotState->resetZoom = false;
                ImPlot3D::SetupBoxRotation(30, -45, true, ImPlot3DCond_Always);
            }
            else
            {
                ImPlot3D::SetupAxesLimits(plotData->Get_X_Min(),
                                          plotData->Get_X_Max(),
                                          plotData->Get_Y_Min(),
                                          plotData->Get_Y_Max(),
                                          plotData->Get_Z_Min(),
                                          plotData->Get_Z_Max(),
                                          ImPlot3DCond_Once);
                ImPlot3D::SetupBoxRotation(30, -45, true, ImPlot3DCond_Once);
            }

            ImPlot3DSpec spec;
            spec.FillAlpha = 1.0f;
            spec.Flags = surfacePlotFlags;
            spec.LineColor = ImPlot3D::GetColormapColor(1);

            ImPlot3D::PlotSurface("## Temperature Surface Plot",
                                  plotData->Get_X_Values().data(),
                                  plotData->Get_Y_Values().data(),
                                  plotData->Get_Z_Values().data(),
                                  plotData->GetCols(),
                                  plotData->GetRows(),
                                  0.0,
                                  0.0,
                                  spec);

            ImPlot3D::EndPlot();
        }

        ImPlot3D::PopColormap();

        // Create color bar

        ImGui::SameLine();
        ImPlot::PushColormap(_surfacePlotState->selectedColorMap);
        ImPlot::ColormapScale(
            "Temperature [°C]", plotData->Get_Z_Min(), plotData->Get_Z_Max(), ImVec2(-1, -1));
        ImPlot::PopColormap();
    }

    void Renderer::CreateHeatMap()
    {
        auto data = _model->GetRenderFrameData();

        // Color map combo box

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.25f);

        ImGui::Combo("Color map type",
                     &_heatMapState->selectedColorMap,
                     COLOR_MAP_OPTIONS,
                     IM_ARRAYSIZE(COLOR_MAP_OPTIONS));

        // Create heat map

        ImPlot::PushColormap(_heatMapState->selectedColorMap);

        auto plotWindowWidth = ImGui::GetWindowSize().x;

        if (ImPlot::BeginPlot(
                "Temperature Heat Map", ImVec2(plotWindowWidth * 0.93, -1), ImPlotFlags_NoLegend))
        {
            ImPlot::SetupAxes(
                "Length [pixels]", "Width [pixels]", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
            ImPlot::SetupAxesLimits(data->Get_X_Min(),
                                    data->Get_X_Max(),
                                    data->Get_Y_Min(),
                                    data->Get_Y_Max(),
                                    ImPlotCond_Always);

            ImPlot::PlotHeatmap("Plotted Heat Map",
                                data->Get_Z_Values().data(),
                                data->GetRows(),
                                data->GetCols(),
                                data->Get_Z_Min(),
                                data->Get_Z_Max(),
                                nullptr,
                                ImPlotPoint(data->Get_X_Min(), data->Get_Y_Min()),
                                ImPlotPoint(data->Get_X_Max(), data->Get_Y_Max()));

            ImPlot::EndPlot();
        }

        // Create color bar

        ImGui::SameLine();
        ImPlot::ColormapScale(
            "Temperature [°C]", data->Get_Z_Min(), data->Get_Z_Max(), ImVec2(-1, -1));
        ImPlot::PopColormap();
    }

    void Renderer::CreateErrorPlots()
    {
        auto plotData = _model->GetRenderFrameData();

        const auto& iterations = plotData->Get_Iterations();
        const auto& maxErrors = plotData->Get_MaxErrors();
        const auto& meanErrors = plotData->Get_MeanErrors();
        const auto& rmsErrors = plotData->Get_RMS_Errors();

        // Print error history button

        if (ImGui::Button("Print error history"))
        {
            _consoleLogger->AddSpace();
            _consoleLogger->Add("================ Errors ================");
            _consoleLogger->AddSpace();

            for (auto i = 0u; i < iterations.size(); i++)
            {
                _consoleLogger->Add(
                    std::format("Iter {:4}  |  Max {:.4e}  |  Mean {:.4e}  |  RMS {:.4e}",
                                (int)iterations[i],
                                maxErrors[i],
                                meanErrors[i],
                                rmsErrors[i]));
            }

            _consoleLogger->AddSpace();
            _consoleLogger->Add("========================================");
            _consoleLogger->AddSpace();
        }

        // Create line plots

        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 3.0f);

        if (ImPlot::BeginPlot("Max errors against iterations"))
        {
            ImPlot::SetupAxes("Iterations", "Max error");
            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            ImPlot::PlotLine(
                "## Max Error", iterations.data(), maxErrors.data(), iterations.size());
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("Mean errors against iteration"))
        {
            ImPlot::SetupAxes("Iterations", "Mean error");
            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            ImPlot::PlotLine(
                "## Max Error", iterations.data(), meanErrors.data(), iterations.size());
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("RMS errors against iteration"))
        {
            ImPlot::SetupAxes("Iterations", "RMS error");
            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            ImPlot::PlotLine(
                "## Max Error", iterations.data(), rmsErrors.data(), iterations.size());
            ImPlot::EndPlot();
        }

        ImPlot::PopStyleVar();
    }

    void Renderer::ShowConsoleLogger()
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize;
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowSize(ImVec2(displaySize.x * 4.0f / 5.0f, displaySize.y * 1.0f / 4.0f));
        ImGui::SetNextWindowPos(ImVec2(displaySize.x / 5.0f, displaySize.y * 3.0f / 4.0f));

        ImGui::Begin("Console", nullptr, windowFlags);
        {
            if (ImGui::Button("Clear"))
            {
                _consoleLogger->Clear();
            }

            ImGui::BeginChild(
                "ConsoleScroll", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

            for (const auto& line : _consoleLogger->GetLines())
            {
                ImGui::TextUnformatted(line.c_str());
            }

            ImGui::EndChild();
        }
        ImGui::End();
    }

    bool Renderer::ShouldClose() const
    {
        return glfwWindowShouldClose(_window);
    }

    void Renderer::PollEvents()
    {
        glfwPollEvents();
    }

    void Renderer::Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImPlot3D::DestroyContext();
        ImPlot::DestroyContext();
        ImGui::DestroyContext();

        glfwDestroyWindow(_window);
        glfwTerminate();
    }
}