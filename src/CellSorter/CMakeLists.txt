cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 11)

project(CellSorter)

#Environment variable $OpenCV_DIR must be set to the "build" folder of OpenCV
find_package(OpenCV REQUIRED)

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
add_executable(${PROJECT_NAME}
    "main.cpp"
    "prettyprint.h"
    "rbc.h"
        analyzer.h process.h analyzer.cpp process.cpp patient.h parameter.h)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
