cmake_minimum_required(VERSION 3.14)
project(MapPointSelection LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(MapPointSelection MapPointSelection.cc)
target_compile_options(MapPointSelection PRIVATE -O2 -Wall)