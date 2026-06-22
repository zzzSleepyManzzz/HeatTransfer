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
        glfwMaximizeWindow(_window);

        if (!_window)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
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
        _console = std::make_shared<Console>();
    }

    void Renderer::Render(const SimulationRunner::SimulationState& state)
    {
        // Start frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Show windows

        ShowSideBar(state);
        ShowPlotsWindow(state);
        ShowConsole();

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

    void Renderer::ShowSideBar(const SimulationRunner::SimulationState& state)
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize;
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowSize(ImVec2(displaySize.x / 5.0f, displaySize.y));
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::Begin("Side Bar", nullptr, windowFlags);
        {
            AddSettings(state);
            AddStatistics(state);
        }
        ImGui::End();
    }

    void Renderer::AddSettings(const SimulationRunner::SimulationState& state)
    {
        static bool lightModeOn = false;

        if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Text("Turn light mode on");

            ImGui::SameLine();

            if (ImGui::Checkbox("##Turn light mode on", &lightModeOn))
            {
                if (lightModeOn)
                    ImGui::StyleColorsLight();
                else
                    ImGui::StyleColorsDark();
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }
    }

    void Renderer::AddStatistics(const SimulationRunner::SimulationState& state)
    {
        if (ImGui::CollapsingHeader("Statistics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // For now, statistics will use big Temperature matrix, not small field vector

            auto minTemperature = state.TemperatureMatrix.minCoeff();
            auto maxTemperature = state.TemperatureMatrix.maxCoeff();
            auto meanTemperature = state.TemperatureMatrix.mean();

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Indent(10.0f);

            ImGui::Text("FPS: %.1f FPS", ImGui::GetIO().Framerate);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Text("Max Temperature: %.2f °C", maxTemperature);
            ImGui::Text("Min Temperature: %.2f °C", minTemperature);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Text("Average Temperature: %.2f °C", meanTemperature);

            ImGui::Unindent(10.0f);
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }
    }

    void Renderer::ShowPlotsWindow(const SimulationRunner::SimulationState& state)
    {
        const auto& data = state.field;

        int cols = state.cols;
        int rows = state.rows;
        int N = cols * rows;

        std::vector<float> xs(N, 0);
        std::vector<float> ys(N, 0);
        std::vector<float> zs(data.begin(), data.end());

        float x_min = 0;
        float x_max = state.TemperatureMatrix.cols();
        float y_min = 0;
        float y_max = state.TemperatureMatrix.rows();
        float z_min = *std::min_element(zs.begin(), zs.end());
        float z_max = *std::max_element(zs.begin(), zs.end());

        float x_step = (x_max - x_min) / (cols - 1);
        float y_step = (y_max - y_min) / (rows - 1);

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                int idx = i * cols + j;
                xs[idx] = x_min + j * x_step;
                ys[idx] = y_min + i * y_step;
            }
        }

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
                    static bool resetZoom = false;
                    static int selectedColorMap = 6; // Hot by default

                    ImPlot3DSurfaceFlags surfacePlotFlags = ImPlot3DSurfaceFlags_None;
                    static bool hideLinesOn = false;
                    static bool removeFillOn = false;

                    if (ImGui::Button("Reset zoom"))
                    {
                        resetZoom = true;
                    }

                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.25f);
                    const char* colorMapOptions[] = {
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
                    ImGui::Combo("Color map type",
                                 &selectedColorMap,
                                 colorMapOptions,
                                 IM_ARRAYSIZE(colorMapOptions));

                    ImGui::SameLine();

                    if (ImGui::Checkbox("Hide lines", &hideLinesOn))
                    {
                    }
                    if (hideLinesOn)
                    {
                        surfacePlotFlags |= ImPlot3DSurfaceFlags_NoLines;
                    }

                    ImGui::SameLine();

                    if (ImGui::Checkbox("Remove fill", &removeFillOn))
                    {
                    }
                    if (removeFillOn)
                    {
                        surfacePlotFlags |= ImPlot3DSurfaceFlags_NoFill;
                    }

                    ImPlot3D::PushColormap(selectedColorMap);

                    ImPlot3DFlags plot3DFlags = ImPlot3DFlags_NoPan;
                    auto plotWindowWidth = ImGui::GetWindowSize().x;

                    if (ImPlot3D::BeginPlot(
                            "## Temperature", ImVec2(plotWindowWidth * 0.93, -1), plot3DFlags))
                    {
                        ImPlot3D::SetupAxes("Width [pixels]", "Length [pixels]", "Temperature [K]");

                        if (resetZoom)
                        {
                            ImPlot3D::SetupAxesLimits(
                                x_min, x_max, y_min, y_max, z_min, z_max, ImPlot3DCond_Always);
                            resetZoom = false;
                            ImPlot3D::SetupBoxRotation(30, -45, true, ImPlot3DCond_Always);
                        }
                        else
                        {
                            ImPlot3D::SetupAxesLimits(
                                x_min, x_max, y_min, y_max, z_min, z_max, ImPlot3DCond_Once);
                            ImPlot3D::SetupBoxRotation(30, -45, true, ImPlot3DCond_Once);
                        }

                        ImPlot3DSpec spec;
                        spec.FillAlpha = 1.0f;
                        spec.Flags = surfacePlotFlags;
                        spec.LineColor = ImPlot3D::GetColormapColor(1);

                        ImPlot3D::PlotSurface("## Temperature Surface Plot",
                                              xs.data(),
                                              ys.data(),
                                              zs.data(),
                                              cols,
                                              rows,
                                              0.0,
                                              0.0,
                                              spec);

                        ImPlot3D::EndPlot();
                    }

                    ImPlot3D::PopColormap();

                    ImGui::SameLine();
                    ImPlot::PushColormap(selectedColorMap);
                    ImPlot::ColormapScale("Temperature [K]", z_min, z_max, ImVec2(-1, -1));
                    ImPlot::PopColormap();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Heat Map"))
                {
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.25f);
                    static int selectedColorMap = 6; // Hot by default

                    const char* colorMapOptions[] = {
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
                    ImGui::Combo("Color map type",
                                 &selectedColorMap,
                                 colorMapOptions,
                                 IM_ARRAYSIZE(colorMapOptions));

                    ImPlot::PushColormap(selectedColorMap);

                    auto plotWindowWidth = ImGui::GetWindowSize().x;

                    if (ImPlot::BeginPlot("Temperature Heat Map",
                                          ImVec2(plotWindowWidth * 0.93, -1),
                                          ImPlotFlags_NoLegend))
                    {
                        ImPlot::SetupAxes("Length [pixels]",
                                          "Width [pixels]",
                                          ImPlotAxisFlags_None,
                                          ImPlotAxisFlags_None);
                        ImPlot::SetupAxesLimits(0,
                                                state.TemperatureMatrix.cols(),
                                                0,
                                                state.TemperatureMatrix.rows(),
                                                ImPlotCond_Always);

                        ImPlot::PlotHeatmap("Plotted Heat Map",
                                            zs.data(),
                                            rows,
                                            cols,
                                            z_min,
                                            z_max,
                                            nullptr,
                                            ImPlotPoint(0, 0),
                                            ImPlotPoint(state.TemperatureMatrix.cols(),
                                                        state.TemperatureMatrix.rows()));

                        ImPlot::EndPlot();
                    }

                    ImGui::SameLine();
                    ImPlot::ColormapScale("Temperature [K]", z_min, z_max, ImVec2(-1, -1));
                    ImPlot::PopColormap();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Error plot"))
                {
                    const auto& errors = state.errors;

                    std::vector<float> iterations = {};
                    std::vector<float> maxErrors = {};
                    std::vector<float> meanErrors = {};
                    std::vector<float> rmsErrors = {};

                    for (const auto& error : errors)
                    {
                        iterations.push_back(error.iteration);
                        maxErrors.push_back(error.errorMax);
                        meanErrors.push_back(error.errorMean);
                        rmsErrors.push_back(error.errorRMS);
                    }

                    if (ImGui::Button("Print error history"))
                    {
                        _console->AddSpace();
                        _console->Add("================ Errors ================");
                        _console->AddSpace();

                        for (const auto& e : state.errors)
                        {
                            _console->Add(std::format(
                                "Iter {:4}  |  Max {:.4e}  |  Mean {:.4e}  |  RMS {:.4e}",
                                e.iteration,
                                e.errorMax,
                                e.errorMean,
                                e.errorRMS));
                        }

                        _console->AddSpace();
                        _console->Add("========================================");
                        _console->AddSpace();
                    }

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
                        ImPlot::PlotLine("## Max Error",
                                         iterations.data(),
                                         meanErrors.data(),
                                         iterations.size());
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
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    void Renderer::ShowConsole()
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize;
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowSize(ImVec2(displaySize.x * 4.0f / 5.0f, displaySize.y * 1.0f / 4.0f));
        ImGui::SetNextWindowPos(ImVec2(displaySize.x / 5.0f, displaySize.y * 3.0f / 4.0f));

        ImGui::Begin("Console", nullptr, windowFlags);
        {
            if (ImGui::Button("Clear"))
            {
                _console->Clear();
            }

            ImGui::BeginChild(
                "ConsoleScroll", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

            for (const auto& line : _console->GetLines())
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