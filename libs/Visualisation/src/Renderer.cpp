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

        config.SizePixels = 13.0f * 1.2f; // ~15.6 pixels crisp rasterization
        io.Fonts->AddFontDefault(&config);

        // Setup backend
        ImGui_ImplGlfw_InitForOpenGL(_window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    void Renderer::Render(const SimulationRunner::SimulationState& state)
    {
        // Start frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Show windows
        // ImGui::ShowDemoWindow();
        // ImPlot::ShowDemoWindow();
        // ImPlot3D::ShowDemoWindow();

        ShowSideBar();
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

    void Renderer::ShowSideBar()
    {
        static bool lightModeOn = false;

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize;
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowSize(ImVec2(displaySize.x / 5.0f, displaySize.y));
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::Begin("Side Bar", nullptr, windowFlags);
        {
            // SECTION 1: Settings
            if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Dummy(ImVec2(0.0f, 5.0f));
                if (ImGui::Checkbox("Turn light mode on", &lightModeOn))
                {
                    if (lightModeOn)
                        ImGui::StyleColorsLight();
                    else
                        ImGui::StyleColorsDark();
                }
                // Add surface plot checkboxes here later
                ImGui::Dummy(ImVec2(0.0f, 5.0f));
            }

            // SECTION 2: Statistics
            if (ImGui::CollapsingHeader("Statistics", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Dummy(ImVec2(0.0f, 5.0f));
                ImGui::Indent(10.0f);
                ImGui::Text("FPS: %.1f FPS", ImGui::GetIO().Framerate);
                ImGui::Dummy(ImVec2(0.0f, 5.0f));
                ImGui::Text("Max Temperature: %.2f °C", 100.0f);
                ImGui::Text("Min Temperature: %.2f °C", 0.0f);
                ImGui::Unindent(10.0f);
                ImGui::Dummy(ImVec2(0.0f, 5.0f));
            }
        }
        ImGui::End();
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

                    if (ImGui::Button("Reset zoom"))
                    {
                        resetZoom = true;
                    }

                    ImPlot3D::PushColormap(ImPlot3DColormap_Viridis);
                    ImPlot3DFlags plot3DFlags = ImPlot3DFlags_NoPan;

                    if (ImPlot3D::BeginPlot("## Temperature", ImVec2(-1, -1), plot3DFlags))
                    {
                        ImPlot3D::SetupAxes("Width [pixels]", "Depth [pixels]", "Temperature [K]");

                        if (resetZoom)
                        {
                            ImPlot3D::SetupAxesLimits(
                                x_min, x_max, y_min, y_max, z_min, z_max, ImPlot3DCond_Always);
                            resetZoom = false;
                        }
                        else
                        {
                            ImPlot3D::SetupAxesLimits(
                                x_min, x_max, y_min, y_max, z_min, z_max, ImPlot3DCond_Once);
                        }

                        ImPlot3DSpec spec;
                        spec.FillAlpha = 1.0f;
                        spec.Flags = ImPlot3DSurfaceFlags_NoMarkers;
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
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    void Renderer::ShowConsole()
    {
        static bool checkbox1 = false;

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize;
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowSize(ImVec2(displaySize.x * 4.0f / 5.0f, displaySize.y * 1.0f / 4.0f));
        ImGui::SetNextWindowPos(ImVec2(displaySize.x / 5.0f, displaySize.y * 3.0f / 4.0f));

        ImGui::Begin("Console", nullptr, windowFlags);
        {
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            if (ImGui::Checkbox("Checkbox 1", &checkbox1))
            {
            }
            // Add surface plot checkboxes here later
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
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