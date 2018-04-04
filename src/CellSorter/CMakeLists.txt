cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 14)
project(CellSorter)

#Environment variable $OpenCV_DIR must be set to the "build" folder of OpenCV
find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

# Define all source and header files in src/

file(GLOB LIB_SOURCES lib/*.cpp)
file(GLOB LIB_HEADERS lib/*.h)

add_library(CellSorter_lib ${LIB_SOURCES} ${LIB_HEADERS})

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

# Create executables for cellsorter and cellsort_test
add_executable(${PROJECT_NAME} main.cpp)

# Link OpenCV libraries
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} CellSorter_lib)

target_link_libraries(CellSorter_lib ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})

# Link Boost libraries
target_link_libraries( ${PROJECT_NAME} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})


######################################################################
## Catch2 CMake setup - TEST PROJECT
######################################################################
set (TEST_EXECUTABLE "CellSorter_test")

# Prepare catch2 library
set(CATCH_INCLUDE_DIR tst_catch/catch.hpp)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

# Create executable for catch2 test project
file(GLOB CATCH_TEST_SOURCES tst/*.cpp)
add_executable(${TEST_EXECUTABLE} ${CATCH_TEST_SOURCES})
# Link catch
target_link_libraries(${TEST_EXECUTABLE} Catch)
# Link boost
target_link_libraries( ${TEST_EXECUTABLE} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
# Link OpenCV and Cellsorter lib
target_link_libraries(${TEST_EXECUTABLE} ${OpenCV_LIBS} CellSorter_lib)
######################################################################
## endof Catch2 CMake setup
######################################################################



######################################################################
## GUI setup
######################################################################
SET(BUILD_GUI OFF CACHE BOOL "Build GUI")
if(${BUILD_GUI})
    # Set preprocessor flag to trigger gui in main.cpp
    add_definitions(-DBUILD_GUI)

    # Instruct CMake to run moc automatically when needed.
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    set(CMAKE_AUTOUIC ON)

    # Find Qt5Core and Qt5Widgets
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Widgets REQUIRED)

    # For finding Qt includes
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
    include_directories(${Qt5Core_INCLUDE_DIRS})

    file(GLOB GUI_SRC
        gui/*.cpp
        )
    file(GLOB GUI_H
        gui/*.h
        )

    # set Qt specific files
    file ( GLOB UIS
     gui/*.ui
    )

    file ( GLOB RESOURCES
     gui/*.qrc
    )

    add_definitions(-fPIC)
    add_library(gui ${GUI_SRC} ${GUI_H} ${UIS})
    target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets gui)
endif()
######################################################################
## endof GUI setup
######################################################################
