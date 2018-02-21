PROJECT(AcquisitionMode)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

#where to find local cmake modules
FILE(TO_CMAKE_PATH "$ENV{SISODIR5}" SISODIR5)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_INSTALL_PREFIX}/cmake/modules" "${SISODIR5}/cmake/modules")

INCLUDE(SisoPlatform)
FIND_PACKAGE(FgLib5 REQUIRED)
FIND_PACKAGE(SisoDisplayLib REQUIRED)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

INCLUDE_DIRECTORIES(
	${FgLib5_INCLUDE_DIR}
	${SISODISPLAYLIB_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../../ExampleHelper
)

ADD_EXECUTABLE(FgXMLParamterInfo
	FgXMLParamterInfo.cpp
	main.cpp
	../../ExampleHelper/DmaMemWrapper.cpp
	../../ExampleHelper/FgWrapper.cpp
	../../Initialization/FgInitDll/FgInitDllExample.cpp
	../../ExampleHelper/FgExample.cpp
	../../include/fg_pixel.h)

TARGET_LINK_LIBRARIES(FgXMLParamterInfo
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
SET_TARGET_PROPERTIES(FgXMLParamterInfo PROPERTIES FOLDER "Examples/Functionality/AcquisitionMode")

INSTALL(TARGETS
	FgXMLParamterInfo
	DESTINATION bin/examples)
