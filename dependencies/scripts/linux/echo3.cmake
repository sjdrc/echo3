# This file defines sources and settings for the echo3 library specific to this platform

target_compile_definitions(
		echo3
		PUBLIC
		ECHO_PLATFORM_LINUX
)

file(GLOB_RECURSE echo_linux_srcs CONFIGURE_DEPENDS src/Platforms/Linux/*.cpp)
file(GLOB_RECURSE echo_posix_srcs CONFIGURE_DEPENDS src/Platforms/POSIX/*.cpp)
file(GLOB_RECURSE echo_boost_srcs CONFIGURE_DEPENDS src/Platforms/Boost/*.cpp)

target_sources(echo3
	PRIVATE
	${echo_linux_srcs}
	${echo_posix_srcs}
	${echo_boost_srcs}
	src/Network/NetworkExecutionModel.cpp
)

target_link_libraries(
		echo3
		PUBLIC
		Boost::chrono
		Boost::date_time
		Boost::filesystem
		Boost::random
		Boost::regex
		Boost::system
		Boost::thread
		Boost::timer
)

if(BUILD_WITH_FREETYPE)
target_link_libraries(
		echo3
		PUBLIC
		${CMAKE_DL_LIBS}
		freetype
)
endif()