cmake_minimum_required(VERSION 3.8)
project(AcquisitionApp)

# Check that required environment variables are set
if( DEFINED ENV{SISODIR5} )
  message( MESSAGE "SISODIR5 environment variable detected at: " $ENV{SISODIR5}
      "\nThis variable is required for linking against SiliconSoftware provided
      headers and libraries.")
else( )
  message( FATAL_ERROR "SISODIR5 not set, aborting...")
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
    SiliconSoftwareSrc/*.cpp)
file(GLOB LIB_HEADERS
    src/*.h
    SiliconSoftwareSrc/*.h)

# set Qt specific files
file ( GLOB UIS
 src/*.ui
)

file ( GLOB RESOURCES
 resources/*.qrc
)



add_library(AcquisitionApp_lib ${LIB_SOURCES} ${LIB_HEADERS} ${UIS})

add_executable(${PROJECT_NAME} main.cpp)


# SiliconSoftware runtime directories
include_directories($ENV{SISODIR5}/include)
include_directories($ENV{SISODIR5_64}/include)

# Link SiliconSoftware libraries
target_link_libraries( ${PROJECT_NAME}
    $ENV{SISODIR5}/lib/visualc/fglib5.lib
    $ENV{SISODIR5}/lib/visualc/siso_genicam.lib
    $ENV{SISODIR5}/lib/visualc/display_lib.lib
)

# Link AcquisitionApp lib
target_link_libraries(${PROJECT_NAME} AcquisitionApp_lib  Qt5::Core Qt5::Widgets)
