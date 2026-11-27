add_library(Core
    src/BoundaryConditions.cpp
    src/FaceBoundaryCondition.cpp
    src/Glyph.cpp
    src/InitialConditions.cpp
    src/MaterialProperties.cpp
    src/SimulationParameters.cpp
    src/Tensor3dMapper.cpp
)

target_include_directories(Core
    PUBLIC include
)

target_link_libraries(Core
    PUBLIC Eigen3::Eigen
)

add_library(HeatTransfer::Core ALIAS Core)
