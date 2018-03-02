cmake_minimum_required(VERSION 3.8)
project(CheckDevices)

find_package( OpenCV REQUIRED )
find_package( OpenCL REQUIRED )


include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ${OpenCL_INCLUDE_DIRS} )
link_directories( ${OpenCV_LIBRARY})
link_directories( ${OpenCL_LIBRARY})

set(CMAKE_CXX_STANDARD 11)

add_executable(CheckDevices main.cpp)

target_link_libraries( CheckDevices ${OpenCV_LIBS} )
target_link_libraries( CheckDevices ${OpenCL_LIBS} )