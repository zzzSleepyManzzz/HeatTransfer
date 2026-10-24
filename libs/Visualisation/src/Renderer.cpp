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
        auto consoleLogger = std::make_shared<ConsoleLogger>();

        _sidePanel = std::make_shared<SidePanel>(consoleLogger);
        _plotsPanel = std::make_shared<PlotsPanel>(consoleLogger);
        _consolePanel = std::make_shared<ConsolePanel>(consoleLogger);
    }

    void Renderer::Render(std::shared_ptr<RendererModel> model)
    {
        // Start frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Show panels

        _sidePanel->Render(model);
        _plotsPanel->Render(model);
        _consolePanel->Render(model);

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