add_executable(HeatTransferApp
     main.cpp
)

target_link_libraries(HeatTransferApp
    PRIVATE Orchestration
)
