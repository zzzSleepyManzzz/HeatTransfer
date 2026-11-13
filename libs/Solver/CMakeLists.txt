find_package(OpenMP REQUIRED)

add_library(Solver
    src/ISolver.cpp
    src/Stencil.cpp
    src/MultigridPreconditioner.cpp
    src/MultigridPreconditionedConjugateGradientSolver.cpp
    src/Smoothers/Jacobi.cpp
    src/Smoothers/GaussSeidel.cpp
    src/Smoothers/SuccessiveOverRelaxation.cpp
    src/Smoothers/RedBlackSOR.cpp
    src/Smoothers/SmootherFactory.cpp
)

target_include_directories(Solver
    PUBLIC include
)

target_link_libraries(Solver
    PUBLIC Core
    PUBLIC Eigen3::Eigen
    PRIVATE OpenMP::OpenMP_CXX

    PRIVATE Tracy::TracyClient
)

add_library(HeatTransfer::Solver ALIAS Solver)
