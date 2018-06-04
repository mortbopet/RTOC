cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 14)

cmake_policy(SET CMP0020 NEW)
project(CellSorter)

#Environment variable $OpenCV_DIR must be set to the "build" folder of OpenCV
find_package(OpenCV REQUIRED)

# Define all source and header files in src/
file(GLOB LIB_SOURCES lib/*.cpp)
file(GLOB LIB_HEADERS lib/*.h)


######################################################################
## GUI setup
######################################################################
if(WIN32)
else()
add_definitions(-fPIC)
endif()

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

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
 ../*.qrc
)

add_library(CellSorter_lib ${LIB_SOURCES} ${LIB_HEADERS} ${GUI_SRC} ${GUI_H} ${UIS} ${RESOURCES} lib/machinelearning.cpp lib/machinelearning.h)
target_link_libraries(CellSorter_lib Qt5::Core Qt5::Widgets)
######################################################################
## endof GUI setup
######################################################################



INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

# Create executables for cellsorter and cellsort_test
add_executable(${PROJECT_NAME} main.cpp)

# Link OpenCV libraries
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} CellSorter_lib)


SET(BUILD_TESTS OFF CACHE BOOL "Build with Catch unit tests")
if(${BUILD_TESTS})
######################################################################
## Catch2 setup - TEST PROJECT
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
# Link OpenCV and Cellsorter lib
target_link_libraries(${TEST_EXECUTABLE} ${OpenCV_LIBS} CellSorter_lib)

######################################################################
## endof Catch2 setup
######################################################################
endif()


######################################################################
## Boost setup
######################################################################
if(WIN32)
    # Find Boost
    set(Boost_NO_SYSTEM_PATHS OFF)
    set(BOOST_INCLUDEDIR "$ENV{BOOST_ROOT}")
    find_package(Boost REQUIRED)
    message(STATUS ${BOOST_INCLUDEDIR})

    # include boost
    target_include_directories(CellSorter_lib PUBLIC ${BOOST_INCLUDEDIR})
    target_include_directories(${PROJECT_NAME} PUBLIC ${BOOST_INCLUDEDIR})

    macro(set_link_libraries app)
        target_link_libraries(${ARGV0}
            debug     "$ENV{BOOST_ROOT}/lib64-msvc-14.1/libboost_system-vc141-mt-gd-x64-1_66.lib"
            debug     "$ENV{BOOST_ROOT}/lib64-msvc-14.1/libboost_filesystem-vc141-mt-gd-x64-1_66.lib"
            debug     "$ENV{BOOST_ROOT}/lib64-msvc-14.1/libboost_serialization-vc141-mt-gd-x64-1_66.lib"
            optimized "$ENV{BOOST_ROOT}/lib64-msvc-14.1/libboost_system-vc141-mt-x64-1_66.lib"
            optimized "$ENV{BOOST_ROOT}/lib64-msvc-14.1/libboost_filesystem-vc141-mt-x64-1_66.lib"
            optimized "$ENV{BOOST_ROOT}/lib64-msvc-14.1/libboost_serialization-vc141-mt-x64-1_66.lib"
            )
    endmacro()

    # On windows, the app has been compiled with boost 1_66 and MSVC-14 (MSVC2015)
    set_link_libraries(CellSorter_lib)
    set_link_libraries(${PROJECT_NAME})
    if(${BUILD_TESTS})
    set_link_libraries( ${TEST_EXECUTABLE})
    endif()
else()
    if(UNIX)
        set(Boost_NO_SYSTEM_PATHS OFF)
        set(BOOST_INCLUDEDIR "$ENV{BOOST_ROOT}")
    endif()
    find_package(Boost COMPONENTS system filesystem serialization REQUIRED)

    macro(set_link_libraries app)
        target_link_libraries(${ARGV0} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SERIALIZATION_LIBRARY})
    endmacro()

    set_link_libraries(CellSorter_lib)
    set_link_libraries(${PROJECT_NAME})
    if(${BUILD_TESTS})
    set_link_libraries(${TEST_EXECUTABLE})
endif()
endif()
######################################################################
## endof Boost setup
######################################################################


######################################################################
## Doxygen setup
######################################################################
# first we can indicate the documentation build as an option and set it to ON by default
SET(BUILD_DOCS OFF CACHE BOOL "Build with doxygen documentation generation")

if(${BUILD_DOCS})
    # check if Doxygen is installed
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/../../docs/doxygen_conf)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        # note the option ALL which allows to build the docs together with the application
        add_custom_target( doc_doxygen ALL
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM )
    else (DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif()
######################################################################
## endof Doxygen setup
######################################################################

######################################################################
## IronMan camera lib setup
######################################################################
SET(BUILD_IRONMAN OFF CACHE BOOL "Build acquisition related tools")
if(${BUILD_IRONMAN})
    add_definitions(-DBUILD_IRONMAN)
    add_subdirectory(ironman_lib)
    target_link_libraries(CellSorter_lib ironman_lib)
endif()
######################################################################
## end of IronMan camera lib setup
######################################################################




## copy test images to output directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../test_images DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../presets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
