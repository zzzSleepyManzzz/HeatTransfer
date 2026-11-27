add_library(Orchestration 
    src/Application.cpp
    src/SimpleSteadyStateApplication.cpp
    src/SimpleTransientApplication.cpp
)

target_include_directories(Orchestration
    PUBLIC include
)

target_link_libraries(Orchestration
    PUBLIC SimulationRunner
    PUBLIC Visualisation

    PRIVATE Tracy::TracyClient
)

add_library(HeatTransfer::Orchestration ALIAS Orchestration)
