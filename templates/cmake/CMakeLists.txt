cmake_minimum_required(VERSION 3.14.3)

if(NOT CMAKE_CXX_STANDARD)
	set(CMAKE_CXX_STANDARD 14)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_INSTALL_RPATH)
	set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
endif()
if(NOT CMAKE_POSITION_INDEPENDENT_CODE)
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(ECHO_TARGET_PLATFORM "linux" CACHE STRING "Target platform to build for")
set_property(CACHE ECHO_TARGET_PLATFORM PROPERTY STRINGS linux wii)

if(NOT ECHO_ENGINE_INSTALL_DIR)
	message(FATAL_ERROR "Please specify the location of Echo by defining ECHO_ENGINE_INSTALL_DIR with -DECHO_ENGINE_INSTALL_DIR=path/to/echo3/install when invoking cmake.")
endif()

set(DEPS_ROOT "${ECHO_ENGINE_INSTALL_DIR}/${ECHO_TARGET_PLATFORM}" CACHE PATH "Path to dependencies install root")
set(NO_DEFAULT_PATH ON)
set(CMAKE_FIND_ROOT_PATH "${DEPS_ROOT}/lib/cmake;${DEPS_ROOT}/share;${DEPS_ROOT}/CMake;${DEPS_ROOT}/cmake")
set(ENV{PKG_CONFIG_PATH} "${DEPS_ROOT}/lib/pkgconfig:${DEPS_ROOT}/share/pkgconfig")

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
	message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

### Set build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to 'Release' as no build type was specified")
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type (Debug/Release)" FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)
endif()

find_package(PkgConfig REQUIRED)

SET (BOOST_ROOT "${DEPS_ROOT}")
SET (BOOST_INCLUDEDIR "${BOOST_ROOT}/include/boost")
SET (BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")

set(ENV{PKG_CONFIG_PATH} "${DEPS_ROOT}/lib/pkgconfig:${DEPS_ROOT}/share/pkgconfig")

################################################################################
################ Project configuration
################################################################################

project(MyProjectApplication)

find_package(echo3 CONFIG REQUIRED)

file(GLOB_RECURSE project_srcs CONFIGURE_DEPENDS
	src/*.cpp
)

add_executable(MyProjectApplication
	${project_srcs}
)

target_include_directories(MyProjectApplication
	PUBLIC
	include
)

target_link_libraries(
	MyProjectApplication
	PUBLIC
	echo3::echo3
)

install(TARGETS MyProjectApplication
	EXPORT MyProjectApplicationTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)
