add_executable(HeatTransferTransientRunner
     main.cpp
)

target_link_libraries(HeatTransferTransientRunner
    PRIVATE Orchestration
)
