cmake_minimum_required(VERSION 3.8)
project(AcquisitionApp)

SET(BUILD_ACQ ON CACHE BOOL "Build acquisition related tools")

# Check that required environment variables are set
if(${BUILD_ACQ})
    if( DEFINED ENV{SISODIR5} )
        # Set preprocessor flag to trigger inclusion of acquisitor parts in program
        add_definitions(-DBUILD_ACQ)
      message( STATUS "\nSISODIR5 environment variable detected at: " $ENV{SISODIR5}
          "\nThis variable is required for linking against SiliconSoftware provided
          headers and libraries.")
      message( STATUS "\nRemember to set the working directory to the silicon software .bin directory, or other directories containing the required .dll's")
    else( )
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

add_executable(${PROJECT_NAME} main.cpp ${LIB_SOURCES} ${LIB_HEADERS})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets)


# Handle link and includes for acquisitor lib / silicon software related
if(${BUILD_ACQ})
    # SiliconSoftware runtime directories
    include_directories($ENV{SISODIR5}/include)
    include_directories($ENV{SISODIR5_64}/include)

    # Link SiliconSoftware libraries
    target_link_libraries( ${PROJECT_NAME}
        $ENV{SISODIR5}/lib/visualc/fglib5.lib
        $ENV{SISODIR5}/lib/visualc/siso_genicam.lib
        $ENV{SISODIR5}/lib/visualc/display_lib.lib
    )
    target_link_libraries(${PROJECT_NAME} acquisitor_lib )
endif()
