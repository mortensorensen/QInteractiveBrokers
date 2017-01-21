cmake_minimum_required (VERSION 3.1.0)

####################################################
# CONFIGURATION
####################################################
set (PROGRAM_NAME	"qib")
set (PROGRAM_VER    "0.0.1")
set (KX_VER         "3")
set (TWS_VER        "9.71.01")
set (BINARY_NAME	"${PROGRAM_NAME}.${PROGRAM_VER}")

option (BUILD_x64   "build a 64-bit binary instead of 32-bit"               OFF)
option (BUILD_DEBUG "build debug versions of the binaries with symbols"     ON)

project (${BINARY_NAME} CXX C)

set (CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/bin")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file (GLOB_RECURSE QIB_SRC "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_library (${BINARY_NAME} SHARED ${QIB_SRC})

include_directories ("${CMAKE_BINARY_DIR}/include"
										 "${CMAKE_SOURCE_DIR}/third_party/kx"
										 "${CMAKE_SOURCE_DIR}/third_party/ib/IBJts/source/PosixClient/Shared"
										 "${CMAKE_SOURCE_DIR}/third_party/ib/IBJts/source/PosixClient/src")

add_definitions (-DKXVER=3 -DIB_USE_STD_STRING)

set_target_properties (${BINARY_NAME} PROPERTIES
		PREFIX ""
		CXX_STANDARD 14
		CXX_STANDARD_REQUIRED ON)

set (CMAKE_CONFIGURATION_TYPES "Release;Debug;MinSizeRel;RelWithDebInfo")

if (${CMAKE_SYSTEM} MATCHES "Darwin")
	set_target_properties (${BINARY_NAME} PROPERTIES
		LINK_FLAGS "-undefined dynamic_lookup"
		SUFFIX ".so")
elseif (${CMAKE_SYSTEM} MATCHES "Linux")
	set_target_properties (${BINARY_NAME} PROPERTIES
		LINK_FLAGS "-shared -Wl,-rpath,.")
else ()
	message (FATAL_ERROR "QIB not configured for this OS. CMake will exit.")
endif ()

# Set 32-bit
if (NOT ${BUILD_x64})
	set (CMAKE_LIBRARY_ARCHITECTURES i386)
	if (${CMAKE_SYSTEM} MATCHES "Darwin")
		set (ARCHFLAGS "-arch i386 -DMac32")
	elseif (${CMAKE_SYSTEM} MATCHES "Linux")
		set (ARCHFLAGS "-march=i386")
		set_target_properties (${BINARY_NAME} PROPERTIES
			COMPILE_FLAGS "-m32"
			LINK_FLAGS "-m32")
	endif ()
endif ()

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARCHFLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARCHFLAGS}")

execute_process (COMMAND
	"git" describe --match=NeVeRmAtCh --always --abbrev=40 --dirty
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	OUTPUT_VARIABLE GIT_SHA1
	ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process (COMMAND
	"git" log -1 --format=%ad --date=local
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	OUTPUT_VARIABLE GIT_DATE
	ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process (COMMAND
	"date"
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	OUTPUT_VARIABLE BUILD_DATE
	ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

configure_file ("${CMAKE_SOURCE_DIR}/config.h.in" "${CMAKE_BINARY_DIR}/include/config.h")