find_package(OpenMP REQUIRED)

add_library(Solvers
    src/ISolver.cpp
    src/JacobiMethod.cpp
    src/GaussSeidelMethod.cpp
    src/SuccessiveOverRelaxation.cpp
    src/RedBlackSOR.cpp
)

target_include_directories(Solvers
    PUBLIC include
)

target_link_libraries(Solvers
    PUBLIC Core
    PUBLIC Eigen3::Eigen
    PRIVATE OpenMP::OpenMP_CXX
)

add_library(HeatTransfer::Solvers ALIAS Solvers)
