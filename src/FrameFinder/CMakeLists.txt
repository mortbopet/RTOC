cmake_minimum_required(VERSION 3.8)
project(FrameFinder)

find_package( OpenCV REQUIRED )
find_package( OpenCL REQUIRED )
find_package( Boost COMPONENTS system filesystem REQUIRED)

include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ${OpenCL_INCLUDE_DIRS} )

link_directories( ${OpenCV_LIBRARY})
link_directories( ${OpenCL_LIBRARY})

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp framefinder.h timer.h)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )
target_link_libraries( ${PROJECT_NAME} ${OpenCL_LIBS} )
# Link Boost libraries (<filesystem> and it's dependency <system>) directly
target_link_libraries( ${PROJECT_NAME} Boost::system Boost::filesystem)