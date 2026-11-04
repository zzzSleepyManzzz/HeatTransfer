find_package(OpenMP REQUIRED)

add_library(Solvers
    src/ISolver.cpp
    src/IMultigrid.cpp
    src/MultigridPreconditionedConjugateGradientSolver.cpp
    src/Smoothers/Jacobi.cpp
    src/Smoothers/GaussSeidel.cpp
    src/Smoothers/SuccessiveOverRelaxation.cpp
    src/Smoothers/RedBlackSOR.cpp
)

target_include_directories(Solvers
    PUBLIC include
)

target_link_libraries(Solvers
    PUBLIC Core
    PUBLIC Eigen3::Eigen
    PRIVATE OpenMP::OpenMP_CXX

    PRIVATE Tracy::TracyClient
)

add_library(HeatTransfer::Solvers ALIAS Solvers)
