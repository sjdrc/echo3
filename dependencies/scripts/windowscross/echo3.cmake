# This file defines sources and settings for the echo3 library specific to this platform

set(ECHO_DOCTEST_DISCOVER OFF)

target_compile_definitions(
		echo3
		PUBLIC
		ECHO_PLATFORM_WINDOWS
		_WIN32_WINNT=0x0601
)

file(GLOB_RECURSE echo_boost_srcs CONFIGURE_DEPENDS src/Platforms/Boost/*.cpp)

target_sources(echo3
	PRIVATE
	${echo_boost_srcs}
	src/Platforms/Windows/Platform.cpp
	src/Platforms/Windows/SWindow.cpp
	src/Platforms/Windows/SystemInformation.cpp
	src/Platforms/Windows/WindowsExecutionModel.cpp
	src/Platforms/Windows/WindowsGLWindow.cpp
	src/Platforms/Windows/WindowsKeyboard.cpp
	src/Platforms/Windows/WindowsMouse.cpp
	src/Network/NetworkExecutionModel.cpp
)

target_link_libraries(
		echo3
		PUBLIC
		${CMAKE_DL_LIBS}
		Boost::chrono
		Boost::date_time
		Boost::filesystem
		Boost::random
		Boost::regex
		Boost::system
		Boost::thread
		Boost::timer
		ZLIB::ZLIB
		ws2_32
		wsock32
		userenv
)

if(BUILD_WITH_FREETYPE)
target_link_libraries(
		echo3
		PUBLIC
		freetype
)
endif()

install(FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/scripts/FindAndCopyDLLs.sh"
		DESTINATION scripts
)

