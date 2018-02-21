CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
PROJECT(SISO_EXAMPLES)
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

IF(UNIX)
	IF(CMAKE_COMPILER_IS_GNUCC)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	ENDIF(CMAKE_COMPILER_IS_GNUCC)
ENDIF(UNIX)

add_subdirectory("fglib/Initialization/FgAppletIterator")
add_subdirectory("fglib/Initialization/FgInitConfig")
add_subdirectory("fglib/Initialization/FgDmaChannel")

add_subdirectory("fglib/Acquisition/AcqAPC")
add_subdirectory("fglib/Acquisition/AcqBlock")
add_subdirectory("fglib/Acquisition/AcqMasterSlave")
add_subdirectory("fglib/Acquisition/AcqStandard")
add_subdirectory("fglib/Acquisition/AcqAPCBlock")

add_subdirectory("fglib/Events")

add_subdirectory("fglib/Parametrization/FgParameterIteration")
add_subdirectory("fglib/Parametrization/FgRuntimeInfo")
add_subdirectory("fglib/Parametrization/FgXMLParameterInfo")
add_subdirectory("fglib/Parametrization/FgSaveConfig")

add_subdirectory("gbe/CameraDetection")
add_subdirectory("gbe/CameraConnection")
add_subdirectory("gbe/Acquisition")
add_subdirectory("gbe/Events")

add_subdirectory("siso_genicam/Acquisition")
add_subdirectory("siso_genicam/AcquisitionWithLogging")
add_subdirectory("siso_genicam/Discovery_Automatic")
add_subdirectory("siso_genicam/Discovery_Manual")
add_subdirectory("siso_genicam/Parameter")

add_subdirectory("iolibrt/tiff")