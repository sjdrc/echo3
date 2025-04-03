cmake_minimum_required(VERSION 3.14.3)

################################################################################
################ Project initialisation
################################################################################

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

#This option determines the default
set(ECHO_TARGET_PLATFORM "linux" CACHE STRING "Target platform to build for")
set_property(CACHE ECHO_TARGET_PLATFORM PROPERTY STRINGS linux wii windowscross)

### Feature options
set(ECHO_UI_FRAMEWORK "GTK" CACHE STRING "GUI Framework to build support for")
set_property(CACHE ECHO_UI_FRAMEWORK PROPERTY STRINGS Null Qt GTK Native)
set(ECHO_AUDIO_SYSTEM "Null" CACHE STRING "Audio System to build support for")
set_property(CACHE ECHO_AUDIO_SYSTEM PROPERTY STRINGS Null OpenAL)
option(BUILD_WITH_JPEG "Build with JPEG support" ON)
option(BUILD_WITH_PNG "Build with PNG support" ON)
option(BUILD_WITH_OPENGL "Build with OpenGL support" ON)
option(BUILD_WITH_SOCKETS "Build with socket networking support" ON)
option(BUILD_WITH_WEBSOCKETS "Build with Websocket support" ON)
option(BUILD_WITH_SSL "Build with SSL support" ON)
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_TOOLS "Build tools" ON)
option(BUILD_GRAPHICS_TESTS "Build tests that require the Graphics subsystem. These require manual verification" ON)
option(BUILD_WITH_FREETYPE "Build with Freetype support" ON)
option(BUILD_WITH_ADDRESS_SANITISE "Build with compiler flags to profile memory leaks and other memory problems. This has a performance impact." OFF)
option(BUILD_WITH_EFSW "Build with EFSW to allow automatic resource loading by default. This isn't supported on all platforms." OFF)

include("${CMAKE_CURRENT_LIST_DIR}/dependencies/scripts/${ECHO_TARGET_PLATFORM}/echo3Options.cmake" OPTIONAL)

# This can be used to disable doctest discovering tests for ctest which can fail when cross compiling. It can be used
# to cross compile tests. Platform configs will disable this
set(ECHO_DOCTEST_DISCOVER ON)

### CMake stuff
set(DEPS_ROOT "$ENV{ECHO_ENGINE_INSTALL_DIR}/${ECHO_TARGET_PLATFORM}" CACHE PATH "Path to dependencies install root")
set(NO_DEFAULT_PATH ON)
set(CMAKE_FIND_ROOT_PATH "${DEPS_ROOT}/lib/cmake")

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

################################################################################
################ Project configuration
################################################################################

project(echo3)

### Find dependencies
find_package(Box2D CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(utf8cpp CONFIG REQUIRED)
find_package(websocketpp CONFIG REQUIRED)
find_package(CapnProto CONFIG REQUIRED)

file(GLOB_RECURSE capnpFiles CONFIGURE_DEPENDS
	include/echo/Schema/*.capnp
)

set(CAPNPC_SRC_PREFIX "")
# capnpSources capnpHeaders are outputs
capnp_generate_cpp(capnpSources capnpHeaders ${capnpFiles})

# Pkg-config searches vary depending on the platform. This is mainly to satisfy systems that pull in additional dependencies
# that might also include some that we would otherwise supply. The version of the system libraries might conflict with the
# versions we build against. Since our requirements (at the moment) are more flexible we'll link against the system versions.
include("${CMAKE_CURRENT_LIST_DIR}/dependencies/scripts/${ECHO_TARGET_PLATFORM}/echo3Dependencies.cmake")

add_library(echo3
		${capnpSources}
		src/Chrono/Chrono.cpp
		src/Chrono/CPUTimer.cpp
		src/Chrono/FrameRateLimiter.cpp
		src/FileSystem/File.cpp
		src/FileSystem/FileReferenceEncrypted.cpp
		src/FileSystem/FileReferenceFile.cpp
		src/FileSystem/FileReferenceMemory.cpp
		src/FileSystem/FileReferenceVFS.cpp
		src/FileSystem/FileSystem.cpp
		src/FileSystem/FileSystemSource.cpp
		src/FileSystem/FileSystemSourceEncrypted.cpp
		src/FileSystem/FileSystemSourceFile.cpp
		src/FileSystem/FileSystemSourceMemory.cpp
		src/FileSystem/FileSystemSourceVFS.cpp
		src/Graphics/Camera.cpp
		src/Graphics/CameraDollies.cpp
		src/Graphics/Colour.cpp
		src/Graphics/CubeMapTexture.cpp
		src/Graphics/ElementBuffer.cpp
		src/Graphics/Frustum.cpp
		src/Graphics/Light.cpp
		src/Graphics/MaterialAnimation.cpp
		src/Graphics/Material.cpp
		src/Graphics/Mesh.cpp
		src/Graphics/MultipassRenderable.cpp
		src/Graphics/MultiRenderer.cpp
		src/Graphics/Node.cpp
		src/Graphics/PrimitiveTypes.cpp
		src/Graphics/Renderable.cpp
		src/Graphics/Renderer.cpp
		src/Graphics/RenderPass.cpp
		src/Graphics/RenderTarget.cpp
		src/Graphics/RenderTargetNotifier.cpp
		src/Graphics/Scene.cpp
		src/Graphics/SceneEntity.cpp
		src/Graphics/SceneRenderable.cpp
		src/Graphics/Shader.cpp
		src/Graphics/ShaderProgram.cpp
		src/Graphics/SkyBox.cpp
		src/Graphics/Sprite.cpp
		src/Graphics/StereoscopicRenderer.cpp
		src/Graphics/SubMesh.cpp
		src/Graphics/Terrain.cpp
		src/Graphics/TextMesh.cpp
		src/Graphics/Texture.cpp
		src/Graphics/TextureUnit.cpp
		src/Graphics/VertexAttribute.cpp
		src/Graphics/VertexBuffer.cpp
		src/Graphics/Viewport.cpp
		src/Kernel/ExecutionModel.cpp
		src/Kernel/Kernel.cpp
		src/Kernel/Task.cpp
		src/Kernel/TaskGroup.cpp
		src/Kernel/TaskManager.cpp
		src/Kernel/Thread.cpp
		src/Logging/Logging.cpp
		src/Maths/AxisAlignedBox.cpp
		src/Maths/EchoMaths.cpp
		src/Maths/Geometry.cpp
		src/Maths/Matrix4.cpp
		src/Maths/Plane.cpp
		src/Maths/Quaternion.cpp
		src/Maths/Vector4.cpp
		src/Network/Connection.cpp
		src/Network/ConnectionDetails.cpp
		src/Network/DataPacket.cpp
		src/Network/DataPacketFactory.cpp
		src/Network/NetworkManager.cpp
		src/Network/NetworkManagerUpdater.cpp
		src/Network/NetworkSystem.cpp
		src/Network/SimpleDataPacketPool.cpp
		src/Resource/3dsReader.cpp
		src/Resource/BitmapLoader.cpp
		src/Resource/FontManager.cpp
		src/Resource/MaterialManager.cpp
		src/Resource/MeshManager.cpp
		src/Resource/MeshReader.cpp
		src/Resource/ResourceManager.cpp
		src/Resource/RIFFReader.cpp
		src/Resource/ShaderManager.cpp
		src/Resource/SkeletonManager.cpp
		src/Resource/SkeletonReader.cpp
		src/Resource/TextureLoader.cpp
		src/Resource/TextureManager.cpp
		src/Resource/WavAudioSource.cpp
		src/Shell/Shell.cpp
		src/Util/Configuration.cpp
		src/Util/ContextSwitcher.cpp
		src/Util/Parsers.cpp
		src/Util/RegEx.cpp
		src/Util/ScenePicker.cpp
		src/Util/StringSetter.cpp
		src/Util/StringUtils.cpp
		src/Util/Texture.cpp
)

if(BUILD_WITH_SOCKETS)
target_sources(echo3 PRIVATE
	src/Network/SocketNetworkSystem.cpp
	src/Network/TCPConnection.cpp
	src/Network/UDPConnection.cpp
	src/Platforms/Boost/BoostASIONetworkSystem.cpp
)

	if(BUILD_WITH_SSL)
		target_sources(echo3 PRIVATE
			src/Network/TLSConnection.cpp
		)
		target_compile_definitions(echo3
			PUBLIC
			ECHO_SSL_SUPPORT
		)
		endif()

target_compile_definitions(echo3
	PUBLIC
	ECHO_SOCKET_SUPPORT
)
endif()

if(BUILD_WITH_WEBSOCKETS)
target_sources(echo3 PRIVATE
	src/Network/WebSocketConnection.cpp
	src/Network/WebSocketNetworkSystem.cpp
)

target_compile_definitions(echo3
	PUBLIC
	ECHO_WEBSOCKET_SUPPORT

)
endif()

target_link_directories(echo3 PUBLIC ${DEPS_ROOT}/lib)
target_link_libraries(
		echo3
		PUBLIC
		websocketpp::websocketpp
		PkgConfig::bullet
		CapnProto::capnp
		CapnProto::capnp-json
)

if(BUILD_WITH_SSL)
	target_compile_definitions(echo3
		PUBLIC
		ECHO_SSL_SUPPORT
	)

	target_link_libraries(
			echo3
			PUBLIC
			PkgConfig::libtls
			PkgConfig::libssl
			PkgConfig::libcrypto
	)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	if(BUILD_WITH_ADDRESS_SANITISE)
		add_compile_options(-fsanitize=address)
		add_compile_options(-fno-omit-frame-pointer)
		add_link_options(-fsanitize=address)
	endif()
	add_compile_options(-fno-limit-debug-info)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/dependencies/scripts/${ECHO_TARGET_PLATFORM}/echo3.cmake")

if(BUILD_WITH_EFSW)
	find_package(efsw CONFIG REQUIRED)
	target_compile_definitions(echo3
		PUBLIC
		ECHO_EFSW_SUPPORT
	)
	target_link_libraries(
		echo3
		PUBLIC
		efsw::efsw
	)
endif()

if(BUILD_WITH_OPENGL)
	target_sources(echo3 PRIVATE
		src/Platforms/GL/GLRenderTarget.cpp
		src/Platforms/GL/GLShader.cpp
		src/Platforms/GL/GLShaderProgram.cpp
		src/Platforms/GL/GLTexture.cpp
		src/Platforms/GL/GLCubeMapTexture.cpp
		src/Platforms/GL/GLRenderTexture.cpp
		src/Platforms/GL/GLVertexBuffer.cpp
	)
	#target_link_libraries(echo3 PUBLIC PkgConfig::glew)
	target_link_libraries(echo3 PUBLIC GLEW::GLEW)
endif()

if(BUILD_WITH_FREETYPE)
	#The sources are unchanged, FontManager checks for this definition
	target_compile_definitions(echo3 PUBLIC ECHO_FREETYPE_SUPPORT_ENABLED)
else()
endif()

if(BUILD_WITH_JPEG)
	target_sources(echo3 PRIVATE src/Resource/JPEGLoader.cpp)
	target_compile_definitions(echo3 PUBLIC ECHO_JPEG_SUPPORT_ENABLED)
	target_link_libraries(echo3 PUBLIC PkgConfig::libjpeg)
endif()

if(BUILD_WITH_PNG)
	target_sources(echo3 PRIVATE src/Resource/PNGLoader.cpp)
	target_compile_definitions(echo3 PUBLIC ECHO_PNG_SUPPORT_ENABLED)
	target_link_libraries(echo3 PUBLIC PkgConfig::libpng)
endif()

target_include_directories(
		echo3
		PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

set(ECHO_QT_UI_FRAMEWORK FALSE)
set(ECHO_GTK_UI_FRAMEWORK FALSE)
set(ECHO_NATIVE_UI_FRAMEWORK FALSE)

if (ECHO_UI_FRAMEWORK STREQUAL "Null")
	target_sources(echo3
		PRIVATE
		src/Platforms/Null/NullDefaultInputManager.cpp
		src/Platforms/Null/NullDefaultRenderTarget.cpp
		src/Platforms/Null/SimpleDefaultExecutionModel.cpp
	)
elseif (ECHO_UI_FRAMEWORK STREQUAL "Qt")
	find_package(Qt5 CONFIG REQUIRED Core Gui OpenGL)
	file(GLOB_RECURSE echo_qt_srcs CONFIGURE_DEPENDS src/Platforms/Qt/*.cpp)
	target_sources(echo3
		PRIVATE
		${echo_qt_srcs}
	)
	target_link_libraries(echo3
		PRIVATE
		Qt5::OpenGL
		Qt5::Gui
		Qt5::Core
	)
	set(ECHO_QT_UI_FRAMEWORK TRUE)
elseif (ECHO_UI_FRAMEWORK STREQUAL "GTK")
	pkg_check_modules(gtkmm REQUIRED IMPORTED_TARGET gtkmm-2.4)
	pkg_check_modules(gtkglextmm REQUIRED IMPORTED_TARGET gtkglextmm-1.2)
	pkg_check_modules(x11 REQUIRED IMPORTED_TARGET x11)
	file(GLOB_RECURSE echo_gtk_srcs CONFIGURE_DEPENDS src/Platforms/GTK/*.cpp)
	target_sources(echo3
		PRIVATE
		${echo_gtk_srcs}
	)
	# Ubuntu 18.04 has a lot of packages incorrectly configured and hard code bad paths in pkg-config. cmake policy CMP0027 errors if
	# an include path doesn't exist. A work around is to not import the package using PkgConfig::XYZ and instead manually add the
	# compile and link flags. This is why ${gtkglextmm_INCLUDE_DIRS} and ${gtkglextmm_LIBRARIES} are used here.
	# Additionally, unfortunately to the way CMake applies the import rules we need to specify PRIVATE here and any applications using
	# GTK will have to do the same.
	target_include_directories(echo3
		PRIVATE
		${gtkglextmm_INCLUDE_DIRS}
	)

	target_link_libraries(echo3
		PRIVATE
		PkgConfig::gtkmm
		PkgConfig::x11
		${gtkglextmm_LIBRARIES}
	)
	set(ECHO_GTK_UI_FRAMEWORK TRUE)
endif()

set(ECHO_OPENAL_AUDIO_SYSTEM FALSE)
if (ECHO_AUDIO_SYSTEM STREQUAL "Null")
	target_sources(echo3
		PRIVATE
		src/Platforms/Null/NullAudio.cpp
	)
elseif (ECHO_AUDIO_SYSTEM STREQUAL "OpenAL")
	find_package(OpenAL CONFIG REQUIRED)
	file(GLOB_RECURSE echo_openal_srcs CONFIGURE_DEPENDS src/Platforms/AL/*.cpp)
	target_sources(echo3
		PRIVATE
		${echo_openal_srcs}
	)
	target_link_libraries(echo3
		PRIVATE
		OpenAL::OpenAL
	)
	set(ECHO_OPENAL_AUDIO_SYSTEM TRUE)
endif()

if (BUILD_TESTS OR BUILD_GRAPHICS_TESTS)
	find_package(doctest CONFIG REQUIRED)
	enable_testing()
endif()

if (BUILD_TESTS)
	add_executable(RunUnitTests src/Tests/RunUnitTests.cpp)
	target_link_libraries(RunUnitTests
		PRIVATE
			doctest::doctest
			echo3
			${gtkglextmm_LIBRARIES}
	)
	include(doctest)

	file(GLOB TestsUnit_srcs CONFIGURE_DEPENDS src/Tests/Unit/*.cpp)
	target_sources(RunUnitTests PRIVATE ${TestsUnit_srcs})

	if (ECHO_DOCTEST_DISCOVER)
		doctest_discover_tests(RunUnitTests)
	endif()

endif()

if (BUILD_GRAPHICS_TESTS)
	add_executable(RunGraphicTests src/Tests/RunGraphicTests.cpp)
	target_include_directories(RunGraphicTests
		PRIVATE
		${gtkglextmm_INCLUDE_DIRS}
	)
	target_link_libraries(RunGraphicTests
		PRIVATE
			doctest::doctest
			echo3
			${gtkglextmm_LIBRARIES}
	)
	include(doctest)

	file(GLOB TestsGraphic_srcs CONFIGURE_DEPENDS src/Tests/Graphic/*.cpp)
	target_sources(RunGraphicTests PRIVATE ${TestsGraphic_srcs})

	if (ECHO_DOCTEST_DISCOVER)
		doctest_discover_tests(RunGraphicTests)
	endif()
endif()

if (BUILD_TOOLS)
	add_executable(evfsc src/Tools/evfsc.cpp)
	target_link_libraries(
		evfsc
		PUBLIC
		echo3
	)

	add_executable(EchoLogServer src/Tools/LogServer.cpp)
	target_link_libraries(
		EchoLogServer
		PUBLIC
		echo3
	)
endif()

install(TARGETS echo3
		EXPORT echo3Targets
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin
		INCLUDES DESTINATION include
)
install(EXPORT echo3Targets
		FILE echo3Targets.cmake
		NAMESPACE echo3::
		DESTINATION lib/cmake/echo3
)
configure_file(cmake/${ECHO_TARGET_PLATFORM}/echo3Config.cmake.in echo3Config.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/echo3Config.cmake"
		DESTINATION lib/cmake/echo3
)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/scripts/echoConvertResource"
		DESTINATION bin
)
install(FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/dependencies/scripts/${ECHO_TARGET_PLATFORM}/${ECHO_TARGET_PLATFORM}.config"
		"${CMAKE_CURRENT_SOURCE_DIR}/dependencies/scripts/${ECHO_TARGET_PLATFORM}/${ECHO_TARGET_PLATFORM}-toolchain.cmake"
		OPTIONAL
		DESTINATION config
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/echo/Schema/ DESTINATION include/echo/Schema PATTERN "*.h")

if (BUILD_TOOLS)
	install(TARGETS evfsc
			RUNTIME DESTINATION bin
	)
	install(TARGETS EchoLogServer
			RUNTIME DESTINATION bin
	)
endif()
