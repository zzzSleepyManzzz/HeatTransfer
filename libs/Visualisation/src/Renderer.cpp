#include "HeatTransfer/Visualisation/Renderer.h"

namespace HeatTransfer::Visualisation
{
    Renderer::Renderer(int width, int height) : _width(width), _height(height) {}

    void Renderer::Init()
    {
        InitGLFW();
        InitGLAD();

        InitTexture();
        InitQuad();
        InitShader();

        // 1. Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        // 2. Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(_window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    // -------------------------
    // GLFW + window
    // -------------------------
    void Renderer::InitGLFW()
    {
        // BOILERPLATE CODE

        // 1. Initialise GLFW
        // 2. Provide GLFW hints, otherwise will fallback to default config
        //      - Need to use CORE_PROFILE, so deprecated methods are disabled
        // 3. Create pointer to GLFW window
        //      - GLFWwindow* glfwCreateWindow(int width, int height, const char* title,
        //                                     GLFWmonitor* monitor, GLFWwindow* share)
        //      - Width, height and title are self-explainatory
        //      - Monitor: indicates whether we are windowed or full-screen, we want windowed
        //      - Share: Can pass another window pointer, so they share context
        // 4. Terminate if we fail to create window
        // 5. Make the window the current context, i.e. the current focus

        if (!glfwInit())
        {
            throw std::runtime_error("Failed to init GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        _window =
            glfwCreateWindow(_width, _height, "Heat Transfer Visualization", nullptr, nullptr);

        if (!_window)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create window");
        }

        glfwMakeContextCurrent(_window);
    }

    // -------------------------
    // GLAD
    // -------------------------
    void Renderer::InitGLAD()
    {
        // ALMOST-BOILERPLATE

        // 1. Initialise GLAD loader
        // 2. Create viewport i.e. rectangular area of scene - [x, y, width, height]

        if (!gladLoaderLoadGL())
        {
            throw std::runtime_error("Failed to initialize GLAD");
        }

        glViewport(0, 0, _width, _height);
    }

    // -------------------------
    // Texture (heatmap storage)
    // -------------------------
    void Renderer::InitTexture()
    {
        glGenTextures(1, &_texture);
        glBindTexture(GL_TEXTURE_2D, _texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    // -------------------------
    // Fullscreen quad
    // -------------------------
    void Renderer::InitQuad()
    {
        float vertices[] = {// pos      // uv
                            -1.f, -1.f, 0.f, 0.f, 1.f, -1.f, 1.f, 0.f, 1.f,  1.f, 1.f, 1.f,

                            -1.f, -1.f, 0.f, 0.f, 1.f, 1.f,  1.f, 1.f, -1.f, 1.f, 0.f, 1.f};

        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);

        glBindVertexArray(_vao);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    // -------------------------
    // Shader (placeholder)
    // -------------------------

    void Renderer::InitShader()
    {
        const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";
        ;

        const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoord;
    uniform sampler2D heatmap;

    void main() {
        float t = texture(heatmap, TexCoord).r;
        
        // Classic "Jet" or "Thermal" color ramp
        // Low (0.0) = Blue, Mid (0.5) = Green, High (1.0) = Red
        vec3 color;
        color.r = clamp(4.0 * t - 1.5, 0.0, 1.0);
        color.g = clamp(4.0 * t - 0.5, 0.0, 1.0) - clamp(4.0 * t - 3.5, 0.0, 1.0);
        color.b = clamp(1.5 - 4.0 * t, 0.0, 1.0);

        FragColor = vec4(color, 1.0);
    }
)";

        // Helper to compile shaders
        auto compileShader = [](GLenum type, const char* source)
        {
            GLuint s = glCreateShader(type);
            glShaderSource(s, 1, &source, NULL);
            glCompileShader(s);
            return s;
        };

        GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

        _shader = glCreateProgram();
        glAttachShader(_shader, vs);
        glAttachShader(_shader, fs);
        glLinkProgram(_shader);

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    // -------------------------
    // Normalize simulation data
    // -------------------------
    std::vector<float> Renderer::Normalize(const SimulationRunner::SimulationState& state)
    {
        const auto& f = state.field;

        auto [minIt, maxIt] = std::minmax_element(f.begin(), f.end());
        float minVal = *minIt;
        float maxVal = *maxIt;

        float range = (maxVal - minVal);
        if (range == 0.0f)
            range = 1.0f;

        std::vector<float> out;
        out.resize(f.size());

        for (size_t i = 0; i < f.size(); i++)
            out[i] = (f[i] - minVal) / range;

        return out;
    }

    // -------------------------
    // Render heatmap
    // -------------------------
    void Renderer::Render(const SimulationRunner::SimulationState& state)
    {
        auto texData = Normalize(state);

        glBindTexture(GL_TEXTURE_2D, _texture);

        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_R32F, state.cols, state.rows, 0, GL_RED, GL_FLOAT, texData.data());

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(_shader);
        glBindVertexArray(_vao);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 1. Start ImGui Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 2. Define the Overlay
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::Begin("Simulation Controls");
        ImGui::Text("Grid: %d x %d", state.cols, state.rows);
        ImGui::Text("Avg FPS: %.1f", ImGui::GetIO().Framerate);

        static float conductivity = 1.0f;
        if (ImGui::SliderFloat("Conductivity", &conductivity, 0.1f, 10.0f))
        {
            // Here you would send 'conductivity' back to your solver
        }
        ImGui::End();

        // 3. Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(_window);
    }

    // -------------------------
    // Loop control
    // -------------------------
    bool Renderer::ShouldClose() const
    {
        return glfwWindowShouldClose(_window);
    }

    void Renderer::PollEvents()
    {
        glfwPollEvents();
    }

    // -------------------------
    // Cleanup
    // -------------------------
    void Renderer::Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glDeleteTextures(1, &_texture);
        glDeleteBuffers(1, &_vbo);
        glDeleteVertexArrays(1, &_vao);

        glfwDestroyWindow(_window);
        glfwTerminate();
    }
}