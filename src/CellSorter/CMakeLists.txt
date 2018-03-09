cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 11)
set (TEST_EXECUTABLE "CellSorter_test")

project(CellSorter)

#Environment variable $OpenCV_DIR must be set to the "build" folder of OpenCV
find_package(OpenCV REQUIRED)

# Define all source and header files in src/

file(GLOB LIB_SOURCES lib/*.cpp)
file(GLOB LIB_HEADERS lib/*.h)

# Define all source and header files from src/tst
file(GLOB LIB_TST tst/*.cpp)

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} main.cpp ${LIB_SOURCES} ${LIB_HEADERS})

add_executable(${TEST_EXECUTABLE} ${LIB_TST} ${LIB_SOURCES} ${LIB_HEADERS})

target_link_libraries(${TEST_EXECUTABLE} ${OpenCV_LIBS} ${CellSorter_LIB})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${CellSorter_LIB})
