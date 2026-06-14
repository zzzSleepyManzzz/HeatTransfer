add_executable(HeatTransferRender
     main.cpp
)

target_link_libraries(HeatTransferRender
    PRIVATE SimulationRunner
    PRIVATE Visualisation
)
