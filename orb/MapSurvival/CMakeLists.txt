cmake_minimum_required(VERSION 3.14)
project(MapPointSelection LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

# Eigen3 (header-only)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

add_executable(MapPointSelection MapPointSelection.cc)
target_link_libraries(MapPointSelection Eigen3::Eigen)
target_compile_options(MapPointSelection PRIVATE -O2 -Wall)
