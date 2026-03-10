cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -march=native -Wall -Wextra")
project(ORB CXX)

find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)
# include_directories(${OpenCV_INCLUDE_DIRS})

# add_executable(harris src/harris.cpp)
# target_link_libraries(harris ${OpenCV_LIBS})

add_executable(FAST src/fast.cpp)
target_link_libraries(FAST ${OpenCV_LIBS})

# add_library(FASTLIB src/fast.cpp src/fast.hpp)
# target_include_directories(FASTLIB PUBLIC src)

# add_executable(ORB src/orb.cpp src/fast.hpp src/fast.cpp src/harris.hpp src/harris.cpp)
# target_link_libraries(ORB ${OpenCV_LIBS})

add_executable(trainrBrief src/trainRBrief.cpp src/processing.hpp src/processing.cpp src/harris.hpp src/harris.cpp)
target_link_libraries(trainrBrief ${OpenCV_LIBS} OpenMP::OpenMP_CXX)

# add_executable(blur src/blur.cpp src/processing.hpp src/processing.cpp)
# target_link_libraries(blur ${OpenCV_LIBS})

# add_executable(testORB src/testorb.cpp src/orb.cpp src/orb.hpp src/fast.cpp src/fast.hpp src/harris.cpp src/harris.hpp src/briefpattern.hpp src/processing.cpp src/processing.hpp)
# target_link_libraries(testORB ${OpenCV_LIBS})

add_executable(optim optimization_playground.cpp src/processing.hpp src/processing.cpp)
target_link_libraries(optim ${OpenCV_LIBS} OpenMP::OpenMP_CXX)
# target_include_directories(optim PRIVATE ${OpenCV_INCLUDE_DIRS})
# target_link_libraries(optim PRIVATE ${OpenCV_LIBS})

# target_link_libraries(ORB FASTLIB)
