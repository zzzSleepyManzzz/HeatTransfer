add_library(Visualisation 
    src/ConsoleLogger.cpp
    src/Renderer.cpp

    thirdparty/implot3d/implot3d.cpp         # Vendored source
    thirdparty/implot3d/implot3d_demo.cpp    # Vendored source
    thirdparty/implot3d/implot3d_items.cpp   # Vendored source
    thirdparty/implot3d/implot3d_meshes.cpp  # Vendored source
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

    PUBLIC thirdparty/implot3d # Vendored include
)

target_link_libraries(Visualisation
    PUBLIC SimulationRunner
    PUBLIC glfw
    PUBLIC glad::glad
    PUBLIC opengl::opengl
    PUBLIC imgui::imgui
    PUBLIC implot::implot
)

target_compile_definitions(Visualisation
    PRIVATE VISUALISATION_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

add_library(HeatTransfer::Visualisation ALIAS Visualisation)
