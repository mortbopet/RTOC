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

set(SOURCE_FILES
    main.cpp )

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# SiliconSoftware runtime directories
include_directories($ENV{SISODIR5}/include)
include_directories($ENV{SISODIR5_64}/include)

# Link SiliconSoftware libraries
target_link_libraries( ${PROJECT_NAME}
    $ENV{SISODIR5}/lib/visualc/fglib5.lib
    $ENV{SISODIR5}/lib/visualc/siso_genicam.lib
    $ENV{SISODIR5}/lib/visualc/display_lib.lib
)
