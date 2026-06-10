find_package(Catch2 REQUIRED)

# Unit tests
add_executable(unit_tests
    unit/ExpandMatrix_Test.cpp
    unit/SolverFactory_Test.cpp
)

target_link_libraries(unit_tests
    PRIVATE Catch2::Catch2WithMain
    HeatTransfer::Core
    HeatTransfer::Solvers
    HeatTransfer::SimulationRunner
)

# Integration tests
add_executable(integration_tests
    integration/JacobiMethod_Test.cpp
    integration/GaussSeidelMethod_Test.cpp
    integration/SuccessiveOverRelaxation_Test.cpp
    integration/SolverRelativePerformance_Test.cpp
)

target_link_libraries(integration_tests
    PRIVATE Catch2::Catch2WithMain
    HeatTransfer::Core
    HeatTransfer::Solvers
    HeatTransfer::SimulationRunner
)

add_test(NAME UnitTests COMMAND unit_tests)
add_test(NAME IntegrationTests COMMAND integration_tests)
