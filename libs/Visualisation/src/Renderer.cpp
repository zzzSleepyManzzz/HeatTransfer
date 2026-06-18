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

        InitImGui();
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
        // ALMOST-BOILERPLATE

        // 1. Generate n texture ids (usually 1) and store it in _texture
        // 2. Bind a 2D texture to the texture id
        // 3. Configure the bound texture e.g linear shrinking and magnifacation

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
        // SEMI-BOILERPLATE

        // Important concepts:
        //  - Vertex data : flat array of interleaving [x,y,u,v]
        //    (x,y) is the vertex position on a normalised device coordinate i.e. (-1,-1) to (1,1)
        //    (u,v) is the texture coordinates stretching from (0,0) to (1,1)
        //  - Vertex buff object (vbo): Chooses memory buffer in GPU for vertex data
        //  - Vertex array object (vao): Record configuration
        //  - Pipeline: Vbo takes the vertex data, places it in a memory buffer. Vao then sets a
        //    configuration on how the GPU should read the bytes in the memory buffer. This read
        //    information is then passed to the shader!

        // 1. First we must create a vertices array, which is composed of interleaving [x,y,u,v]
        //    If we want to create a quad, we essentially interleave 2 triangles, as given:
        //
        //      float vertices[] = {
        //          // === TRIANGLE 1 ===
        //          -1.f, -1.f,   0.f, 0.f,  // Bottom-Left  (Pos: -1, -1 | UV: 0, 0)
        //          1.f, -1.f,   1.f, 0.f,   // Bottom-Right (Pos:  1, -1 | UV: 1, 0)
        //          1.f,  1.f,   1.f, 1.f,   // Top-Right    (Pos:  1,  1 | UV: 1, 1)
        //
        //          // === TRIANGLE 2 ===
        //          -1.f, -1.f,   0.f, 0.f,  // Bottom-Left  (Pos: -1, -1 | UV: 0, 0)
        //          1.f,  1.f,   1.f, 1.f,   // Top-Right    (Pos:  1,  1 | UV: 1, 1)
        //          -1.f,  1.f,   0.f, 1.f   // Top-Left     (Pos: -1,  1 | UV: 0, 1)
        //      };

        // 2. Generate n vao (vertex array object) ids (usually 1) and store it in _vao
        // 3. Generate n vbo (vertex buffer object) ids (usually 1) and store it in _vbo

        // 4. Activate (bind) your vao - it starts recording
        // 5. Activate (bind) your vbo - chooses memory buffer on GPU
        // 6. glBufferData - Allocates GPU memory and copies your quad vertex data into it

        // 7. glVertexAttribPointer - Tell OpenGl how to read the vertex layout (x,y) and (u,v)
        // 8. glEnableVertexAttribArray - Enable the layout i.e the vao records this

        float vertices[] = {-1.f, -1.f, 0.f, 0.f, 1.f, -1.f, 1.f, 0.f, 1.f,  1.f, 1.f, 1.f,
                            -1.f, -1.f, 0.f, 0.f, 1.f, 1.f,  1.f, 1.f, -1.f, 1.f, 0.f, 1.f};

        // Generate ids
        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);

        // Activate/bind
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        // Copy buffer data
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Set position layout config
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Set texture layout config
        glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    // -------------------------
    // Shader (placeholder)
    // -------------------------

    void Renderer::InitShader()
    {
        // SEMI-BOILERPLATE

        // Important concepts:
        //  - First vbo places vertex data on memory buffer of GPU and the vao passes configurations
        //    to the GPU. Next, the GPU interprets the byte data and passes it to the shader
        //    program. The shader program (composed of vertex and fragment shader) takes the inputs
        //    and then gives outputs e.g. Vertex Position, Texture Position, FragColor. After this,
        //    OpenGL can draw the triangles/quads.

        // 1. Obtain vertex and fragment shader sources
        // 2. Compile vertex shader
        // 3. Compile fragment shader
        // 4. Create shader program
        // 5. Clean-up

        // Obtain vertex and fragment sources as char*
        std::string vertexShaderSourceString = ShaderFileSource::LoadVertexShaderSource();
        std::string fragmentShaderSourceString = ShaderFileSource::LoadFragmentShaderSource();

        const char* vertexShaderSource = vertexShaderSourceString.c_str();
        const char* fragmentShaderSource = fragmentShaderSourceString.c_str();

        GLint success;
        GLchar infoLog[512];

        // Compile vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            throw std::runtime_error("Vertex shader compilation failed: \n" + std::string(infoLog));
        }

        // Compile fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            throw std::runtime_error("Fragment shader compilation failed: \n" +
                                     std::string(infoLog));
        }

        // Create shader program
        _shaderProgram = glCreateProgram();
        glAttachShader(_shaderProgram, vertexShader);
        glAttachShader(_shaderProgram, fragmentShader);
        glLinkProgram(_shaderProgram);

        glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(_shaderProgram, 512, NULL, infoLog);
            throw std::runtime_error("Shader program linking failed: \n" + std::string(infoLog));
        }

        // Clean up
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void Renderer::InitImGui()
    {
        // BOILERPLATE

        // 1. Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        // 2. Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(_window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
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

        glUseProgram(_shaderProgram);
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