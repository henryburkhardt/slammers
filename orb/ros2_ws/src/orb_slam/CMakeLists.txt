cmake_minimum_required(VERSION 3.8)
project(orb_slam)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-w)
  # add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -O3 -march=native ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -O3 -march=native")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread system)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(rclcpp REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(image_transport REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(OpenMP REQUIRED)

add_compile_definitions(EIGEN_DEFAULT_DENSE_INDEX_TYPE=int)
add_compile_definitions(EIGEN_DEFAULT_INDEX_TYPE=int)

include_directories(
${PROJECT_SOURCE_DIR}
${EIGEN3_INCLUDE_DIR}
${Boost_INCLUDE_DIRS}
include/orb_slam
Thirdparty
)

add_executable(${PROJECT_NAME}
src/main.cc
src/Tracking.cc
src/LocalMapping.cc
src/LoopClosing.cc
src/ORBextractor.cc
src/ORBmatcher.cc
src/FramePublisher.cc
src/Converter.cc
src/MapPoint.cc
src/KeyFrame.cc
src/Map.cc
src/MapPublisher.cc
src/Optimizer.cc
src/PnPsolver.cc
src/Frame.cc
src/KeyFrameDatabase.cc
src/Sim3Solver.cc
src/Initializer.cc
src/CV.cc
)
ament_target_dependencies(${PROJECT_NAME} rclcpp OpenCV Eigen3 std_msgs sensor_msgs cv_bridge tf2 image_transport visualization_msgs tf2_ros ament_index_cpp tf2_geometry_msgs)
target_link_libraries(${PROJECT_NAME}
${OpenCV_LIBS}
${EIGEN3_LIBS}
${PROJECT_SOURCE_DIR}/Thirdparty/DBoW2/lib/libDBoW2.so
${PROJECT_SOURCE_DIR}/Thirdparty/g2o/lib/libg2o.so
Boost::thread Boost::system
OpenMP::OpenMP_CXX
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    BUILD_RPATH "${PROJECT_SOURCE_DIR}/Thirdparty/DBoW2/lib"
    INSTALL_RPATH "$ORIGIN/../Thirdparty/DBoW2/lib"
)

install(TARGETS
${PROJECT_NAME}
DESTINATION lib/${PROJECT_NAME}
)

install(
  FILES Thirdparty/DBoW2/lib/libDBoW2.so
  FILES Thirdparty/g2o/lib/libg2o.so
  DESTINATION lib
)
# Install Data files.
install(DIRECTORY
  Data
  DESTINATION share/${PROJECT_NAME}/
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
