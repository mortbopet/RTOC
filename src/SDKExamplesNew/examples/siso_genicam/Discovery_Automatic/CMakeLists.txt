PROJECT(SgcAutoDiscoveryExample)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

#where to find local cmake modules
FILE(TO_CMAKE_PATH "$ENV{SISODIR5}" SISODIR5)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_INSTALL_PREFIX}/cmake/modules" "${SISODIR5}/cmake/modules")

INCLUDE(SisoPlatform)
FIND_PACKAGE(FgLib5 REQUIRED)
FIND_PACKAGE(SisoDisplayLib REQUIRED)
FIND_PACKAGE(SisoGenicamLib REQUIRED)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

INCLUDE_DIRECTORIES(
	${FgLib5_INCLUDE_DIR}
	${SISODISPLAYLIB_INCLUDE_DIR}
	${SisoGenicam_INCLUDE_DIR}	
	${CMAKE_CURRENT_SOURCE_DIR}/../../include
	${CMAKE_CURRENT_SOURCE_DIR}/../../ExampleHelper
	${CMAKE_CURRENT_SOURCE_DIR}/../../fglib/Acquisition/AcqBlock
	${CMAKE_CURRENT_SOURCE_DIR}/../../fglib/Initialization/FgAppletIterator/
	${CMAKE_CURRENT_SOURCE_DIR}/../../fglib/Initialization/FgDmaChannel
)

SET(EXAMPLE_SOURCE 	
	SgcAutoDiscoveryExample.cpp
	main.cpp
)

SET(HELPER_SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/../../ExampleHelper/DmaMemWrapper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../fglib/Acquisition/AcqBlock/FgAcqBlockExample.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../ExampleHelper/DisplayWrapper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../ExampleHelper/FgWrapper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../ExampleHelper/FgExample.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../ExampleHelper/SgcWrapper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../ExampleHelper/SgcExample.cpp
)

ADD_EXECUTABLE(SgcAutoDiscoveryExample
	${EXAMPLE_SOURCE}
	${HELPER_SOURCE}
)

source_group("src" FILES ${EXAMPLE_SOURCE} )
source_group("helper" FILES ${HELPER_SOURCE} )

TARGET_LINK_LIBRARIES(SgcAutoDiscoveryExample
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
	${SisoGenicam_LIBRARIES}
)
SET_TARGET_PROPERTIES(SgcAutoDiscoveryExample PROPERTIES FOLDER "siso_genicam/Discovery_Automatic")

INSTALL(TARGETS
	SgcAutoDiscoveryExample
	DESTINATION bin/examples)
