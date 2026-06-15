add_library(Visualisation src/Renderer.cpp)

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

target_link_libraries(Visualisation
    PUBLIC SimulationRunner
    PUBLIC glfw
    PUBLIC glad::glad
    PUBLIC imgui::imgui
    PUBLIC opengl::opengl
)

add_library(HeatTransfer::Visualisation ALIAS Visualisation)
