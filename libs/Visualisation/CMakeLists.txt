add_library(Visualisation
    src/Renderer.cpp
)

target_include_directories(Visualisation
    PUBLIC include
)

target_link_libraries(Visualisation
    PUBLIC SimulationRunner
)

add_library(HeatTransfer::Visualisation ALIAS Visualisation)
