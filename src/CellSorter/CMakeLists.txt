cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 11)
set (TEST_EXECUTABLE "CellSorter_test")

project(CellSorter)

#Environment variable $OpenCV_DIR must be set to the "build" folder of OpenCV
find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

# Define all source and header files in src/

file(GLOB LIB_SOURCES lib/*.cpp)
file(GLOB LIB_HEADERS lib/*.h)

add_library(CellSorter_lib ${LIB_SOURCES} ${LIB_HEADERS})

# Define all source and header files from src/tst
file(GLOB TST_SRC tst/*.cpp)

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

# Create executables for cellsorter and cellsort_test
add_executable(${PROJECT_NAME} main.cpp)
add_executable(${TEST_EXECUTABLE} ${TST_SRC})

# Link OpenCV libraries
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} CellSorter_lib)
target_link_libraries(${TEST_EXECUTABLE} ${OpenCV_LIBS} CellSorter_lib)

target_link_libraries(CellSorter_lib ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})

# Link Boost libraries
target_link_libraries( ${PROJECT_NAME} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries( ${TEST_EXECUTABLE} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})


######################################################################
## Catch2 CMake setup
######################################################################
set (CATCH_TEST_EXECUTABLE "CellSorter_test_catch2")

# Prepare catch2 library
set(CATCH_INCLUDE_DIR tst_catch/catch.hpp)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

# Create executable for catch2 test project
set(CATCH_TEST_SOURCES tst_catch/main tst_catch/catch_framefinder.cpp)
add_executable(${CATCH_TEST_EXECUTABLE} ${CATCH_TEST_SOURCES})
# Link catch
target_link_libraries(${CATCH_TEST_EXECUTABLE} Catch)
# Link boost
target_link_libraries( ${CATCH_TEST_EXECUTABLE} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
# Link OpenCV and Cellsorter lib
target_link_libraries(${CATCH_TEST_EXECUTABLE} ${OpenCV_LIBS} CellSorter_lib)
######################################################################
## endof Catch2 CMake setup
######################################################################
