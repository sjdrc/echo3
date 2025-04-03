# This file defines sources and settings for the echo3 library specific to this platform

target_include_directories(echo3
	PUBLIC
	$ENV{DEVKITPRO}/libogc/include
)

include(cmake/wii/libogcConfig.cmake)

target_compile_definitions(
		echo3
		PUBLIC
		ECHO_PLATFORM_NINTENDO_WII
		GEKKO
		#GOOGLE_PROTOBUF_NO_THREAD_SAFETY
)

file(GLOB_RECURSE echo_wii_srcs CONFIGURE_DEPENDS src/Platforms/Wii/*.cpp)
target_sources(echo3
	PRIVATE
	${echo_wii_srcs}
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	message("Adding libogc debug support")
	target_link_libraries(echo3 PUBLIC db)
else()
	message("No libogc debug support included")
endif()

target_link_libraries(
		echo3
		PUBLIC
		libogc::libfat
		libogc::libwiiuse
		libogc::libbte
		libogc::libogc
		freetype
		Boost::regex
		PkgConfig::zlib
		
)

install(FILES "cmake/wii/libogcConfig.cmake"
		DESTINATION lib/cmake/libogc
)
