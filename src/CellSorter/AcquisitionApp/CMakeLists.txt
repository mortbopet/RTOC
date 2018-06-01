cmake_minimum_required(VERSION 3.8)
project(AcquisitionApp)

SET(BUILD_ACQ ON CACHE BOOL "Build acquisition related tools")

if(COMMAND cmake_policy)
        cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# Check that required environment variables are set
if(${BUILD_ACQ})
    message( STATUS "\nBUILDING WITH ACQUISITION APP")
    if( DEFINED ENV{SISODIR5} )
        # Set preprocessor flag to trigger inclusion of acquisitor parts in program
        add_definitions(-DBUILD_ACQ)
      message( STATUS "\nSISODIR5 environment variable detected at: " $ENV{SISODIR5}
          "\nThis variable is required for linking against SiliconSoftware provided
          headers and libraries.")
      message( STATUS "\nRemember to set the working directory to the silicon software .bin directory, or other directories containing the required .dll's")
    else()
      message( WARNING "\nSISODIR5 not set!")
    endif()
endif()

set(CMAKE_CXX_STANDARD 11)

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

file(GLOB LIB_SOURCES
    src/*.cpp
    )
file(GLOB LIB_HEADERS
    src/*.h
    )

file(GLOB ACQUISITOR_SOURCES
    acquisitor_src/*.cpp)
file(GLOB ACQUISITOR_HEADERS
    acquisitor_src/*.h)

# set Qt specific files
file ( GLOB UIS
 src/*.ui
)

file ( GLOB RESOURCES
 resources/*.qrc
)

if(${BUILD_ACQ})
    add_library(acquisitor_lib ${ACQUISITOR_SOURCES} ${ACQUISITOR_HEADERS})
endif()

add_library(ACQ_LIB ${LIB_SOURCES} ${LIB_HEADERS} ${UIS})
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets ACQ_LIB)
target_include_directories(acquisitor_lib PUBLIC ${BOOST_INCLUDEDIR})


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
    target_include_directories(${PROJECT_NAME} PUBLIC ${BOOST_INCLUDEDIR})
    target_include_directories(acquisitor_lib PUBLIC ${BOOST_INCLUDEDIR})

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
    set_link_libraries(acquisitor_lib)
    set_link_libraries(${PROJECT_NAME})
else()
    if(UNIX)
        set(Boost_NO_SYSTEM_PATHS OFF)
        set(BOOST_INCLUDEDIR "$ENV{BOOST_ROOT}")
    endif()
    find_package(Boost COMPONENTS system filesystem serialization REQUIRED)

    macro(set_link_libraries app)
        target_link_libraries(${ARGV0} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SERIALIZATION_LIBRARY})
    endmacro()

    set_link_libraries(acquisitor_lib)
    set_link_libraries(${PROJECT_NAME})
endif()
######################################################################
## endof Boost setup
######################################################################

# Handle link and includes for acquisitor lib / silicon software related
if(${BUILD_ACQ})
    # the SiliconSoftware provided libraries use some old c-style includes - the below macro must be defined to avoid compile errors
    add_definitions(-D__STDC_LIMIT_MACROS)
    SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_INSTALL_PREFIX}/cmake/modules" "$ENV{SISODIR5}/cmake/modules")
    INCLUDE(SisoPlatform)
    FIND_PACKAGE(FgLib5 REQUIRED)
    FIND_PACKAGE(SisoGenicamLib REQUIRED)

    # SiliconSoftware runtime directories
    target_include_directories(acquisitor_lib PUBLIC ${FgLib5_INCLUDE_DIR} ${SisoGenicam_INCLUDE_DIR})

    # Link SiliconSoftware libraries
    target_link_libraries( ${PROJECT_NAME}
        ${FgLib5_LIBRARIES}
        ${SisoGenicam_LIBRARIES}
    )
    target_link_libraries(${PROJECT_NAME} acquisitor_lib )
    target_link_libraries( ACQ_LIB
        ${FgLib5_LIBRARIES}
        ${SisoGenicam_LIBRARIES}
    )
    target_link_libraries( ACQ_LIB
        acquisitor_lib
    )

endif()
