PROJECT(FgAcqMasterSlaveExample)

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
	${CMAKE_CURRENT_SOURCE_DIR}/../../../ExampleHelper
	${CMAKE_CURRENT_SOURCE_DIR}/../../Initialization/FgAppletIterator
	${CMAKE_CURRENT_SOURCE_DIR}/../../Initialization/FgDmaChannel
	${CMAKE_CURRENT_SOURCE_DIR}/../AcqBlock
)

SET(EXAMPLE_SOURCE 	
	FgAcqMasterSlaveExample.h
	FgAcqMasterSlaveExample.cpp
	main.cpp
)

SET(HELPER_SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/../AcqBlock/FgAcqBlockExample.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../../ExampleHelper/DmaMemWrapper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../Initialization/FgAppletIterator/FgAppletIteratorExample.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../../ExampleHelper/DisplayWrapper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../../ExampleHelper/FgWrapper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../../ExampleHelper/FgExample.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../../include/fg_pixel.h
)

ADD_EXECUTABLE(FgAcqMasterSlaveExample
	${EXAMPLE_SOURCE}
	${HELPER_SOURCE})

TARGET_LINK_LIBRARIES(FgAcqMasterSlaveExample
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
source_group("src" FILES ${EXAMPLE_SOURCE} )
source_group("helper" FILES ${HELPER_SOURCE} )
SET_TARGET_PROPERTIES(FgAcqMasterSlaveExample PROPERTIES FOLDER "fglib/Acquisition/AcqMasterSlave")

INSTALL(TARGETS
	FgAcqMasterSlaveExample
	DESTINATION bin/examples)
