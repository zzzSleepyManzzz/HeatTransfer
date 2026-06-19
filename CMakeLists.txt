cmake_minimum_required(VERSION 3.20)

project(HeatTransfer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Eigen3 REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glad REQUIRED)
find_package(imgui REQUIRED)
find_package(implot REQUIRED)


add_subdirectory(libs/Core)
add_subdirectory(libs/SimulationRunner)
add_subdirectory(libs/Solvers)
add_subdirectory(libs/Visualisation)

add_subdirectory(apps/HeatTransferApp)
add_subdirectory(apps/HeatTransferRender)

enable_testing()
add_subdirectory(tests)
