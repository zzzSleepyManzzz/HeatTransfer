find_package(Threads REQUIRED)

add_library(SimulationRunner
    src/AsyncSimulation.cpp
    src/Simulation.cpp
)

target_include_directories(SimulationRunner
    PUBLIC include
)

target_link_libraries(SimulationRunner
    PUBLIC Core
    PUBLIC Solvers
    
    PUBLIC Threads::Threads

    PRIVATE Tracy::TracyClient
)

add_library(HeatTransfer::SimulationRunner ALIAS SimulationRunner)
