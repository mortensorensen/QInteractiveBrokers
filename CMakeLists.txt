cmake_minimum_required (VERSION 3.1.0)

####################################################
# CONFIGURATION
####################################################
set (PROGRAM_NAME     "qib")
set (PROGRAM_VER      "0.0.1")
set (KX_VER           "3")
set (TWS_VER          "9.71.01")
set (BINARY_NAME      "${PROGRAM_NAME}.${PROGRAM_VER}")

option (BUILD_x86   "build a 32-bit binary instead of 64-bit"               ON)
option (BUILD_DEBUG "build debug versions of the binaries with symbols"     ON)

project (${BINARY_NAME} CXX C)

file (MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

set (CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/bin")

file (GLOB_RECURSE QIB_SRC
	"${CMAKE_SOURCE_DIR}/src/*.h"
	"${CMAKE_SOURCE_DIR}/src/*.cpp")

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

if (DEFINED Mac32)
		set (ARCHFLAGS "-arch i386 -DMac32")
elseif (${CMAKE_SYSTEM} MATCHES "Linux" AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES "i[3-6]86")
		set (ARCHFLAGS "-march=i486")
else ()
		set (ARCHFLAGS "-arch i386")
endif ()

set (CMAKE_OSX_ARCHITECTURES i386) # TODO: Improve

set (CMAKE_CONFIGURATION_TYPES "Release;Debug;MinSizeRel;RelWithDebInfo")

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARCHFLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARCHFLAGS}")

if (${CMAKE_SYSTEM} MATCHES "Linux")
		set_target_properties (${BINARY_NAME} PROPERTIES
					COMPILE_FLAGS "-fPIC"
					LINK_FLAGS "-shared -Wl,-rpath,.")
elseif (${CMAKE_SYSTEM} MATCHES "Darwin")
		set_target_properties (${BINARY_NAME} PROPERTIES
					BUILD_WITH_INSTALL_RPATH 1
					INSTALL_NAME_DIR "@rpath"
					LINK_FLAGS "-undefined dynamic_lookup"
					SUFFIX ".so")
endif ()

# Move finished binaries into binary folder if installing
install (TARGETS ${BINARY_NAME} DESTINATION "${CMAKE_SOURCE_DIR}/bin")

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
