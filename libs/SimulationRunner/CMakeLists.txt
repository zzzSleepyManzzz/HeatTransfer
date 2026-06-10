add_library(SimulationRunner
    src/SolverFactory.cpp
    src/Simulations.cpp
)

target_include_directories(SimulationRunner
    PUBLIC include
)

target_link_libraries(SimulationRunner
    PUBLIC Core
    PUBLIC Solvers
)

add_library(HeatTransfer::SimulationRunner ALIAS SimulationRunner)
