project(ObjectMeasurement)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(ObjectMeasurement src/Object_Measurement.cpp)

target_link_libraries(ObjectMeasurement ${OpenCV_LIBS})