include(FetchContent)

FetchContent_Declare(implot3d
    GIT_REPOSITORY https://github.com/brenocq/implot3d.git
    GIT_TAG dc53089543240d167bee1740604e32ed7061e8c6 # v0.5
)
FetchContent_MakeAvailable(implot3d)

add_library(Visualisation
    src/ConsoleLogger.cpp
    src/Renderer.cpp
    src/RendererModel.cpp
    src/RenderFrameData.cpp
    src/StatisticsData.cpp
    src/SliceData.cpp
    src/Slicer.cpp
    src/VolumeMesh.cpp
    src/VolumeMeshCache.cpp
    src/IPanel.cpp
    src/SidePanel.cpp
    src/PlotsPanel.cpp
    src/ConsolePanel.cpp

    ${implot3d_SOURCE_DIR}/implot3d.cpp
    ${implot3d_SOURCE_DIR}/implot3d_items.cpp
    ${implot3d_SOURCE_DIR}/implot3d_meshes.cpp
)

# Get ImGui's base path and store it
get_target_property(IMGUI_BASE_DIR imgui::imgui INTERFACE_INCLUDE_DIRECTORIES)
set(IMGUI_BINDINGS "${IMGUI_BASE_DIR}/../res/bindings")

target_sources(Visualisation PRIVATE
    "${IMGUI_BINDINGS}/imgui_impl_glfw.cpp"
    "${IMGUI_BINDINGS}/imgui_impl_opengl3.cpp"
)

target_include_directories(Visualisation
    PUBLIC include
    PUBLIC "${IMGUI_BINDINGS}"
)

target_include_directories(Visualisation SYSTEM
    PUBLIC "${implot3d_SOURCE_DIR}"
)

target_link_libraries(Visualisation
    PUBLIC SimulationRunner
    PUBLIC glfw
    PUBLIC glad::glad
    PUBLIC opengl::opengl
    PUBLIC imgui::imgui
    PUBLIC implot::implot

    PRIVATE Tracy::TracyClient
)

target_compile_definitions(Visualisation
    PRIVATE VISUALISATION_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

add_library(HeatTransfer::Visualisation ALIAS Visualisation)
