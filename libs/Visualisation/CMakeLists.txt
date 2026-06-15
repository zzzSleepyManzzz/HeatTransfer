add_library(Visualisation
    src/Renderer.cpp
)

target_include_directories(Visualisation
    PUBLIC include
)

target_link_libraries(Visualisation
    PUBLIC SimulationRunner
    PUBLIC glfw
    PUBLIC glad::glad
    PUBLIC imgui::imgui
)

add_library(HeatTransfer::Visualisation ALIAS Visualisation)
