add_library(Orchestration 
    src/Application.cpp
    src/SimpleApplication.cpp
)

target_include_directories(Orchestration
    PUBLIC include
)

target_link_libraries(Orchestration
    PUBLIC SimulationRunner
    PUBLIC Visualisation
)

add_library(HeatTransfer::Orchestration ALIAS Orchestration)
