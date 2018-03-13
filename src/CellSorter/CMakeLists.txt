cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 11)
set (TEST_EXECUTABLE "CellSorter_test")

project(CellSorter)

#Environment variable $OpenCV_DIR must be set to the "build" folder of OpenCV
find_package(OpenCV REQUIRED)
find_package( Boost COMPONENTS system filesystem REQUIRED)

# Define all source and header files in src/

file(GLOB LIB_SOURCES lib/*.cpp)
file(GLOB LIB_HEADERS lib/*.h)

add_library(CellSorter_lib ${LIB_SOURCES} ${LIB_HEADERS})

# Define all source and header files from src/tst
file(GLOB TST_SRC tst/*.cpp)

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

# Create executable for cellsorter
add_executable(${PROJECT_NAME} main.cpp)

# Create executable for test project
add_executable(${TEST_EXECUTABLE} ${TST_SRC})

target_link_libraries(${TEST_EXECUTABLE} ${OpenCV_LIBS} CellSorter_lib)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} CellSorter_lib)

# Link Boost libraries
target_link_libraries( ${PROJECT_NAME} Boost::system Boost::filesystem)
