add_library(Core
    INTERFACE
)

target_include_directories(Core
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(HeatTransfer::Core ALIAS Core)
