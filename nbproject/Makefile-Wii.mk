#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=powerpc-eabi-gcc
CCC=powerpc-eabi-g++
CXX=powerpc-eabi-g++
FC=gfortran
AS=powerpc-eabi-as

# Macros
CND_PLATFORM=devkitPPC-Linux
CND_DLIB_EXT=so
CND_CONF=Wii
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/Animation/Animation.o \
	${OBJECTDIR}/src/Animation/AnimationState.o \
	${OBJECTDIR}/src/Animation/AnimationTrack.o \
	${OBJECTDIR}/src/Animation/BoneAnimationTrack.o \
	${OBJECTDIR}/src/Animation/Skeleton.o \
	${OBJECTDIR}/src/Animation/SkeletonAnimation.o \
	${OBJECTDIR}/src/Animation/SkeletonAnimationState.o \
	${OBJECTDIR}/src/Animation/SpriteAnimation.o \
	${OBJECTDIR}/src/Application.o \
	${OBJECTDIR}/src/Audio/Audio.o \
	${OBJECTDIR}/src/Audio/AudioBuffer.o \
	${OBJECTDIR}/src/Audio/AudioPlayer.o \
	${OBJECTDIR}/src/Audio/AudioSource.o \
	${OBJECTDIR}/src/Audio/AudioStream.o \
	${OBJECTDIR}/src/Chrono/CPUTimer.o \
	${OBJECTDIR}/src/Chrono/Chrono.o \
	${OBJECTDIR}/src/Chrono/FrameRateLimiter.o \
	${OBJECTDIR}/src/FileSystem/File.o \
	${OBJECTDIR}/src/FileSystem/FileReferenceEncrypted.o \
	${OBJECTDIR}/src/FileSystem/FileReferenceFile.o \
	${OBJECTDIR}/src/FileSystem/FileReferenceMemory.o \
	${OBJECTDIR}/src/FileSystem/FileReferenceVFS.o \
	${OBJECTDIR}/src/FileSystem/FileSystem.o \
	${OBJECTDIR}/src/FileSystem/FileSystemSource.o \
	${OBJECTDIR}/src/FileSystem/FileSystemSourceEncrypted.o \
	${OBJECTDIR}/src/FileSystem/FileSystemSourceFile.o \
	${OBJECTDIR}/src/FileSystem/FileSystemSourceMemory.o \
	${OBJECTDIR}/src/FileSystem/FileSystemSourceVFS.o \
	${OBJECTDIR}/src/GUI/Button.o \
	${OBJECTDIR}/src/GUI/Container.o \
	${OBJECTDIR}/src/GUI/Cursor.o \
	${OBJECTDIR}/src/GUI/Element.o \
	${OBJECTDIR}/src/GUI/GUI.o \
	${OBJECTDIR}/src/GUI/Image.o \
	${OBJECTDIR}/src/GUI/Layout.o \
	${OBJECTDIR}/src/GUI/Menu.o \
	${OBJECTDIR}/src/GUI/Screen.o \
	${OBJECTDIR}/src/GUI/Text.o \
	${OBJECTDIR}/src/GUI/TextBox.o \
	${OBJECTDIR}/src/Graphics/Camera.o \
	${OBJECTDIR}/src/Graphics/CameraDollies.o \
	${OBJECTDIR}/src/Graphics/Colour.o \
	${OBJECTDIR}/src/Graphics/ElementBuffer.o \
	${OBJECTDIR}/src/Graphics/Frustum.o \
	${OBJECTDIR}/src/Graphics/Light.o \
	${OBJECTDIR}/src/Graphics/Material.o \
	${OBJECTDIR}/src/Graphics/MaterialAnimation.o \
	${OBJECTDIR}/src/Graphics/Mesh.o \
	${OBJECTDIR}/src/Graphics/MultiRenderer.o \
	${OBJECTDIR}/src/Graphics/MultipassRenderable.o \
	${OBJECTDIR}/src/Graphics/Node.o \
	${OBJECTDIR}/src/Graphics/PrimitiveTypes.o \
	${OBJECTDIR}/src/Graphics/RenderPass.o \
	${OBJECTDIR}/src/Graphics/RenderTarget.o \
	${OBJECTDIR}/src/Graphics/RenderTargetNotifier.o \
	${OBJECTDIR}/src/Graphics/Renderable.o \
	${OBJECTDIR}/src/Graphics/Renderer.o \
	${OBJECTDIR}/src/Graphics/Scene.o \
	${OBJECTDIR}/src/Graphics/SceneEntity.o \
	${OBJECTDIR}/src/Graphics/SceneRenderable.o \
	${OBJECTDIR}/src/Graphics/Shader.o \
	${OBJECTDIR}/src/Graphics/ShaderProgram.o \
	${OBJECTDIR}/src/Graphics/SkyBox.o \
	${OBJECTDIR}/src/Graphics/Sprite.o \
	${OBJECTDIR}/src/Graphics/StereoscopicRenderer.o \
	${OBJECTDIR}/src/Graphics/SubMesh.o \
	${OBJECTDIR}/src/Graphics/Terrain.o \
	${OBJECTDIR}/src/Graphics/TextMesh.o \
	${OBJECTDIR}/src/Graphics/Texture.o \
	${OBJECTDIR}/src/Graphics/TextureUnit.o \
	${OBJECTDIR}/src/Graphics/VertexAttribute.o \
	${OBJECTDIR}/src/Graphics/VertexBuffer.o \
	${OBJECTDIR}/src/Graphics/Viewport.o \
	${OBJECTDIR}/src/Input/GenericInputDevice.o \
	${OBJECTDIR}/src/Input/Input.o \
	${OBJECTDIR}/src/Input/InputDevice.o \
	${OBJECTDIR}/src/Input/MappedInputDevice.o \
	${OBJECTDIR}/src/Input/MappedInputDeviceLoader.o \
	${OBJECTDIR}/src/Input/OnScreenAnalogueStick.o \
	${OBJECTDIR}/src/Input/PseudoKeyboard.o \
	${OBJECTDIR}/src/Input/PseudoMouse.o \
	${OBJECTDIR}/src/Kernel/ExecutionModel.o \
	${OBJECTDIR}/src/Kernel/Kernel.o \
	${OBJECTDIR}/src/Kernel/Task.o \
	${OBJECTDIR}/src/Kernel/TaskGroup.o \
	${OBJECTDIR}/src/Kernel/TaskManager.o \
	${OBJECTDIR}/src/Kernel/Thread.o \
	${OBJECTDIR}/src/Logging/Logging.o \
	${OBJECTDIR}/src/Maths/AxisAlignedBox.o \
	${OBJECTDIR}/src/Maths/EchoMaths.o \
	${OBJECTDIR}/src/Maths/Geometry.o \
	${OBJECTDIR}/src/Maths/Matrix4.o \
	${OBJECTDIR}/src/Maths/Plane.o \
	${OBJECTDIR}/src/Maths/Quaternion.o \
	${OBJECTDIR}/src/Maths/Vector4.o \
	${OBJECTDIR}/src/Network/Connection.o \
	${OBJECTDIR}/src/Network/ConnectionDetails.o \
	${OBJECTDIR}/src/Network/DataPacket.o \
	${OBJECTDIR}/src/Network/DataPacketFactory.o \
	${OBJECTDIR}/src/Network/NetworkManager.o \
	${OBJECTDIR}/src/Network/NetworkManagerUpdater.o \
	${OBJECTDIR}/src/Network/NetworkSystem.o \
	${OBJECTDIR}/src/Network/SimpleDataPacketPool.o \
	${OBJECTDIR}/src/Network/SocketNetworkSystem.o \
	${OBJECTDIR}/src/Network/TCPConnection.o \
	${OBJECTDIR}/src/Network/TLSConnection.o \
	${OBJECTDIR}/src/Network/UDPConnection.o \
	${OBJECTDIR}/src/Network/WebSocketConnection.o \
	${OBJECTDIR}/src/Network/WebSocketNetworkSystem.o \
	${OBJECTDIR}/src/Physics/BulletDynamicMotionState.o \
	${OBJECTDIR}/src/Physics/BulletFactory.o \
	${OBJECTDIR}/src/Physics/BulletKinematicMotionState.o \
	${OBJECTDIR}/src/Physics/BulletMeshDebugDrawer.o \
	${OBJECTDIR}/src/Physics/BulletMeshPhysicsShape.o \
	${OBJECTDIR}/src/Physics/BulletMotionState.o \
	${OBJECTDIR}/src/Physics/BulletPhysicsShape.o \
	${OBJECTDIR}/src/Physics/BulletPhysicsWorld.o \
	${OBJECTDIR}/src/Physics/BulletRigidBody.o \
	${OBJECTDIR}/src/Physics/MotionState.o \
	${OBJECTDIR}/src/Physics/PhysicsBody.o \
	${OBJECTDIR}/src/Physics/PhysicsShape.o \
	${OBJECTDIR}/src/Physics/PhysicsWorld.o \
	${OBJECTDIR}/src/Platforms/GL/FragmentShaderManager.o \
	${OBJECTDIR}/src/Platforms/GL/VertexShaderManager.o \
	${OBJECTDIR}/src/Platforms/GTK/Platform.o \
	${OBJECTDIR}/src/Platforms/Qt/Platform.o \
	${OBJECTDIR}/src/Platforms/Qt/QtExecutionModel.o \
	${OBJECTDIR}/src/Platforms/Qt/QtKeyboard.o \
	${OBJECTDIR}/src/Platforms/Qt/QtMouse.o \
	${OBJECTDIR}/src/Platforms/Qt/QtWindow.o \
	${OBJECTDIR}/src/Platforms/Wii/CPUTimer.o \
	${OBJECTDIR}/src/Platforms/Wii/GXRenderTarget.o \
	${OBJECTDIR}/src/Platforms/Wii/GXTexture.o \
	${OBJECTDIR}/src/Platforms/Wii/LWPMutex.o \
	${OBJECTDIR}/src/Platforms/Wii/LWPThread.o \
	${OBJECTDIR}/src/Platforms/Wii/Platform.o \
	${OBJECTDIR}/src/Platforms/Wii/SDFileSystemSource.o \
	${OBJECTDIR}/src/Platforms/Wii/SystemInformation.o \
	${OBJECTDIR}/src/Platforms/Wii/USBFileSystemSource.o \
	${OBJECTDIR}/src/Platforms/Wii/WiiSystemInput.o \
	${OBJECTDIR}/src/Platforms/Wii/Wiimote.o \
	${OBJECTDIR}/src/Resource/3dsReader.o \
	${OBJECTDIR}/src/Resource/BitmapLoader.o \
	${OBJECTDIR}/src/Resource/FontManager.o \
	${OBJECTDIR}/src/Resource/JPEGLoader.o \
	${OBJECTDIR}/src/Resource/MaterialManager.o \
	${OBJECTDIR}/src/Resource/MeshManager.o \
	${OBJECTDIR}/src/Resource/MeshReader.o \
	${OBJECTDIR}/src/Resource/OggAudioSource.o \
	${OBJECTDIR}/src/Resource/PNGLoader.o \
	${OBJECTDIR}/src/Resource/RIFFReader.o \
	${OBJECTDIR}/src/Resource/ResourceManager.o \
	${OBJECTDIR}/src/Resource/SkeletonManager.o \
	${OBJECTDIR}/src/Resource/SkeletonReader.o \
	${OBJECTDIR}/src/Resource/TextureLoader.o \
	${OBJECTDIR}/src/Resource/TextureManager.o \
	${OBJECTDIR}/src/Resource/WavAudioSource.o \
	${OBJECTDIR}/src/Resource/XMAudioSource.o \
	${OBJECTDIR}/src/Shell/Shell.o \
	${OBJECTDIR}/src/Tile/TMXLoader.o \
	${OBJECTDIR}/src/Tile/TileLayer.o \
	${OBJECTDIR}/src/Tile/TileLayerMesh.o \
	${OBJECTDIR}/src/Tile/TileMap.o \
	${OBJECTDIR}/src/Tile/TileSet.o \
	${OBJECTDIR}/src/Util/Configuration.o \
	${OBJECTDIR}/src/Util/ContextSwitcher.o \
	${OBJECTDIR}/src/Util/Parsers.o \
	${OBJECTDIR}/src/Util/RegEx.o \
	${OBJECTDIR}/src/Util/ScenePicker.o \
	${OBJECTDIR}/src/Util/StringSetter.o \
	${OBJECTDIR}/src/Util/StringUtils.o \
	${OBJECTDIR}/src/Util/Texture.o \
	${OBJECTDIR}/src/generated/Font.pb.o \
	${OBJECTDIR}/src/generated/GUI.pb.o \
	${OBJECTDIR}/src/generated/MappedInputDevice.pb.o \
	${OBJECTDIR}/src/generated/Material.pb.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f24 \
	${TESTDIR}/TestFiles/f7 \
	${TESTDIR}/TestFiles/f6 \
	${TESTDIR}/TestFiles/f31 \
	${TESTDIR}/TestFiles/f12 \
	${TESTDIR}/TestFiles/f33 \
	${TESTDIR}/TestFiles/f29 \
	${TESTDIR}/TestFiles/f8 \
	${TESTDIR}/TestFiles/f3 \
	${TESTDIR}/TestFiles/f20 \
	${TESTDIR}/TestFiles/f5 \
	${TESTDIR}/TestFiles/f15 \
	${TESTDIR}/TestFiles/f27 \
	${TESTDIR}/TestFiles/f18 \
	${TESTDIR}/TestFiles/f22 \
	${TESTDIR}/TestFiles/f11 \
	${TESTDIR}/TestFiles/f34 \
	${TESTDIR}/TestFiles/f32 \
	${TESTDIR}/TestFiles/f35 \
	${TESTDIR}/TestFiles/f2 \
	${TESTDIR}/TestFiles/f21 \
	${TESTDIR}/TestFiles/f25 \
	${TESTDIR}/TestFiles/f19 \
	${TESTDIR}/TestFiles/f17 \
	${TESTDIR}/TestFiles/f14 \
	${TESTDIR}/TestFiles/f30 \
	${TESTDIR}/TestFiles/f28 \
	${TESTDIR}/TestFiles/f1 \
	${TESTDIR}/TestFiles/f13 \
	${TESTDIR}/TestFiles/f10 \
	${TESTDIR}/TestFiles/f36 \
	${TESTDIR}/TestFiles/f23 \
	${TESTDIR}/TestFiles/f16 \
	${TESTDIR}/TestFiles/f4 \
	${TESTDIR}/TestFiles/f9 \
	${TESTDIR}/TestFiles/f26

# Test Object Files
TESTOBJECTFILES= \
	${TESTDIR}/src/tests/AlphaRejectTest.o \
	${TESTDIR}/src/tests/ApplicationTest.o \
	${TESTDIR}/src/tests/AudioTests.o \
	${TESTDIR}/src/tests/BulletPhysicsTest.o \
	${TESTDIR}/src/tests/ConfigurationTest.o \
	${TESTDIR}/src/tests/ContextSwitcherTests.o \
	${TESTDIR}/src/tests/ConverterTests.o \
	${TESTDIR}/src/tests/FontMapTest.o \
	${TESTDIR}/src/tests/FunctionBinderTests.o \
	${TESTDIR}/src/tests/GUITest.o \
	${TESTDIR}/src/tests/InputTest.o \
	${TESTDIR}/src/tests/MaterialAnimationTest.o \
	${TESTDIR}/src/tests/MenuTest.o \
	${TESTDIR}/src/tests/MultiPassRenderTest.o \
	${TESTDIR}/src/tests/MultiStageTexturing.o \
	${TESTDIR}/src/tests/NetworkTests.o \
	${TESTDIR}/src/tests/PBRTests.o \
	${TESTDIR}/src/tests/ParserTests.o \
	${TESTDIR}/src/tests/ParticleTest.o \
	${TESTDIR}/src/tests/RenderTest.o \
	${TESTDIR}/src/tests/ResourceTests.o \
	${TESTDIR}/src/tests/ScissorTest.o \
	${TESTDIR}/src/tests/ShaderTest.o \
	${TESTDIR}/src/tests/ShellTest.o \
	${TESTDIR}/src/tests/SkyBoxTest.o \
	${TESTDIR}/src/tests/StringUtilsTest.o \
	${TESTDIR}/src/tests/SystemInformationTest.o \
	${TESTDIR}/src/tests/TaskTests.o \
	${TESTDIR}/src/tests/TerrainTest.o \
	${TESTDIR}/src/tests/TextRenderTest.o \
	${TESTDIR}/src/tests/TextureTest.o \
	${TESTDIR}/src/tests/TileMapTests.o \
	${TESTDIR}/src/tests/TimerTests.o \
	${TESTDIR}/src/tests/TypeTests.o \
	${TESTDIR}/src/tests/UTFStringTest.o \
	${TESTDIR}/src/tests/VertexBufferTests.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-mrvl -mcpu=750 -meabi -mhard-float
CXXFLAGS=-mrvl -mcpu=750 -meabi -mhard-float

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libecho3.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libecho3.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libecho3.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libecho3.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libecho3.a

${OBJECTDIR}/src/Animation/Animation.o: src/Animation/Animation.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Animation
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Animation/Animation.o src/Animation/Animation.cpp

${OBJECTDIR}/src/Animation/AnimationState.o: src/Animation/AnimationState.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Animation
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Animation/AnimationState.o src/Animation/AnimationState.cpp

${OBJECTDIR}/src/Animation/AnimationTrack.o: src/Animation/AnimationTrack.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Animation
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Animation/AnimationTrack.o src/Animation/AnimationTrack.cpp

${OBJECTDIR}/src/Animation/BoneAnimationTrack.o: src/Animation/BoneAnimationTrack.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Animation
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Animation/BoneAnimationTrack.o src/Animation/BoneAnimationTrack.cpp

${OBJECTDIR}/src/Animation/Skeleton.o: src/Animation/Skeleton.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Animation
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Animation/Skeleton.o src/Animation/Skeleton.cpp

${OBJECTDIR}/src/Animation/SkeletonAnimation.o: src/Animation/SkeletonAnimation.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Animation
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Animation/SkeletonAnimation.o src/Animation/SkeletonAnimation.cpp

${OBJECTDIR}/src/Animation/SkeletonAnimationState.o: src/Animation/SkeletonAnimationState.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Animation
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Animation/SkeletonAnimationState.o src/Animation/SkeletonAnimationState.cpp

${OBJECTDIR}/src/Animation/SpriteAnimation.o: src/Animation/SpriteAnimation.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Animation
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Animation/SpriteAnimation.o src/Animation/SpriteAnimation.cpp

${OBJECTDIR}/src/Application.o: src/Application.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Application.o src/Application.cpp

${OBJECTDIR}/src/Audio/Audio.o: src/Audio/Audio.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Audio
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Audio/Audio.o src/Audio/Audio.cpp

${OBJECTDIR}/src/Audio/AudioBuffer.o: src/Audio/AudioBuffer.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Audio
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Audio/AudioBuffer.o src/Audio/AudioBuffer.cpp

${OBJECTDIR}/src/Audio/AudioPlayer.o: src/Audio/AudioPlayer.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Audio
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Audio/AudioPlayer.o src/Audio/AudioPlayer.cpp

${OBJECTDIR}/src/Audio/AudioSource.o: src/Audio/AudioSource.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Audio
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Audio/AudioSource.o src/Audio/AudioSource.cpp

${OBJECTDIR}/src/Audio/AudioStream.o: src/Audio/AudioStream.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Audio
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Audio/AudioStream.o src/Audio/AudioStream.cpp

${OBJECTDIR}/src/Chrono/CPUTimer.o: src/Chrono/CPUTimer.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Chrono
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Chrono/CPUTimer.o src/Chrono/CPUTimer.cpp

${OBJECTDIR}/src/Chrono/Chrono.o: src/Chrono/Chrono.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Chrono
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Chrono/Chrono.o src/Chrono/Chrono.cpp

${OBJECTDIR}/src/Chrono/FrameRateLimiter.o: src/Chrono/FrameRateLimiter.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Chrono
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Chrono/FrameRateLimiter.o src/Chrono/FrameRateLimiter.cpp

${OBJECTDIR}/src/FileSystem/File.o: src/FileSystem/File.cpp
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/File.o src/FileSystem/File.cpp

${OBJECTDIR}/src/FileSystem/FileReferenceEncrypted.o: src/FileSystem/FileReferenceEncrypted.cpp
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileReferenceEncrypted.o src/FileSystem/FileReferenceEncrypted.cpp

${OBJECTDIR}/src/FileSystem/FileReferenceFile.o: src/FileSystem/FileReferenceFile.cpp
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileReferenceFile.o src/FileSystem/FileReferenceFile.cpp

${OBJECTDIR}/src/FileSystem/FileReferenceMemory.o: src/FileSystem/FileReferenceMemory.cpp
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileReferenceMemory.o src/FileSystem/FileReferenceMemory.cpp

${OBJECTDIR}/src/FileSystem/FileReferenceVFS.o: src/FileSystem/FileReferenceVFS.cpp
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileReferenceVFS.o src/FileSystem/FileReferenceVFS.cpp

${OBJECTDIR}/src/FileSystem/FileSystem.o: src/FileSystem/FileSystem.cpp
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileSystem.o src/FileSystem/FileSystem.cpp

${OBJECTDIR}/src/FileSystem/FileSystemSource.o: src/FileSystem/FileSystemSource.cpp
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileSystemSource.o src/FileSystem/FileSystemSource.cpp

${OBJECTDIR}/src/FileSystem/FileSystemSourceEncrypted.o: src/FileSystem/FileSystemSourceEncrypted.cpp
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileSystemSourceEncrypted.o src/FileSystem/FileSystemSourceEncrypted.cpp

${OBJECTDIR}/src/FileSystem/FileSystemSourceFile.o: src/FileSystem/FileSystemSourceFile.cpp
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileSystemSourceFile.o src/FileSystem/FileSystemSourceFile.cpp

${OBJECTDIR}/src/FileSystem/FileSystemSourceMemory.o: src/FileSystem/FileSystemSourceMemory.cpp
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileSystemSourceMemory.o src/FileSystem/FileSystemSourceMemory.cpp

${OBJECTDIR}/src/FileSystem/FileSystemSourceVFS.o: src/FileSystem/FileSystemSourceVFS.cpp
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileSystemSourceVFS.o src/FileSystem/FileSystemSourceVFS.cpp

${OBJECTDIR}/src/GUI/Button.o: src/GUI/Button.cpp
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Button.o src/GUI/Button.cpp

${OBJECTDIR}/src/GUI/Container.o: src/GUI/Container.cpp
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Container.o src/GUI/Container.cpp

${OBJECTDIR}/src/GUI/Cursor.o: src/GUI/Cursor.cpp
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Cursor.o src/GUI/Cursor.cpp

${OBJECTDIR}/src/GUI/Element.o: src/GUI/Element.cpp
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Element.o src/GUI/Element.cpp

${OBJECTDIR}/src/GUI/GUI.o: src/GUI/GUI.cpp
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/GUI.o src/GUI/GUI.cpp

${OBJECTDIR}/src/GUI/Image.o: src/GUI/Image.cpp
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Image.o src/GUI/Image.cpp

${OBJECTDIR}/src/GUI/Layout.o: src/GUI/Layout.cpp
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Layout.o src/GUI/Layout.cpp

${OBJECTDIR}/src/GUI/Menu.o: src/GUI/Menu.cpp
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Menu.o src/GUI/Menu.cpp

${OBJECTDIR}/src/GUI/Screen.o: src/GUI/Screen.cpp
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Screen.o src/GUI/Screen.cpp

${OBJECTDIR}/src/GUI/Text.o: src/GUI/Text.cpp
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Text.o src/GUI/Text.cpp

${OBJECTDIR}/src/GUI/TextBox.o: src/GUI/TextBox.cpp
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/TextBox.o src/GUI/TextBox.cpp

${OBJECTDIR}/src/Graphics/Camera.o: src/Graphics/Camera.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Camera.o src/Graphics/Camera.cpp

${OBJECTDIR}/src/Graphics/CameraDollies.o: src/Graphics/CameraDollies.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/CameraDollies.o src/Graphics/CameraDollies.cpp

${OBJECTDIR}/src/Graphics/Colour.o: src/Graphics/Colour.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Colour.o src/Graphics/Colour.cpp

${OBJECTDIR}/src/Graphics/ElementBuffer.o: src/Graphics/ElementBuffer.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/ElementBuffer.o src/Graphics/ElementBuffer.cpp

${OBJECTDIR}/src/Graphics/Frustum.o: src/Graphics/Frustum.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Frustum.o src/Graphics/Frustum.cpp

${OBJECTDIR}/src/Graphics/Light.o: src/Graphics/Light.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Light.o src/Graphics/Light.cpp

${OBJECTDIR}/src/Graphics/Material.o: src/Graphics/Material.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Material.o src/Graphics/Material.cpp

${OBJECTDIR}/src/Graphics/MaterialAnimation.o: src/Graphics/MaterialAnimation.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/MaterialAnimation.o src/Graphics/MaterialAnimation.cpp

${OBJECTDIR}/src/Graphics/Mesh.o: src/Graphics/Mesh.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Mesh.o src/Graphics/Mesh.cpp

${OBJECTDIR}/src/Graphics/MultiRenderer.o: src/Graphics/MultiRenderer.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/MultiRenderer.o src/Graphics/MultiRenderer.cpp

${OBJECTDIR}/src/Graphics/MultipassRenderable.o: src/Graphics/MultipassRenderable.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/MultipassRenderable.o src/Graphics/MultipassRenderable.cpp

${OBJECTDIR}/src/Graphics/Node.o: src/Graphics/Node.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Node.o src/Graphics/Node.cpp

${OBJECTDIR}/src/Graphics/PrimitiveTypes.o: src/Graphics/PrimitiveTypes.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/PrimitiveTypes.o src/Graphics/PrimitiveTypes.cpp

${OBJECTDIR}/src/Graphics/RenderPass.o: src/Graphics/RenderPass.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/RenderPass.o src/Graphics/RenderPass.cpp

${OBJECTDIR}/src/Graphics/RenderTarget.o: src/Graphics/RenderTarget.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/RenderTarget.o src/Graphics/RenderTarget.cpp

${OBJECTDIR}/src/Graphics/RenderTargetNotifier.o: src/Graphics/RenderTargetNotifier.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/RenderTargetNotifier.o src/Graphics/RenderTargetNotifier.cpp

${OBJECTDIR}/src/Graphics/Renderable.o: src/Graphics/Renderable.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Renderable.o src/Graphics/Renderable.cpp

${OBJECTDIR}/src/Graphics/Renderer.o: src/Graphics/Renderer.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Renderer.o src/Graphics/Renderer.cpp

${OBJECTDIR}/src/Graphics/Scene.o: src/Graphics/Scene.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Scene.o src/Graphics/Scene.cpp

${OBJECTDIR}/src/Graphics/SceneEntity.o: src/Graphics/SceneEntity.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/SceneEntity.o src/Graphics/SceneEntity.cpp

${OBJECTDIR}/src/Graphics/SceneRenderable.o: src/Graphics/SceneRenderable.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/SceneRenderable.o src/Graphics/SceneRenderable.cpp

${OBJECTDIR}/src/Graphics/Shader.o: src/Graphics/Shader.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Shader.o src/Graphics/Shader.cpp

${OBJECTDIR}/src/Graphics/ShaderProgram.o: src/Graphics/ShaderProgram.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/ShaderProgram.o src/Graphics/ShaderProgram.cpp

${OBJECTDIR}/src/Graphics/SkyBox.o: src/Graphics/SkyBox.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/SkyBox.o src/Graphics/SkyBox.cpp

${OBJECTDIR}/src/Graphics/Sprite.o: src/Graphics/Sprite.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Sprite.o src/Graphics/Sprite.cpp

${OBJECTDIR}/src/Graphics/StereoscopicRenderer.o: src/Graphics/StereoscopicRenderer.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/StereoscopicRenderer.o src/Graphics/StereoscopicRenderer.cpp

${OBJECTDIR}/src/Graphics/SubMesh.o: src/Graphics/SubMesh.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/SubMesh.o src/Graphics/SubMesh.cpp

${OBJECTDIR}/src/Graphics/Terrain.o: src/Graphics/Terrain.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Terrain.o src/Graphics/Terrain.cpp

${OBJECTDIR}/src/Graphics/TextMesh.o: src/Graphics/TextMesh.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/TextMesh.o src/Graphics/TextMesh.cpp

${OBJECTDIR}/src/Graphics/Texture.o: src/Graphics/Texture.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Texture.o src/Graphics/Texture.cpp

${OBJECTDIR}/src/Graphics/TextureUnit.o: src/Graphics/TextureUnit.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/TextureUnit.o src/Graphics/TextureUnit.cpp

${OBJECTDIR}/src/Graphics/VertexAttribute.o: src/Graphics/VertexAttribute.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/VertexAttribute.o src/Graphics/VertexAttribute.cpp

${OBJECTDIR}/src/Graphics/VertexBuffer.o: src/Graphics/VertexBuffer.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/VertexBuffer.o src/Graphics/VertexBuffer.cpp

${OBJECTDIR}/src/Graphics/Viewport.o: src/Graphics/Viewport.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Viewport.o src/Graphics/Viewport.cpp

${OBJECTDIR}/src/Input/GenericInputDevice.o: src/Input/GenericInputDevice.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Input
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/GenericInputDevice.o src/Input/GenericInputDevice.cpp

${OBJECTDIR}/src/Input/Input.o: src/Input/Input.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Input
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/Input.o src/Input/Input.cpp

${OBJECTDIR}/src/Input/InputDevice.o: src/Input/InputDevice.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Input
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/InputDevice.o src/Input/InputDevice.cpp

${OBJECTDIR}/src/Input/MappedInputDevice.o: src/Input/MappedInputDevice.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Input
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/MappedInputDevice.o src/Input/MappedInputDevice.cpp

${OBJECTDIR}/src/Input/MappedInputDeviceLoader.o: src/Input/MappedInputDeviceLoader.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Input
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/MappedInputDeviceLoader.o src/Input/MappedInputDeviceLoader.cpp

${OBJECTDIR}/src/Input/OnScreenAnalogueStick.o: src/Input/OnScreenAnalogueStick.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Input
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/OnScreenAnalogueStick.o src/Input/OnScreenAnalogueStick.cpp

${OBJECTDIR}/src/Input/PseudoKeyboard.o: src/Input/PseudoKeyboard.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Input
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/PseudoKeyboard.o src/Input/PseudoKeyboard.cpp

${OBJECTDIR}/src/Input/PseudoMouse.o: src/Input/PseudoMouse.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Input
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/PseudoMouse.o src/Input/PseudoMouse.cpp

${OBJECTDIR}/src/Kernel/ExecutionModel.o: src/Kernel/ExecutionModel.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Kernel
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Kernel/ExecutionModel.o src/Kernel/ExecutionModel.cpp

${OBJECTDIR}/src/Kernel/Kernel.o: src/Kernel/Kernel.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Kernel
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Kernel/Kernel.o src/Kernel/Kernel.cpp

${OBJECTDIR}/src/Kernel/Task.o: src/Kernel/Task.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Kernel
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Kernel/Task.o src/Kernel/Task.cpp

${OBJECTDIR}/src/Kernel/TaskGroup.o: src/Kernel/TaskGroup.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Kernel
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Kernel/TaskGroup.o src/Kernel/TaskGroup.cpp

${OBJECTDIR}/src/Kernel/TaskManager.o: src/Kernel/TaskManager.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Kernel
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Kernel/TaskManager.o src/Kernel/TaskManager.cpp

${OBJECTDIR}/src/Kernel/Thread.o: src/Kernel/Thread.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Kernel
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Kernel/Thread.o src/Kernel/Thread.cpp

${OBJECTDIR}/src/Logging/Logging.o: src/Logging/Logging.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Logging
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Logging/Logging.o src/Logging/Logging.cpp

${OBJECTDIR}/src/Maths/AxisAlignedBox.o: src/Maths/AxisAlignedBox.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Maths
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Maths/AxisAlignedBox.o src/Maths/AxisAlignedBox.cpp

${OBJECTDIR}/src/Maths/EchoMaths.o: src/Maths/EchoMaths.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Maths
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Maths/EchoMaths.o src/Maths/EchoMaths.cpp

${OBJECTDIR}/src/Maths/Geometry.o: src/Maths/Geometry.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Maths
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Maths/Geometry.o src/Maths/Geometry.cpp

${OBJECTDIR}/src/Maths/Matrix4.o: src/Maths/Matrix4.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Maths
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Maths/Matrix4.o src/Maths/Matrix4.cpp

${OBJECTDIR}/src/Maths/Plane.o: src/Maths/Plane.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Maths
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Maths/Plane.o src/Maths/Plane.cpp

${OBJECTDIR}/src/Maths/Quaternion.o: src/Maths/Quaternion.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Maths
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Maths/Quaternion.o src/Maths/Quaternion.cpp

${OBJECTDIR}/src/Maths/Vector4.o: src/Maths/Vector4.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Maths
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Maths/Vector4.o src/Maths/Vector4.cpp

${OBJECTDIR}/src/Network/Connection.o: src/Network/Connection.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Network
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/Connection.o src/Network/Connection.cpp

${OBJECTDIR}/src/Network/ConnectionDetails.o: src/Network/ConnectionDetails.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Network
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/ConnectionDetails.o src/Network/ConnectionDetails.cpp

${OBJECTDIR}/src/Network/DataPacket.o: src/Network/DataPacket.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Network
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/DataPacket.o src/Network/DataPacket.cpp

${OBJECTDIR}/src/Network/DataPacketFactory.o: src/Network/DataPacketFactory.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Network
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/DataPacketFactory.o src/Network/DataPacketFactory.cpp

${OBJECTDIR}/src/Network/NetworkManager.o: src/Network/NetworkManager.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Network
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/NetworkManager.o src/Network/NetworkManager.cpp

${OBJECTDIR}/src/Network/NetworkManagerUpdater.o: src/Network/NetworkManagerUpdater.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Network
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/NetworkManagerUpdater.o src/Network/NetworkManagerUpdater.cpp

${OBJECTDIR}/src/Network/NetworkSystem.o: src/Network/NetworkSystem.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Network
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/NetworkSystem.o src/Network/NetworkSystem.cpp

${OBJECTDIR}/src/Network/SimpleDataPacketPool.o: src/Network/SimpleDataPacketPool.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Network
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/SimpleDataPacketPool.o src/Network/SimpleDataPacketPool.cpp

${OBJECTDIR}/src/Network/SocketNetworkSystem.o: src/Network/SocketNetworkSystem.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Network
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/SocketNetworkSystem.o src/Network/SocketNetworkSystem.cpp

${OBJECTDIR}/src/Network/TCPConnection.o: src/Network/TCPConnection.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Network
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/TCPConnection.o src/Network/TCPConnection.cpp

${OBJECTDIR}/src/Network/TLSConnection.o: src/Network/TLSConnection.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Network
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/TLSConnection.o src/Network/TLSConnection.cpp

${OBJECTDIR}/src/Network/UDPConnection.o: src/Network/UDPConnection.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Network
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/UDPConnection.o src/Network/UDPConnection.cpp

${OBJECTDIR}/src/Network/WebSocketConnection.o: src/Network/WebSocketConnection.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Network
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/WebSocketConnection.o src/Network/WebSocketConnection.cpp

${OBJECTDIR}/src/Network/WebSocketNetworkSystem.o: src/Network/WebSocketNetworkSystem.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Network
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/WebSocketNetworkSystem.o src/Network/WebSocketNetworkSystem.cpp

${OBJECTDIR}/src/Physics/BulletDynamicMotionState.o: src/Physics/BulletDynamicMotionState.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletDynamicMotionState.o src/Physics/BulletDynamicMotionState.cpp

${OBJECTDIR}/src/Physics/BulletFactory.o: src/Physics/BulletFactory.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletFactory.o src/Physics/BulletFactory.cpp

${OBJECTDIR}/src/Physics/BulletKinematicMotionState.o: src/Physics/BulletKinematicMotionState.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletKinematicMotionState.o src/Physics/BulletKinematicMotionState.cpp

${OBJECTDIR}/src/Physics/BulletMeshDebugDrawer.o: src/Physics/BulletMeshDebugDrawer.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletMeshDebugDrawer.o src/Physics/BulletMeshDebugDrawer.cpp

${OBJECTDIR}/src/Physics/BulletMeshPhysicsShape.o: src/Physics/BulletMeshPhysicsShape.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletMeshPhysicsShape.o src/Physics/BulletMeshPhysicsShape.cpp

${OBJECTDIR}/src/Physics/BulletMotionState.o: src/Physics/BulletMotionState.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletMotionState.o src/Physics/BulletMotionState.cpp

${OBJECTDIR}/src/Physics/BulletPhysicsShape.o: src/Physics/BulletPhysicsShape.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletPhysicsShape.o src/Physics/BulletPhysicsShape.cpp

${OBJECTDIR}/src/Physics/BulletPhysicsWorld.o: src/Physics/BulletPhysicsWorld.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletPhysicsWorld.o src/Physics/BulletPhysicsWorld.cpp

${OBJECTDIR}/src/Physics/BulletRigidBody.o: src/Physics/BulletRigidBody.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletRigidBody.o src/Physics/BulletRigidBody.cpp

${OBJECTDIR}/src/Physics/MotionState.o: src/Physics/MotionState.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/MotionState.o src/Physics/MotionState.cpp

${OBJECTDIR}/src/Physics/PhysicsBody.o: src/Physics/PhysicsBody.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/PhysicsBody.o src/Physics/PhysicsBody.cpp

${OBJECTDIR}/src/Physics/PhysicsShape.o: src/Physics/PhysicsShape.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/PhysicsShape.o src/Physics/PhysicsShape.cpp

${OBJECTDIR}/src/Physics/PhysicsWorld.o: src/Physics/PhysicsWorld.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/PhysicsWorld.o src/Physics/PhysicsWorld.cpp

${OBJECTDIR}/src/Platforms/GL/FragmentShaderManager.o: src/Platforms/GL/FragmentShaderManager.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/GL
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/GL/FragmentShaderManager.o src/Platforms/GL/FragmentShaderManager.cpp

${OBJECTDIR}/src/Platforms/GL/VertexShaderManager.o: src/Platforms/GL/VertexShaderManager.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/GL
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/GL/VertexShaderManager.o src/Platforms/GL/VertexShaderManager.cpp

${OBJECTDIR}/src/Platforms/GTK/Platform.o: src/Platforms/GTK/Platform.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/GTK
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/GTK/Platform.o src/Platforms/GTK/Platform.cpp

${OBJECTDIR}/src/Platforms/Qt/Platform.o: src/Platforms/Qt/Platform.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Qt
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Qt/Platform.o src/Platforms/Qt/Platform.cpp

${OBJECTDIR}/src/Platforms/Qt/QtExecutionModel.o: src/Platforms/Qt/QtExecutionModel.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Qt
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Qt/QtExecutionModel.o src/Platforms/Qt/QtExecutionModel.cpp

${OBJECTDIR}/src/Platforms/Qt/QtKeyboard.o: src/Platforms/Qt/QtKeyboard.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Qt
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Qt/QtKeyboard.o src/Platforms/Qt/QtKeyboard.cpp

${OBJECTDIR}/src/Platforms/Qt/QtMouse.o: src/Platforms/Qt/QtMouse.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Qt
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Qt/QtMouse.o src/Platforms/Qt/QtMouse.cpp

${OBJECTDIR}/src/Platforms/Qt/QtWindow.o: src/Platforms/Qt/QtWindow.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Qt
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Qt/QtWindow.o src/Platforms/Qt/QtWindow.cpp

${OBJECTDIR}/src/Platforms/Wii/CPUTimer.o: src/Platforms/Wii/CPUTimer.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/CPUTimer.o src/Platforms/Wii/CPUTimer.cpp

${OBJECTDIR}/src/Platforms/Wii/GXRenderTarget.o: src/Platforms/Wii/GXRenderTarget.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/GXRenderTarget.o src/Platforms/Wii/GXRenderTarget.cpp

${OBJECTDIR}/src/Platforms/Wii/GXTexture.o: src/Platforms/Wii/GXTexture.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/GXTexture.o src/Platforms/Wii/GXTexture.cpp

${OBJECTDIR}/src/Platforms/Wii/LWPMutex.o: src/Platforms/Wii/LWPMutex.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/LWPMutex.o src/Platforms/Wii/LWPMutex.cpp

${OBJECTDIR}/src/Platforms/Wii/LWPThread.o: src/Platforms/Wii/LWPThread.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/LWPThread.o src/Platforms/Wii/LWPThread.cpp

${OBJECTDIR}/src/Platforms/Wii/Platform.o: src/Platforms/Wii/Platform.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/Platform.o src/Platforms/Wii/Platform.cpp

${OBJECTDIR}/src/Platforms/Wii/SDFileSystemSource.o: src/Platforms/Wii/SDFileSystemSource.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/SDFileSystemSource.o src/Platforms/Wii/SDFileSystemSource.cpp

${OBJECTDIR}/src/Platforms/Wii/SystemInformation.o: src/Platforms/Wii/SystemInformation.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/SystemInformation.o src/Platforms/Wii/SystemInformation.cpp

${OBJECTDIR}/src/Platforms/Wii/USBFileSystemSource.o: src/Platforms/Wii/USBFileSystemSource.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/USBFileSystemSource.o src/Platforms/Wii/USBFileSystemSource.cpp

${OBJECTDIR}/src/Platforms/Wii/WiiSystemInput.o: src/Platforms/Wii/WiiSystemInput.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/WiiSystemInput.o src/Platforms/Wii/WiiSystemInput.cpp

${OBJECTDIR}/src/Platforms/Wii/Wiimote.o: src/Platforms/Wii/Wiimote.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/Wiimote.o src/Platforms/Wii/Wiimote.cpp

${OBJECTDIR}/src/Resource/3dsReader.o: src/Resource/3dsReader.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/3dsReader.o src/Resource/3dsReader.cpp

${OBJECTDIR}/src/Resource/BitmapLoader.o: src/Resource/BitmapLoader.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/BitmapLoader.o src/Resource/BitmapLoader.cpp

${OBJECTDIR}/src/Resource/FontManager.o: src/Resource/FontManager.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/FontManager.o src/Resource/FontManager.cpp

${OBJECTDIR}/src/Resource/JPEGLoader.o: src/Resource/JPEGLoader.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/JPEGLoader.o src/Resource/JPEGLoader.cpp

${OBJECTDIR}/src/Resource/MaterialManager.o: src/Resource/MaterialManager.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/MaterialManager.o src/Resource/MaterialManager.cpp

${OBJECTDIR}/src/Resource/MeshManager.o: src/Resource/MeshManager.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/MeshManager.o src/Resource/MeshManager.cpp

${OBJECTDIR}/src/Resource/MeshReader.o: src/Resource/MeshReader.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/MeshReader.o src/Resource/MeshReader.cpp

${OBJECTDIR}/src/Resource/OggAudioSource.o: src/Resource/OggAudioSource.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/OggAudioSource.o src/Resource/OggAudioSource.cpp

${OBJECTDIR}/src/Resource/PNGLoader.o: src/Resource/PNGLoader.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/PNGLoader.o src/Resource/PNGLoader.cpp

${OBJECTDIR}/src/Resource/RIFFReader.o: src/Resource/RIFFReader.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/RIFFReader.o src/Resource/RIFFReader.cpp

${OBJECTDIR}/src/Resource/ResourceManager.o: src/Resource/ResourceManager.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/ResourceManager.o src/Resource/ResourceManager.cpp

${OBJECTDIR}/src/Resource/SkeletonManager.o: src/Resource/SkeletonManager.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/SkeletonManager.o src/Resource/SkeletonManager.cpp

${OBJECTDIR}/src/Resource/SkeletonReader.o: src/Resource/SkeletonReader.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/SkeletonReader.o src/Resource/SkeletonReader.cpp

${OBJECTDIR}/src/Resource/TextureLoader.o: src/Resource/TextureLoader.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/TextureLoader.o src/Resource/TextureLoader.cpp

${OBJECTDIR}/src/Resource/TextureManager.o: src/Resource/TextureManager.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/TextureManager.o src/Resource/TextureManager.cpp

${OBJECTDIR}/src/Resource/WavAudioSource.o: src/Resource/WavAudioSource.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/WavAudioSource.o src/Resource/WavAudioSource.cpp

${OBJECTDIR}/src/Resource/XMAudioSource.o: src/Resource/XMAudioSource.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/XMAudioSource.o src/Resource/XMAudioSource.cpp

${OBJECTDIR}/src/Shell/Shell.o: src/Shell/Shell.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Shell
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Shell/Shell.o src/Shell/Shell.cpp

${OBJECTDIR}/src/Tile/TMXLoader.o: src/Tile/TMXLoader.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Tile
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Tile/TMXLoader.o src/Tile/TMXLoader.cpp

${OBJECTDIR}/src/Tile/TileLayer.o: src/Tile/TileLayer.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Tile
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Tile/TileLayer.o src/Tile/TileLayer.cpp

${OBJECTDIR}/src/Tile/TileLayerMesh.o: src/Tile/TileLayerMesh.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Tile
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Tile/TileLayerMesh.o src/Tile/TileLayerMesh.cpp

${OBJECTDIR}/src/Tile/TileMap.o: src/Tile/TileMap.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Tile
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Tile/TileMap.o src/Tile/TileMap.cpp

${OBJECTDIR}/src/Tile/TileSet.o: src/Tile/TileSet.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Tile
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Tile/TileSet.o src/Tile/TileSet.cpp

${OBJECTDIR}/src/Util/Configuration.o: src/Util/Configuration.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Util
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Util/Configuration.o src/Util/Configuration.cpp

${OBJECTDIR}/src/Util/ContextSwitcher.o: src/Util/ContextSwitcher.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Util
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Util/ContextSwitcher.o src/Util/ContextSwitcher.cpp

${OBJECTDIR}/src/Util/Parsers.o: src/Util/Parsers.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Util
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Util/Parsers.o src/Util/Parsers.cpp

${OBJECTDIR}/src/Util/RegEx.o: src/Util/RegEx.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Util
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Util/RegEx.o src/Util/RegEx.cpp

${OBJECTDIR}/src/Util/ScenePicker.o: src/Util/ScenePicker.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Util
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Util/ScenePicker.o src/Util/ScenePicker.cpp

${OBJECTDIR}/src/Util/StringSetter.o: src/Util/StringSetter.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Util
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Util/StringSetter.o src/Util/StringSetter.cpp

${OBJECTDIR}/src/Util/StringUtils.o: src/Util/StringUtils.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Util
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Util/StringUtils.o src/Util/StringUtils.cpp

${OBJECTDIR}/src/Util/Texture.o: src/Util/Texture.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Util
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Util/Texture.o src/Util/Texture.cpp

${OBJECTDIR}/src/generated/Font.pb.o: src/generated/Font.pb.cc
	${MKDIR} -p ${OBJECTDIR}/src/generated
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/generated/Font.pb.o src/generated/Font.pb.cc

${OBJECTDIR}/src/generated/GUI.pb.o: src/generated/GUI.pb.cc
	${MKDIR} -p ${OBJECTDIR}/src/generated
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/generated/GUI.pb.o src/generated/GUI.pb.cc

${OBJECTDIR}/src/generated/MappedInputDevice.pb.o: src/generated/MappedInputDevice.pb.cc
	${MKDIR} -p ${OBJECTDIR}/src/generated
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/generated/MappedInputDevice.pb.o src/generated/MappedInputDevice.pb.cc

${OBJECTDIR}/src/generated/Material.pb.o: src/generated/Material.pb.cc
	${MKDIR} -p ${OBJECTDIR}/src/generated
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/generated/Material.pb.o src/generated/Material.pb.cc

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-tests-subprojects .build-conf ${TESTFILES}
.build-tests-subprojects:

${TESTDIR}/TestFiles/f24: ${TESTDIR}/src/tests/AlphaRejectTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f24 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f7: ${TESTDIR}/src/tests/ApplicationTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f7 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f6: ${TESTDIR}/src/tests/AudioTests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f6 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f31: ${TESTDIR}/src/tests/BulletPhysicsTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f31 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f12: ${TESTDIR}/src/tests/ConfigurationTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f12 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f33: ${TESTDIR}/src/tests/ContextSwitcherTests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f33 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f29: ${TESTDIR}/src/tests/ConverterTests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f29 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f8: ${TESTDIR}/src/tests/FontMapTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f8 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f3: ${TESTDIR}/src/tests/FunctionBinderTests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f3 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f20: ${TESTDIR}/src/tests/GUITest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f20 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f5: ${TESTDIR}/src/tests/InputTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f5 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f15: ${TESTDIR}/src/tests/MaterialAnimationTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f15 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f27: ${TESTDIR}/src/tests/MenuTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f27 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f18: ${TESTDIR}/src/tests/MultiPassRenderTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f18 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f22: ${TESTDIR}/src/tests/MultiStageTexturing.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f22 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f11: ${TESTDIR}/src/tests/NetworkTests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f11 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f34: ${TESTDIR}/src/tests/ParserTests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f34 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f32: ${TESTDIR}/src/tests/ParticleTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f32 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f35: ${TESTDIR}/src/tests/PBRTests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f35 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f2: ${TESTDIR}/src/tests/RenderTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f2 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f21: ${TESTDIR}/src/tests/ResourceTests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f21 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f25: ${TESTDIR}/src/tests/ScissorTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f25 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f19: ${TESTDIR}/src/tests/ShaderTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f19 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f17: ${TESTDIR}/src/tests/ShellTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f17 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f14: ${TESTDIR}/src/tests/SkyBoxTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f14 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f30: ${TESTDIR}/src/tests/StringUtilsTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f30 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f28: ${TESTDIR}/src/tests/SystemInformationTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f28 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f1: ${TESTDIR}/src/tests/TaskTests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f1 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f13: ${TESTDIR}/src/tests/TerrainTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f13 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f10: ${TESTDIR}/src/tests/TextRenderTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f10 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f36: ${TESTDIR}/src/tests/TextureTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f36 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f23: ${TESTDIR}/src/tests/TileMapTests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f23 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f16: ${TESTDIR}/src/tests/TimerTests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f16 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f4: ${TESTDIR}/src/tests/TypeTests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f4 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f9: ${TESTDIR}/src/tests/UTFStringTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f9 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f26: ${TESTDIR}/src/tests/VertexBufferTests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f26 $^ ${LDLIBSOPTIONS}   


${TESTDIR}/src/tests/AlphaRejectTest.o: src/tests/AlphaRejectTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/AlphaRejectTest.o src/tests/AlphaRejectTest.cpp


${TESTDIR}/src/tests/ApplicationTest.o: src/tests/ApplicationTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/ApplicationTest.o src/tests/ApplicationTest.cpp


${TESTDIR}/src/tests/AudioTests.o: src/tests/AudioTests.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/AudioTests.o src/tests/AudioTests.cpp


${TESTDIR}/src/tests/BulletPhysicsTest.o: src/tests/BulletPhysicsTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/BulletPhysicsTest.o src/tests/BulletPhysicsTest.cpp


${TESTDIR}/src/tests/ConfigurationTest.o: src/tests/ConfigurationTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/ConfigurationTest.o src/tests/ConfigurationTest.cpp


${TESTDIR}/src/tests/ContextSwitcherTests.o: src/tests/ContextSwitcherTests.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/ContextSwitcherTests.o src/tests/ContextSwitcherTests.cpp


${TESTDIR}/src/tests/ConverterTests.o: src/tests/ConverterTests.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/ConverterTests.o src/tests/ConverterTests.cpp


${TESTDIR}/src/tests/FontMapTest.o: src/tests/FontMapTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/FontMapTest.o src/tests/FontMapTest.cpp


${TESTDIR}/src/tests/FunctionBinderTests.o: src/tests/FunctionBinderTests.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/FunctionBinderTests.o src/tests/FunctionBinderTests.cpp


${TESTDIR}/src/tests/GUITest.o: src/tests/GUITest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/GUITest.o src/tests/GUITest.cpp


${TESTDIR}/src/tests/InputTest.o: src/tests/InputTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/InputTest.o src/tests/InputTest.cpp


${TESTDIR}/src/tests/MaterialAnimationTest.o: src/tests/MaterialAnimationTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/MaterialAnimationTest.o src/tests/MaterialAnimationTest.cpp


${TESTDIR}/src/tests/MenuTest.o: src/tests/MenuTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/MenuTest.o src/tests/MenuTest.cpp


${TESTDIR}/src/tests/MultiPassRenderTest.o: src/tests/MultiPassRenderTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/MultiPassRenderTest.o src/tests/MultiPassRenderTest.cpp


${TESTDIR}/src/tests/MultiStageTexturing.o: src/tests/MultiStageTexturing.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/MultiStageTexturing.o src/tests/MultiStageTexturing.cpp


${TESTDIR}/src/tests/NetworkTests.o: src/tests/NetworkTests.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/NetworkTests.o src/tests/NetworkTests.cpp


${TESTDIR}/src/tests/ParserTests.o: src/tests/ParserTests.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/ParserTests.o src/tests/ParserTests.cpp


${TESTDIR}/src/tests/ParticleTest.o: src/tests/ParticleTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/ParticleTest.o src/tests/ParticleTest.cpp


${TESTDIR}/src/tests/PBRTests.o: src/tests/PBRTests.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/PBRTests.o src/tests/PBRTests.cpp


${TESTDIR}/src/tests/RenderTest.o: src/tests/RenderTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/RenderTest.o src/tests/RenderTest.cpp


${TESTDIR}/src/tests/ResourceTests.o: src/tests/ResourceTests.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/ResourceTests.o src/tests/ResourceTests.cpp


${TESTDIR}/src/tests/ScissorTest.o: src/tests/ScissorTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/ScissorTest.o src/tests/ScissorTest.cpp


${TESTDIR}/src/tests/ShaderTest.o: src/tests/ShaderTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/ShaderTest.o src/tests/ShaderTest.cpp


${TESTDIR}/src/tests/ShellTest.o: src/tests/ShellTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/ShellTest.o src/tests/ShellTest.cpp


${TESTDIR}/src/tests/SkyBoxTest.o: src/tests/SkyBoxTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/SkyBoxTest.o src/tests/SkyBoxTest.cpp


${TESTDIR}/src/tests/StringUtilsTest.o: src/tests/StringUtilsTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/StringUtilsTest.o src/tests/StringUtilsTest.cpp


${TESTDIR}/src/tests/SystemInformationTest.o: src/tests/SystemInformationTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/SystemInformationTest.o src/tests/SystemInformationTest.cpp


${TESTDIR}/src/tests/TaskTests.o: src/tests/TaskTests.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/TaskTests.o src/tests/TaskTests.cpp


${TESTDIR}/src/tests/TerrainTest.o: src/tests/TerrainTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/TerrainTest.o src/tests/TerrainTest.cpp


${TESTDIR}/src/tests/TextRenderTest.o: src/tests/TextRenderTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/TextRenderTest.o src/tests/TextRenderTest.cpp


${TESTDIR}/src/tests/TextureTest.o: src/tests/TextureTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/TextureTest.o src/tests/TextureTest.cpp


${TESTDIR}/src/tests/TileMapTests.o: src/tests/TileMapTests.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/TileMapTests.o src/tests/TileMapTests.cpp


${TESTDIR}/src/tests/TimerTests.o: src/tests/TimerTests.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/TimerTests.o src/tests/TimerTests.cpp


${TESTDIR}/src/tests/TypeTests.o: src/tests/TypeTests.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/TypeTests.o src/tests/TypeTests.cpp


${TESTDIR}/src/tests/UTFStringTest.o: src/tests/UTFStringTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/UTFStringTest.o src/tests/UTFStringTest.cpp


${TESTDIR}/src/tests/VertexBufferTests.o: src/tests/VertexBufferTests.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/VertexBufferTests.o src/tests/VertexBufferTests.cpp


${OBJECTDIR}/src/Animation/Animation_nomain.o: ${OBJECTDIR}/src/Animation/Animation.o src/Animation/Animation.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Animation
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Animation/Animation.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Animation/Animation_nomain.o src/Animation/Animation.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Animation/Animation.o ${OBJECTDIR}/src/Animation/Animation_nomain.o;\
	fi

${OBJECTDIR}/src/Animation/AnimationState_nomain.o: ${OBJECTDIR}/src/Animation/AnimationState.o src/Animation/AnimationState.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Animation
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Animation/AnimationState.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Animation/AnimationState_nomain.o src/Animation/AnimationState.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Animation/AnimationState.o ${OBJECTDIR}/src/Animation/AnimationState_nomain.o;\
	fi

${OBJECTDIR}/src/Animation/AnimationTrack_nomain.o: ${OBJECTDIR}/src/Animation/AnimationTrack.o src/Animation/AnimationTrack.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Animation
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Animation/AnimationTrack.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Animation/AnimationTrack_nomain.o src/Animation/AnimationTrack.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Animation/AnimationTrack.o ${OBJECTDIR}/src/Animation/AnimationTrack_nomain.o;\
	fi

${OBJECTDIR}/src/Animation/BoneAnimationTrack_nomain.o: ${OBJECTDIR}/src/Animation/BoneAnimationTrack.o src/Animation/BoneAnimationTrack.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Animation
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Animation/BoneAnimationTrack.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Animation/BoneAnimationTrack_nomain.o src/Animation/BoneAnimationTrack.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Animation/BoneAnimationTrack.o ${OBJECTDIR}/src/Animation/BoneAnimationTrack_nomain.o;\
	fi

${OBJECTDIR}/src/Animation/Skeleton_nomain.o: ${OBJECTDIR}/src/Animation/Skeleton.o src/Animation/Skeleton.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Animation
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Animation/Skeleton.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Animation/Skeleton_nomain.o src/Animation/Skeleton.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Animation/Skeleton.o ${OBJECTDIR}/src/Animation/Skeleton_nomain.o;\
	fi

${OBJECTDIR}/src/Animation/SkeletonAnimation_nomain.o: ${OBJECTDIR}/src/Animation/SkeletonAnimation.o src/Animation/SkeletonAnimation.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Animation
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Animation/SkeletonAnimation.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Animation/SkeletonAnimation_nomain.o src/Animation/SkeletonAnimation.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Animation/SkeletonAnimation.o ${OBJECTDIR}/src/Animation/SkeletonAnimation_nomain.o;\
	fi

${OBJECTDIR}/src/Animation/SkeletonAnimationState_nomain.o: ${OBJECTDIR}/src/Animation/SkeletonAnimationState.o src/Animation/SkeletonAnimationState.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Animation
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Animation/SkeletonAnimationState.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Animation/SkeletonAnimationState_nomain.o src/Animation/SkeletonAnimationState.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Animation/SkeletonAnimationState.o ${OBJECTDIR}/src/Animation/SkeletonAnimationState_nomain.o;\
	fi

${OBJECTDIR}/src/Animation/SpriteAnimation_nomain.o: ${OBJECTDIR}/src/Animation/SpriteAnimation.o src/Animation/SpriteAnimation.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Animation
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Animation/SpriteAnimation.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Animation/SpriteAnimation_nomain.o src/Animation/SpriteAnimation.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Animation/SpriteAnimation.o ${OBJECTDIR}/src/Animation/SpriteAnimation_nomain.o;\
	fi

${OBJECTDIR}/src/Application_nomain.o: ${OBJECTDIR}/src/Application.o src/Application.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Application.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Application_nomain.o src/Application.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Application.o ${OBJECTDIR}/src/Application_nomain.o;\
	fi

${OBJECTDIR}/src/Audio/Audio_nomain.o: ${OBJECTDIR}/src/Audio/Audio.o src/Audio/Audio.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Audio
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Audio/Audio.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Audio/Audio_nomain.o src/Audio/Audio.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Audio/Audio.o ${OBJECTDIR}/src/Audio/Audio_nomain.o;\
	fi

${OBJECTDIR}/src/Audio/AudioBuffer_nomain.o: ${OBJECTDIR}/src/Audio/AudioBuffer.o src/Audio/AudioBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Audio
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Audio/AudioBuffer.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Audio/AudioBuffer_nomain.o src/Audio/AudioBuffer.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Audio/AudioBuffer.o ${OBJECTDIR}/src/Audio/AudioBuffer_nomain.o;\
	fi

${OBJECTDIR}/src/Audio/AudioPlayer_nomain.o: ${OBJECTDIR}/src/Audio/AudioPlayer.o src/Audio/AudioPlayer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Audio
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Audio/AudioPlayer.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Audio/AudioPlayer_nomain.o src/Audio/AudioPlayer.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Audio/AudioPlayer.o ${OBJECTDIR}/src/Audio/AudioPlayer_nomain.o;\
	fi

${OBJECTDIR}/src/Audio/AudioSource_nomain.o: ${OBJECTDIR}/src/Audio/AudioSource.o src/Audio/AudioSource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Audio
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Audio/AudioSource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Audio/AudioSource_nomain.o src/Audio/AudioSource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Audio/AudioSource.o ${OBJECTDIR}/src/Audio/AudioSource_nomain.o;\
	fi

${OBJECTDIR}/src/Audio/AudioStream_nomain.o: ${OBJECTDIR}/src/Audio/AudioStream.o src/Audio/AudioStream.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Audio
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Audio/AudioStream.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Audio/AudioStream_nomain.o src/Audio/AudioStream.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Audio/AudioStream.o ${OBJECTDIR}/src/Audio/AudioStream_nomain.o;\
	fi

${OBJECTDIR}/src/Chrono/CPUTimer_nomain.o: ${OBJECTDIR}/src/Chrono/CPUTimer.o src/Chrono/CPUTimer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Chrono
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Chrono/CPUTimer.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Chrono/CPUTimer_nomain.o src/Chrono/CPUTimer.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Chrono/CPUTimer.o ${OBJECTDIR}/src/Chrono/CPUTimer_nomain.o;\
	fi

${OBJECTDIR}/src/Chrono/Chrono_nomain.o: ${OBJECTDIR}/src/Chrono/Chrono.o src/Chrono/Chrono.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Chrono
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Chrono/Chrono.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Chrono/Chrono_nomain.o src/Chrono/Chrono.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Chrono/Chrono.o ${OBJECTDIR}/src/Chrono/Chrono_nomain.o;\
	fi

${OBJECTDIR}/src/Chrono/FrameRateLimiter_nomain.o: ${OBJECTDIR}/src/Chrono/FrameRateLimiter.o src/Chrono/FrameRateLimiter.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Chrono
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Chrono/FrameRateLimiter.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Chrono/FrameRateLimiter_nomain.o src/Chrono/FrameRateLimiter.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Chrono/FrameRateLimiter.o ${OBJECTDIR}/src/Chrono/FrameRateLimiter_nomain.o;\
	fi

${OBJECTDIR}/src/FileSystem/File_nomain.o: ${OBJECTDIR}/src/FileSystem/File.o src/FileSystem/File.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/FileSystem/File.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/File_nomain.o src/FileSystem/File.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/FileSystem/File.o ${OBJECTDIR}/src/FileSystem/File_nomain.o;\
	fi

${OBJECTDIR}/src/FileSystem/FileReferenceEncrypted_nomain.o: ${OBJECTDIR}/src/FileSystem/FileReferenceEncrypted.o src/FileSystem/FileReferenceEncrypted.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/FileSystem/FileReferenceEncrypted.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileReferenceEncrypted_nomain.o src/FileSystem/FileReferenceEncrypted.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/FileSystem/FileReferenceEncrypted.o ${OBJECTDIR}/src/FileSystem/FileReferenceEncrypted_nomain.o;\
	fi

${OBJECTDIR}/src/FileSystem/FileReferenceFile_nomain.o: ${OBJECTDIR}/src/FileSystem/FileReferenceFile.o src/FileSystem/FileReferenceFile.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/FileSystem/FileReferenceFile.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileReferenceFile_nomain.o src/FileSystem/FileReferenceFile.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/FileSystem/FileReferenceFile.o ${OBJECTDIR}/src/FileSystem/FileReferenceFile_nomain.o;\
	fi

${OBJECTDIR}/src/FileSystem/FileReferenceMemory_nomain.o: ${OBJECTDIR}/src/FileSystem/FileReferenceMemory.o src/FileSystem/FileReferenceMemory.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/FileSystem/FileReferenceMemory.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileReferenceMemory_nomain.o src/FileSystem/FileReferenceMemory.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/FileSystem/FileReferenceMemory.o ${OBJECTDIR}/src/FileSystem/FileReferenceMemory_nomain.o;\
	fi

${OBJECTDIR}/src/FileSystem/FileReferenceVFS_nomain.o: ${OBJECTDIR}/src/FileSystem/FileReferenceVFS.o src/FileSystem/FileReferenceVFS.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/FileSystem/FileReferenceVFS.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileReferenceVFS_nomain.o src/FileSystem/FileReferenceVFS.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/FileSystem/FileReferenceVFS.o ${OBJECTDIR}/src/FileSystem/FileReferenceVFS_nomain.o;\
	fi

${OBJECTDIR}/src/FileSystem/FileSystem_nomain.o: ${OBJECTDIR}/src/FileSystem/FileSystem.o src/FileSystem/FileSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/FileSystem/FileSystem.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileSystem_nomain.o src/FileSystem/FileSystem.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/FileSystem/FileSystem.o ${OBJECTDIR}/src/FileSystem/FileSystem_nomain.o;\
	fi

${OBJECTDIR}/src/FileSystem/FileSystemSource_nomain.o: ${OBJECTDIR}/src/FileSystem/FileSystemSource.o src/FileSystem/FileSystemSource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/FileSystem/FileSystemSource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileSystemSource_nomain.o src/FileSystem/FileSystemSource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/FileSystem/FileSystemSource.o ${OBJECTDIR}/src/FileSystem/FileSystemSource_nomain.o;\
	fi

${OBJECTDIR}/src/FileSystem/FileSystemSourceEncrypted_nomain.o: ${OBJECTDIR}/src/FileSystem/FileSystemSourceEncrypted.o src/FileSystem/FileSystemSourceEncrypted.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/FileSystem/FileSystemSourceEncrypted.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileSystemSourceEncrypted_nomain.o src/FileSystem/FileSystemSourceEncrypted.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/FileSystem/FileSystemSourceEncrypted.o ${OBJECTDIR}/src/FileSystem/FileSystemSourceEncrypted_nomain.o;\
	fi

${OBJECTDIR}/src/FileSystem/FileSystemSourceFile_nomain.o: ${OBJECTDIR}/src/FileSystem/FileSystemSourceFile.o src/FileSystem/FileSystemSourceFile.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/FileSystem/FileSystemSourceFile.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileSystemSourceFile_nomain.o src/FileSystem/FileSystemSourceFile.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/FileSystem/FileSystemSourceFile.o ${OBJECTDIR}/src/FileSystem/FileSystemSourceFile_nomain.o;\
	fi

${OBJECTDIR}/src/FileSystem/FileSystemSourceMemory_nomain.o: ${OBJECTDIR}/src/FileSystem/FileSystemSourceMemory.o src/FileSystem/FileSystemSourceMemory.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/FileSystem/FileSystemSourceMemory.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileSystemSourceMemory_nomain.o src/FileSystem/FileSystemSourceMemory.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/FileSystem/FileSystemSourceMemory.o ${OBJECTDIR}/src/FileSystem/FileSystemSourceMemory_nomain.o;\
	fi

${OBJECTDIR}/src/FileSystem/FileSystemSourceVFS_nomain.o: ${OBJECTDIR}/src/FileSystem/FileSystemSourceVFS.o src/FileSystem/FileSystemSourceVFS.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/FileSystem
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/FileSystem/FileSystemSourceVFS.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileSystem/FileSystemSourceVFS_nomain.o src/FileSystem/FileSystemSourceVFS.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/FileSystem/FileSystemSourceVFS.o ${OBJECTDIR}/src/FileSystem/FileSystemSourceVFS_nomain.o;\
	fi

${OBJECTDIR}/src/GUI/Button_nomain.o: ${OBJECTDIR}/src/GUI/Button.o src/GUI/Button.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/GUI/Button.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Button_nomain.o src/GUI/Button.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/GUI/Button.o ${OBJECTDIR}/src/GUI/Button_nomain.o;\
	fi

${OBJECTDIR}/src/GUI/Container_nomain.o: ${OBJECTDIR}/src/GUI/Container.o src/GUI/Container.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/GUI/Container.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Container_nomain.o src/GUI/Container.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/GUI/Container.o ${OBJECTDIR}/src/GUI/Container_nomain.o;\
	fi

${OBJECTDIR}/src/GUI/Cursor_nomain.o: ${OBJECTDIR}/src/GUI/Cursor.o src/GUI/Cursor.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/GUI/Cursor.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Cursor_nomain.o src/GUI/Cursor.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/GUI/Cursor.o ${OBJECTDIR}/src/GUI/Cursor_nomain.o;\
	fi

${OBJECTDIR}/src/GUI/Element_nomain.o: ${OBJECTDIR}/src/GUI/Element.o src/GUI/Element.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/GUI/Element.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Element_nomain.o src/GUI/Element.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/GUI/Element.o ${OBJECTDIR}/src/GUI/Element_nomain.o;\
	fi

${OBJECTDIR}/src/GUI/GUI_nomain.o: ${OBJECTDIR}/src/GUI/GUI.o src/GUI/GUI.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/GUI/GUI.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/GUI_nomain.o src/GUI/GUI.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/GUI/GUI.o ${OBJECTDIR}/src/GUI/GUI_nomain.o;\
	fi

${OBJECTDIR}/src/GUI/Image_nomain.o: ${OBJECTDIR}/src/GUI/Image.o src/GUI/Image.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/GUI/Image.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Image_nomain.o src/GUI/Image.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/GUI/Image.o ${OBJECTDIR}/src/GUI/Image_nomain.o;\
	fi

${OBJECTDIR}/src/GUI/Layout_nomain.o: ${OBJECTDIR}/src/GUI/Layout.o src/GUI/Layout.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/GUI/Layout.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Layout_nomain.o src/GUI/Layout.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/GUI/Layout.o ${OBJECTDIR}/src/GUI/Layout_nomain.o;\
	fi

${OBJECTDIR}/src/GUI/Menu_nomain.o: ${OBJECTDIR}/src/GUI/Menu.o src/GUI/Menu.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/GUI/Menu.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Menu_nomain.o src/GUI/Menu.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/GUI/Menu.o ${OBJECTDIR}/src/GUI/Menu_nomain.o;\
	fi

${OBJECTDIR}/src/GUI/Screen_nomain.o: ${OBJECTDIR}/src/GUI/Screen.o src/GUI/Screen.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/GUI/Screen.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Screen_nomain.o src/GUI/Screen.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/GUI/Screen.o ${OBJECTDIR}/src/GUI/Screen_nomain.o;\
	fi

${OBJECTDIR}/src/GUI/Text_nomain.o: ${OBJECTDIR}/src/GUI/Text.o src/GUI/Text.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/GUI/Text.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/Text_nomain.o src/GUI/Text.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/GUI/Text.o ${OBJECTDIR}/src/GUI/Text_nomain.o;\
	fi

${OBJECTDIR}/src/GUI/TextBox_nomain.o: ${OBJECTDIR}/src/GUI/TextBox.o src/GUI/TextBox.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/GUI
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/GUI/TextBox.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GUI/TextBox_nomain.o src/GUI/TextBox.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/GUI/TextBox.o ${OBJECTDIR}/src/GUI/TextBox_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/Camera_nomain.o: ${OBJECTDIR}/src/Graphics/Camera.o src/Graphics/Camera.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/Camera.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Camera_nomain.o src/Graphics/Camera.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/Camera.o ${OBJECTDIR}/src/Graphics/Camera_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/CameraDollies_nomain.o: ${OBJECTDIR}/src/Graphics/CameraDollies.o src/Graphics/CameraDollies.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/CameraDollies.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/CameraDollies_nomain.o src/Graphics/CameraDollies.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/CameraDollies.o ${OBJECTDIR}/src/Graphics/CameraDollies_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/Colour_nomain.o: ${OBJECTDIR}/src/Graphics/Colour.o src/Graphics/Colour.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/Colour.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Colour_nomain.o src/Graphics/Colour.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/Colour.o ${OBJECTDIR}/src/Graphics/Colour_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/ElementBuffer_nomain.o: ${OBJECTDIR}/src/Graphics/ElementBuffer.o src/Graphics/ElementBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/ElementBuffer.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/ElementBuffer_nomain.o src/Graphics/ElementBuffer.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/ElementBuffer.o ${OBJECTDIR}/src/Graphics/ElementBuffer_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/Frustum_nomain.o: ${OBJECTDIR}/src/Graphics/Frustum.o src/Graphics/Frustum.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/Frustum.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Frustum_nomain.o src/Graphics/Frustum.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/Frustum.o ${OBJECTDIR}/src/Graphics/Frustum_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/Light_nomain.o: ${OBJECTDIR}/src/Graphics/Light.o src/Graphics/Light.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/Light.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Light_nomain.o src/Graphics/Light.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/Light.o ${OBJECTDIR}/src/Graphics/Light_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/Material_nomain.o: ${OBJECTDIR}/src/Graphics/Material.o src/Graphics/Material.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/Material.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Material_nomain.o src/Graphics/Material.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/Material.o ${OBJECTDIR}/src/Graphics/Material_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/MaterialAnimation_nomain.o: ${OBJECTDIR}/src/Graphics/MaterialAnimation.o src/Graphics/MaterialAnimation.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/MaterialAnimation.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/MaterialAnimation_nomain.o src/Graphics/MaterialAnimation.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/MaterialAnimation.o ${OBJECTDIR}/src/Graphics/MaterialAnimation_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/Mesh_nomain.o: ${OBJECTDIR}/src/Graphics/Mesh.o src/Graphics/Mesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/Mesh.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Mesh_nomain.o src/Graphics/Mesh.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/Mesh.o ${OBJECTDIR}/src/Graphics/Mesh_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/MultiRenderer_nomain.o: ${OBJECTDIR}/src/Graphics/MultiRenderer.o src/Graphics/MultiRenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/MultiRenderer.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/MultiRenderer_nomain.o src/Graphics/MultiRenderer.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/MultiRenderer.o ${OBJECTDIR}/src/Graphics/MultiRenderer_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/MultipassRenderable_nomain.o: ${OBJECTDIR}/src/Graphics/MultipassRenderable.o src/Graphics/MultipassRenderable.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/MultipassRenderable.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/MultipassRenderable_nomain.o src/Graphics/MultipassRenderable.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/MultipassRenderable.o ${OBJECTDIR}/src/Graphics/MultipassRenderable_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/Node_nomain.o: ${OBJECTDIR}/src/Graphics/Node.o src/Graphics/Node.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/Node.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Node_nomain.o src/Graphics/Node.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/Node.o ${OBJECTDIR}/src/Graphics/Node_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/PrimitiveTypes_nomain.o: ${OBJECTDIR}/src/Graphics/PrimitiveTypes.o src/Graphics/PrimitiveTypes.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/PrimitiveTypes.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/PrimitiveTypes_nomain.o src/Graphics/PrimitiveTypes.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/PrimitiveTypes.o ${OBJECTDIR}/src/Graphics/PrimitiveTypes_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/RenderPass_nomain.o: ${OBJECTDIR}/src/Graphics/RenderPass.o src/Graphics/RenderPass.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/RenderPass.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/RenderPass_nomain.o src/Graphics/RenderPass.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/RenderPass.o ${OBJECTDIR}/src/Graphics/RenderPass_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/RenderTarget_nomain.o: ${OBJECTDIR}/src/Graphics/RenderTarget.o src/Graphics/RenderTarget.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/RenderTarget.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/RenderTarget_nomain.o src/Graphics/RenderTarget.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/RenderTarget.o ${OBJECTDIR}/src/Graphics/RenderTarget_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/RenderTargetNotifier_nomain.o: ${OBJECTDIR}/src/Graphics/RenderTargetNotifier.o src/Graphics/RenderTargetNotifier.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/RenderTargetNotifier.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/RenderTargetNotifier_nomain.o src/Graphics/RenderTargetNotifier.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/RenderTargetNotifier.o ${OBJECTDIR}/src/Graphics/RenderTargetNotifier_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/Renderable_nomain.o: ${OBJECTDIR}/src/Graphics/Renderable.o src/Graphics/Renderable.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/Renderable.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Renderable_nomain.o src/Graphics/Renderable.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/Renderable.o ${OBJECTDIR}/src/Graphics/Renderable_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/Renderer_nomain.o: ${OBJECTDIR}/src/Graphics/Renderer.o src/Graphics/Renderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/Renderer.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Renderer_nomain.o src/Graphics/Renderer.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/Renderer.o ${OBJECTDIR}/src/Graphics/Renderer_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/Scene_nomain.o: ${OBJECTDIR}/src/Graphics/Scene.o src/Graphics/Scene.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/Scene.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Scene_nomain.o src/Graphics/Scene.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/Scene.o ${OBJECTDIR}/src/Graphics/Scene_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/SceneEntity_nomain.o: ${OBJECTDIR}/src/Graphics/SceneEntity.o src/Graphics/SceneEntity.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/SceneEntity.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/SceneEntity_nomain.o src/Graphics/SceneEntity.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/SceneEntity.o ${OBJECTDIR}/src/Graphics/SceneEntity_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/SceneRenderable_nomain.o: ${OBJECTDIR}/src/Graphics/SceneRenderable.o src/Graphics/SceneRenderable.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/SceneRenderable.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/SceneRenderable_nomain.o src/Graphics/SceneRenderable.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/SceneRenderable.o ${OBJECTDIR}/src/Graphics/SceneRenderable_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/Shader_nomain.o: ${OBJECTDIR}/src/Graphics/Shader.o src/Graphics/Shader.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/Shader.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Shader_nomain.o src/Graphics/Shader.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/Shader.o ${OBJECTDIR}/src/Graphics/Shader_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/ShaderProgram_nomain.o: ${OBJECTDIR}/src/Graphics/ShaderProgram.o src/Graphics/ShaderProgram.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/ShaderProgram.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/ShaderProgram_nomain.o src/Graphics/ShaderProgram.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/ShaderProgram.o ${OBJECTDIR}/src/Graphics/ShaderProgram_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/SkyBox_nomain.o: ${OBJECTDIR}/src/Graphics/SkyBox.o src/Graphics/SkyBox.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/SkyBox.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/SkyBox_nomain.o src/Graphics/SkyBox.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/SkyBox.o ${OBJECTDIR}/src/Graphics/SkyBox_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/Sprite_nomain.o: ${OBJECTDIR}/src/Graphics/Sprite.o src/Graphics/Sprite.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/Sprite.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Sprite_nomain.o src/Graphics/Sprite.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/Sprite.o ${OBJECTDIR}/src/Graphics/Sprite_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/StereoscopicRenderer_nomain.o: ${OBJECTDIR}/src/Graphics/StereoscopicRenderer.o src/Graphics/StereoscopicRenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/StereoscopicRenderer.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/StereoscopicRenderer_nomain.o src/Graphics/StereoscopicRenderer.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/StereoscopicRenderer.o ${OBJECTDIR}/src/Graphics/StereoscopicRenderer_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/SubMesh_nomain.o: ${OBJECTDIR}/src/Graphics/SubMesh.o src/Graphics/SubMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/SubMesh.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/SubMesh_nomain.o src/Graphics/SubMesh.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/SubMesh.o ${OBJECTDIR}/src/Graphics/SubMesh_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/Terrain_nomain.o: ${OBJECTDIR}/src/Graphics/Terrain.o src/Graphics/Terrain.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/Terrain.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Terrain_nomain.o src/Graphics/Terrain.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/Terrain.o ${OBJECTDIR}/src/Graphics/Terrain_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/TextMesh_nomain.o: ${OBJECTDIR}/src/Graphics/TextMesh.o src/Graphics/TextMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/TextMesh.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/TextMesh_nomain.o src/Graphics/TextMesh.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/TextMesh.o ${OBJECTDIR}/src/Graphics/TextMesh_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/Texture_nomain.o: ${OBJECTDIR}/src/Graphics/Texture.o src/Graphics/Texture.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/Texture.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Texture_nomain.o src/Graphics/Texture.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/Texture.o ${OBJECTDIR}/src/Graphics/Texture_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/TextureUnit_nomain.o: ${OBJECTDIR}/src/Graphics/TextureUnit.o src/Graphics/TextureUnit.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/TextureUnit.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/TextureUnit_nomain.o src/Graphics/TextureUnit.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/TextureUnit.o ${OBJECTDIR}/src/Graphics/TextureUnit_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/VertexAttribute_nomain.o: ${OBJECTDIR}/src/Graphics/VertexAttribute.o src/Graphics/VertexAttribute.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/VertexAttribute.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/VertexAttribute_nomain.o src/Graphics/VertexAttribute.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/VertexAttribute.o ${OBJECTDIR}/src/Graphics/VertexAttribute_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/VertexBuffer_nomain.o: ${OBJECTDIR}/src/Graphics/VertexBuffer.o src/Graphics/VertexBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/VertexBuffer.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/VertexBuffer_nomain.o src/Graphics/VertexBuffer.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/VertexBuffer.o ${OBJECTDIR}/src/Graphics/VertexBuffer_nomain.o;\
	fi

${OBJECTDIR}/src/Graphics/Viewport_nomain.o: ${OBJECTDIR}/src/Graphics/Viewport.o src/Graphics/Viewport.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Graphics/Viewport.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Viewport_nomain.o src/Graphics/Viewport.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Graphics/Viewport.o ${OBJECTDIR}/src/Graphics/Viewport_nomain.o;\
	fi

${OBJECTDIR}/src/Input/GenericInputDevice_nomain.o: ${OBJECTDIR}/src/Input/GenericInputDevice.o src/Input/GenericInputDevice.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Input
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Input/GenericInputDevice.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/GenericInputDevice_nomain.o src/Input/GenericInputDevice.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Input/GenericInputDevice.o ${OBJECTDIR}/src/Input/GenericInputDevice_nomain.o;\
	fi

${OBJECTDIR}/src/Input/Input_nomain.o: ${OBJECTDIR}/src/Input/Input.o src/Input/Input.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Input
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Input/Input.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/Input_nomain.o src/Input/Input.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Input/Input.o ${OBJECTDIR}/src/Input/Input_nomain.o;\
	fi

${OBJECTDIR}/src/Input/InputDevice_nomain.o: ${OBJECTDIR}/src/Input/InputDevice.o src/Input/InputDevice.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Input
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Input/InputDevice.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/InputDevice_nomain.o src/Input/InputDevice.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Input/InputDevice.o ${OBJECTDIR}/src/Input/InputDevice_nomain.o;\
	fi

${OBJECTDIR}/src/Input/MappedInputDevice_nomain.o: ${OBJECTDIR}/src/Input/MappedInputDevice.o src/Input/MappedInputDevice.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Input
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Input/MappedInputDevice.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/MappedInputDevice_nomain.o src/Input/MappedInputDevice.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Input/MappedInputDevice.o ${OBJECTDIR}/src/Input/MappedInputDevice_nomain.o;\
	fi

${OBJECTDIR}/src/Input/MappedInputDeviceLoader_nomain.o: ${OBJECTDIR}/src/Input/MappedInputDeviceLoader.o src/Input/MappedInputDeviceLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Input
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Input/MappedInputDeviceLoader.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/MappedInputDeviceLoader_nomain.o src/Input/MappedInputDeviceLoader.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Input/MappedInputDeviceLoader.o ${OBJECTDIR}/src/Input/MappedInputDeviceLoader_nomain.o;\
	fi

${OBJECTDIR}/src/Input/OnScreenAnalogueStick_nomain.o: ${OBJECTDIR}/src/Input/OnScreenAnalogueStick.o src/Input/OnScreenAnalogueStick.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Input
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Input/OnScreenAnalogueStick.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/OnScreenAnalogueStick_nomain.o src/Input/OnScreenAnalogueStick.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Input/OnScreenAnalogueStick.o ${OBJECTDIR}/src/Input/OnScreenAnalogueStick_nomain.o;\
	fi

${OBJECTDIR}/src/Input/PseudoKeyboard_nomain.o: ${OBJECTDIR}/src/Input/PseudoKeyboard.o src/Input/PseudoKeyboard.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Input
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Input/PseudoKeyboard.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/PseudoKeyboard_nomain.o src/Input/PseudoKeyboard.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Input/PseudoKeyboard.o ${OBJECTDIR}/src/Input/PseudoKeyboard_nomain.o;\
	fi

${OBJECTDIR}/src/Input/PseudoMouse_nomain.o: ${OBJECTDIR}/src/Input/PseudoMouse.o src/Input/PseudoMouse.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Input
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Input/PseudoMouse.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/PseudoMouse_nomain.o src/Input/PseudoMouse.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Input/PseudoMouse.o ${OBJECTDIR}/src/Input/PseudoMouse_nomain.o;\
	fi

${OBJECTDIR}/src/Kernel/ExecutionModel_nomain.o: ${OBJECTDIR}/src/Kernel/ExecutionModel.o src/Kernel/ExecutionModel.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Kernel
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Kernel/ExecutionModel.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Kernel/ExecutionModel_nomain.o src/Kernel/ExecutionModel.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Kernel/ExecutionModel.o ${OBJECTDIR}/src/Kernel/ExecutionModel_nomain.o;\
	fi

${OBJECTDIR}/src/Kernel/Kernel_nomain.o: ${OBJECTDIR}/src/Kernel/Kernel.o src/Kernel/Kernel.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Kernel
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Kernel/Kernel.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Kernel/Kernel_nomain.o src/Kernel/Kernel.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Kernel/Kernel.o ${OBJECTDIR}/src/Kernel/Kernel_nomain.o;\
	fi

${OBJECTDIR}/src/Kernel/Task_nomain.o: ${OBJECTDIR}/src/Kernel/Task.o src/Kernel/Task.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Kernel
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Kernel/Task.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Kernel/Task_nomain.o src/Kernel/Task.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Kernel/Task.o ${OBJECTDIR}/src/Kernel/Task_nomain.o;\
	fi

${OBJECTDIR}/src/Kernel/TaskGroup_nomain.o: ${OBJECTDIR}/src/Kernel/TaskGroup.o src/Kernel/TaskGroup.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Kernel
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Kernel/TaskGroup.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Kernel/TaskGroup_nomain.o src/Kernel/TaskGroup.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Kernel/TaskGroup.o ${OBJECTDIR}/src/Kernel/TaskGroup_nomain.o;\
	fi

${OBJECTDIR}/src/Kernel/TaskManager_nomain.o: ${OBJECTDIR}/src/Kernel/TaskManager.o src/Kernel/TaskManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Kernel
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Kernel/TaskManager.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Kernel/TaskManager_nomain.o src/Kernel/TaskManager.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Kernel/TaskManager.o ${OBJECTDIR}/src/Kernel/TaskManager_nomain.o;\
	fi

${OBJECTDIR}/src/Kernel/Thread_nomain.o: ${OBJECTDIR}/src/Kernel/Thread.o src/Kernel/Thread.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Kernel
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Kernel/Thread.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Kernel/Thread_nomain.o src/Kernel/Thread.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Kernel/Thread.o ${OBJECTDIR}/src/Kernel/Thread_nomain.o;\
	fi

${OBJECTDIR}/src/Logging/Logging_nomain.o: ${OBJECTDIR}/src/Logging/Logging.o src/Logging/Logging.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Logging
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Logging/Logging.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Logging/Logging_nomain.o src/Logging/Logging.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Logging/Logging.o ${OBJECTDIR}/src/Logging/Logging_nomain.o;\
	fi

${OBJECTDIR}/src/Maths/AxisAlignedBox_nomain.o: ${OBJECTDIR}/src/Maths/AxisAlignedBox.o src/Maths/AxisAlignedBox.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Maths
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Maths/AxisAlignedBox.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Maths/AxisAlignedBox_nomain.o src/Maths/AxisAlignedBox.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Maths/AxisAlignedBox.o ${OBJECTDIR}/src/Maths/AxisAlignedBox_nomain.o;\
	fi

${OBJECTDIR}/src/Maths/EchoMaths_nomain.o: ${OBJECTDIR}/src/Maths/EchoMaths.o src/Maths/EchoMaths.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Maths
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Maths/EchoMaths.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Maths/EchoMaths_nomain.o src/Maths/EchoMaths.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Maths/EchoMaths.o ${OBJECTDIR}/src/Maths/EchoMaths_nomain.o;\
	fi

${OBJECTDIR}/src/Maths/Geometry_nomain.o: ${OBJECTDIR}/src/Maths/Geometry.o src/Maths/Geometry.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Maths
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Maths/Geometry.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Maths/Geometry_nomain.o src/Maths/Geometry.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Maths/Geometry.o ${OBJECTDIR}/src/Maths/Geometry_nomain.o;\
	fi

${OBJECTDIR}/src/Maths/Matrix4_nomain.o: ${OBJECTDIR}/src/Maths/Matrix4.o src/Maths/Matrix4.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Maths
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Maths/Matrix4.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Maths/Matrix4_nomain.o src/Maths/Matrix4.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Maths/Matrix4.o ${OBJECTDIR}/src/Maths/Matrix4_nomain.o;\
	fi

${OBJECTDIR}/src/Maths/Plane_nomain.o: ${OBJECTDIR}/src/Maths/Plane.o src/Maths/Plane.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Maths
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Maths/Plane.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Maths/Plane_nomain.o src/Maths/Plane.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Maths/Plane.o ${OBJECTDIR}/src/Maths/Plane_nomain.o;\
	fi

${OBJECTDIR}/src/Maths/Quaternion_nomain.o: ${OBJECTDIR}/src/Maths/Quaternion.o src/Maths/Quaternion.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Maths
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Maths/Quaternion.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Maths/Quaternion_nomain.o src/Maths/Quaternion.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Maths/Quaternion.o ${OBJECTDIR}/src/Maths/Quaternion_nomain.o;\
	fi

${OBJECTDIR}/src/Maths/Vector4_nomain.o: ${OBJECTDIR}/src/Maths/Vector4.o src/Maths/Vector4.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Maths
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Maths/Vector4.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Maths/Vector4_nomain.o src/Maths/Vector4.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Maths/Vector4.o ${OBJECTDIR}/src/Maths/Vector4_nomain.o;\
	fi

${OBJECTDIR}/src/Network/Connection_nomain.o: ${OBJECTDIR}/src/Network/Connection.o src/Network/Connection.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Network
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Network/Connection.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/Connection_nomain.o src/Network/Connection.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Network/Connection.o ${OBJECTDIR}/src/Network/Connection_nomain.o;\
	fi

${OBJECTDIR}/src/Network/ConnectionDetails_nomain.o: ${OBJECTDIR}/src/Network/ConnectionDetails.o src/Network/ConnectionDetails.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Network
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Network/ConnectionDetails.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/ConnectionDetails_nomain.o src/Network/ConnectionDetails.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Network/ConnectionDetails.o ${OBJECTDIR}/src/Network/ConnectionDetails_nomain.o;\
	fi

${OBJECTDIR}/src/Network/DataPacket_nomain.o: ${OBJECTDIR}/src/Network/DataPacket.o src/Network/DataPacket.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Network
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Network/DataPacket.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/DataPacket_nomain.o src/Network/DataPacket.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Network/DataPacket.o ${OBJECTDIR}/src/Network/DataPacket_nomain.o;\
	fi

${OBJECTDIR}/src/Network/DataPacketFactory_nomain.o: ${OBJECTDIR}/src/Network/DataPacketFactory.o src/Network/DataPacketFactory.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Network
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Network/DataPacketFactory.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/DataPacketFactory_nomain.o src/Network/DataPacketFactory.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Network/DataPacketFactory.o ${OBJECTDIR}/src/Network/DataPacketFactory_nomain.o;\
	fi

${OBJECTDIR}/src/Network/NetworkManager_nomain.o: ${OBJECTDIR}/src/Network/NetworkManager.o src/Network/NetworkManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Network
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Network/NetworkManager.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/NetworkManager_nomain.o src/Network/NetworkManager.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Network/NetworkManager.o ${OBJECTDIR}/src/Network/NetworkManager_nomain.o;\
	fi

${OBJECTDIR}/src/Network/NetworkManagerUpdater_nomain.o: ${OBJECTDIR}/src/Network/NetworkManagerUpdater.o src/Network/NetworkManagerUpdater.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Network
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Network/NetworkManagerUpdater.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/NetworkManagerUpdater_nomain.o src/Network/NetworkManagerUpdater.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Network/NetworkManagerUpdater.o ${OBJECTDIR}/src/Network/NetworkManagerUpdater_nomain.o;\
	fi

${OBJECTDIR}/src/Network/NetworkSystem_nomain.o: ${OBJECTDIR}/src/Network/NetworkSystem.o src/Network/NetworkSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Network
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Network/NetworkSystem.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/NetworkSystem_nomain.o src/Network/NetworkSystem.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Network/NetworkSystem.o ${OBJECTDIR}/src/Network/NetworkSystem_nomain.o;\
	fi

${OBJECTDIR}/src/Network/SimpleDataPacketPool_nomain.o: ${OBJECTDIR}/src/Network/SimpleDataPacketPool.o src/Network/SimpleDataPacketPool.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Network
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Network/SimpleDataPacketPool.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/SimpleDataPacketPool_nomain.o src/Network/SimpleDataPacketPool.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Network/SimpleDataPacketPool.o ${OBJECTDIR}/src/Network/SimpleDataPacketPool_nomain.o;\
	fi

${OBJECTDIR}/src/Network/SocketNetworkSystem_nomain.o: ${OBJECTDIR}/src/Network/SocketNetworkSystem.o src/Network/SocketNetworkSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Network
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Network/SocketNetworkSystem.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/SocketNetworkSystem_nomain.o src/Network/SocketNetworkSystem.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Network/SocketNetworkSystem.o ${OBJECTDIR}/src/Network/SocketNetworkSystem_nomain.o;\
	fi

${OBJECTDIR}/src/Network/TCPConnection_nomain.o: ${OBJECTDIR}/src/Network/TCPConnection.o src/Network/TCPConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Network
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Network/TCPConnection.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/TCPConnection_nomain.o src/Network/TCPConnection.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Network/TCPConnection.o ${OBJECTDIR}/src/Network/TCPConnection_nomain.o;\
	fi

${OBJECTDIR}/src/Network/TLSConnection_nomain.o: ${OBJECTDIR}/src/Network/TLSConnection.o src/Network/TLSConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Network
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Network/TLSConnection.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/TLSConnection_nomain.o src/Network/TLSConnection.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Network/TLSConnection.o ${OBJECTDIR}/src/Network/TLSConnection_nomain.o;\
	fi

${OBJECTDIR}/src/Network/UDPConnection_nomain.o: ${OBJECTDIR}/src/Network/UDPConnection.o src/Network/UDPConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Network
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Network/UDPConnection.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/UDPConnection_nomain.o src/Network/UDPConnection.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Network/UDPConnection.o ${OBJECTDIR}/src/Network/UDPConnection_nomain.o;\
	fi

${OBJECTDIR}/src/Network/WebSocketConnection_nomain.o: ${OBJECTDIR}/src/Network/WebSocketConnection.o src/Network/WebSocketConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Network
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Network/WebSocketConnection.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/WebSocketConnection_nomain.o src/Network/WebSocketConnection.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Network/WebSocketConnection.o ${OBJECTDIR}/src/Network/WebSocketConnection_nomain.o;\
	fi

${OBJECTDIR}/src/Network/WebSocketNetworkSystem_nomain.o: ${OBJECTDIR}/src/Network/WebSocketNetworkSystem.o src/Network/WebSocketNetworkSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Network
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Network/WebSocketNetworkSystem.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network/WebSocketNetworkSystem_nomain.o src/Network/WebSocketNetworkSystem.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Network/WebSocketNetworkSystem.o ${OBJECTDIR}/src/Network/WebSocketNetworkSystem_nomain.o;\
	fi

${OBJECTDIR}/src/Physics/BulletDynamicMotionState_nomain.o: ${OBJECTDIR}/src/Physics/BulletDynamicMotionState.o src/Physics/BulletDynamicMotionState.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Physics/BulletDynamicMotionState.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletDynamicMotionState_nomain.o src/Physics/BulletDynamicMotionState.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Physics/BulletDynamicMotionState.o ${OBJECTDIR}/src/Physics/BulletDynamicMotionState_nomain.o;\
	fi

${OBJECTDIR}/src/Physics/BulletFactory_nomain.o: ${OBJECTDIR}/src/Physics/BulletFactory.o src/Physics/BulletFactory.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Physics/BulletFactory.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletFactory_nomain.o src/Physics/BulletFactory.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Physics/BulletFactory.o ${OBJECTDIR}/src/Physics/BulletFactory_nomain.o;\
	fi

${OBJECTDIR}/src/Physics/BulletKinematicMotionState_nomain.o: ${OBJECTDIR}/src/Physics/BulletKinematicMotionState.o src/Physics/BulletKinematicMotionState.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Physics/BulletKinematicMotionState.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletKinematicMotionState_nomain.o src/Physics/BulletKinematicMotionState.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Physics/BulletKinematicMotionState.o ${OBJECTDIR}/src/Physics/BulletKinematicMotionState_nomain.o;\
	fi

${OBJECTDIR}/src/Physics/BulletMeshDebugDrawer_nomain.o: ${OBJECTDIR}/src/Physics/BulletMeshDebugDrawer.o src/Physics/BulletMeshDebugDrawer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Physics/BulletMeshDebugDrawer.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletMeshDebugDrawer_nomain.o src/Physics/BulletMeshDebugDrawer.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Physics/BulletMeshDebugDrawer.o ${OBJECTDIR}/src/Physics/BulletMeshDebugDrawer_nomain.o;\
	fi

${OBJECTDIR}/src/Physics/BulletMeshPhysicsShape_nomain.o: ${OBJECTDIR}/src/Physics/BulletMeshPhysicsShape.o src/Physics/BulletMeshPhysicsShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Physics/BulletMeshPhysicsShape.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletMeshPhysicsShape_nomain.o src/Physics/BulletMeshPhysicsShape.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Physics/BulletMeshPhysicsShape.o ${OBJECTDIR}/src/Physics/BulletMeshPhysicsShape_nomain.o;\
	fi

${OBJECTDIR}/src/Physics/BulletMotionState_nomain.o: ${OBJECTDIR}/src/Physics/BulletMotionState.o src/Physics/BulletMotionState.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Physics/BulletMotionState.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletMotionState_nomain.o src/Physics/BulletMotionState.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Physics/BulletMotionState.o ${OBJECTDIR}/src/Physics/BulletMotionState_nomain.o;\
	fi

${OBJECTDIR}/src/Physics/BulletPhysicsShape_nomain.o: ${OBJECTDIR}/src/Physics/BulletPhysicsShape.o src/Physics/BulletPhysicsShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Physics/BulletPhysicsShape.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletPhysicsShape_nomain.o src/Physics/BulletPhysicsShape.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Physics/BulletPhysicsShape.o ${OBJECTDIR}/src/Physics/BulletPhysicsShape_nomain.o;\
	fi

${OBJECTDIR}/src/Physics/BulletPhysicsWorld_nomain.o: ${OBJECTDIR}/src/Physics/BulletPhysicsWorld.o src/Physics/BulletPhysicsWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Physics/BulletPhysicsWorld.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletPhysicsWorld_nomain.o src/Physics/BulletPhysicsWorld.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Physics/BulletPhysicsWorld.o ${OBJECTDIR}/src/Physics/BulletPhysicsWorld_nomain.o;\
	fi

${OBJECTDIR}/src/Physics/BulletRigidBody_nomain.o: ${OBJECTDIR}/src/Physics/BulletRigidBody.o src/Physics/BulletRigidBody.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Physics/BulletRigidBody.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/BulletRigidBody_nomain.o src/Physics/BulletRigidBody.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Physics/BulletRigidBody.o ${OBJECTDIR}/src/Physics/BulletRigidBody_nomain.o;\
	fi

${OBJECTDIR}/src/Physics/MotionState_nomain.o: ${OBJECTDIR}/src/Physics/MotionState.o src/Physics/MotionState.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Physics/MotionState.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/MotionState_nomain.o src/Physics/MotionState.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Physics/MotionState.o ${OBJECTDIR}/src/Physics/MotionState_nomain.o;\
	fi

${OBJECTDIR}/src/Physics/PhysicsBody_nomain.o: ${OBJECTDIR}/src/Physics/PhysicsBody.o src/Physics/PhysicsBody.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Physics/PhysicsBody.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/PhysicsBody_nomain.o src/Physics/PhysicsBody.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Physics/PhysicsBody.o ${OBJECTDIR}/src/Physics/PhysicsBody_nomain.o;\
	fi

${OBJECTDIR}/src/Physics/PhysicsShape_nomain.o: ${OBJECTDIR}/src/Physics/PhysicsShape.o src/Physics/PhysicsShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Physics/PhysicsShape.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/PhysicsShape_nomain.o src/Physics/PhysicsShape.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Physics/PhysicsShape.o ${OBJECTDIR}/src/Physics/PhysicsShape_nomain.o;\
	fi

${OBJECTDIR}/src/Physics/PhysicsWorld_nomain.o: ${OBJECTDIR}/src/Physics/PhysicsWorld.o src/Physics/PhysicsWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Physics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Physics/PhysicsWorld.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Physics/PhysicsWorld_nomain.o src/Physics/PhysicsWorld.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Physics/PhysicsWorld.o ${OBJECTDIR}/src/Physics/PhysicsWorld_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/GL/FragmentShaderManager_nomain.o: ${OBJECTDIR}/src/Platforms/GL/FragmentShaderManager.o src/Platforms/GL/FragmentShaderManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/GL
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/GL/FragmentShaderManager.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/GL/FragmentShaderManager_nomain.o src/Platforms/GL/FragmentShaderManager.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/GL/FragmentShaderManager.o ${OBJECTDIR}/src/Platforms/GL/FragmentShaderManager_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/GL/VertexShaderManager_nomain.o: ${OBJECTDIR}/src/Platforms/GL/VertexShaderManager.o src/Platforms/GL/VertexShaderManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/GL
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/GL/VertexShaderManager.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/GL/VertexShaderManager_nomain.o src/Platforms/GL/VertexShaderManager.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/GL/VertexShaderManager.o ${OBJECTDIR}/src/Platforms/GL/VertexShaderManager_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/GTK/Platform_nomain.o: ${OBJECTDIR}/src/Platforms/GTK/Platform.o src/Platforms/GTK/Platform.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/GTK
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/GTK/Platform.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/GTK/Platform_nomain.o src/Platforms/GTK/Platform.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/GTK/Platform.o ${OBJECTDIR}/src/Platforms/GTK/Platform_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/Qt/Platform_nomain.o: ${OBJECTDIR}/src/Platforms/Qt/Platform.o src/Platforms/Qt/Platform.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Qt
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/Qt/Platform.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Qt/Platform_nomain.o src/Platforms/Qt/Platform.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/Qt/Platform.o ${OBJECTDIR}/src/Platforms/Qt/Platform_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/Qt/QtExecutionModel_nomain.o: ${OBJECTDIR}/src/Platforms/Qt/QtExecutionModel.o src/Platforms/Qt/QtExecutionModel.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Qt
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/Qt/QtExecutionModel.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Qt/QtExecutionModel_nomain.o src/Platforms/Qt/QtExecutionModel.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/Qt/QtExecutionModel.o ${OBJECTDIR}/src/Platforms/Qt/QtExecutionModel_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/Qt/QtKeyboard_nomain.o: ${OBJECTDIR}/src/Platforms/Qt/QtKeyboard.o src/Platforms/Qt/QtKeyboard.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Qt
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/Qt/QtKeyboard.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Qt/QtKeyboard_nomain.o src/Platforms/Qt/QtKeyboard.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/Qt/QtKeyboard.o ${OBJECTDIR}/src/Platforms/Qt/QtKeyboard_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/Qt/QtMouse_nomain.o: ${OBJECTDIR}/src/Platforms/Qt/QtMouse.o src/Platforms/Qt/QtMouse.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Qt
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/Qt/QtMouse.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Qt/QtMouse_nomain.o src/Platforms/Qt/QtMouse.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/Qt/QtMouse.o ${OBJECTDIR}/src/Platforms/Qt/QtMouse_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/Qt/QtWindow_nomain.o: ${OBJECTDIR}/src/Platforms/Qt/QtWindow.o src/Platforms/Qt/QtWindow.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Qt
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/Qt/QtWindow.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Qt/QtWindow_nomain.o src/Platforms/Qt/QtWindow.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/Qt/QtWindow.o ${OBJECTDIR}/src/Platforms/Qt/QtWindow_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/Wii/CPUTimer_nomain.o: ${OBJECTDIR}/src/Platforms/Wii/CPUTimer.o src/Platforms/Wii/CPUTimer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/Wii/CPUTimer.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/CPUTimer_nomain.o src/Platforms/Wii/CPUTimer.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/Wii/CPUTimer.o ${OBJECTDIR}/src/Platforms/Wii/CPUTimer_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/Wii/GXRenderTarget_nomain.o: ${OBJECTDIR}/src/Platforms/Wii/GXRenderTarget.o src/Platforms/Wii/GXRenderTarget.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/Wii/GXRenderTarget.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/GXRenderTarget_nomain.o src/Platforms/Wii/GXRenderTarget.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/Wii/GXRenderTarget.o ${OBJECTDIR}/src/Platforms/Wii/GXRenderTarget_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/Wii/GXTexture_nomain.o: ${OBJECTDIR}/src/Platforms/Wii/GXTexture.o src/Platforms/Wii/GXTexture.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/Wii/GXTexture.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/GXTexture_nomain.o src/Platforms/Wii/GXTexture.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/Wii/GXTexture.o ${OBJECTDIR}/src/Platforms/Wii/GXTexture_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/Wii/LWPMutex_nomain.o: ${OBJECTDIR}/src/Platforms/Wii/LWPMutex.o src/Platforms/Wii/LWPMutex.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/Wii/LWPMutex.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/LWPMutex_nomain.o src/Platforms/Wii/LWPMutex.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/Wii/LWPMutex.o ${OBJECTDIR}/src/Platforms/Wii/LWPMutex_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/Wii/LWPThread_nomain.o: ${OBJECTDIR}/src/Platforms/Wii/LWPThread.o src/Platforms/Wii/LWPThread.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/Wii/LWPThread.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/LWPThread_nomain.o src/Platforms/Wii/LWPThread.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/Wii/LWPThread.o ${OBJECTDIR}/src/Platforms/Wii/LWPThread_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/Wii/Platform_nomain.o: ${OBJECTDIR}/src/Platforms/Wii/Platform.o src/Platforms/Wii/Platform.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/Wii/Platform.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/Platform_nomain.o src/Platforms/Wii/Platform.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/Wii/Platform.o ${OBJECTDIR}/src/Platforms/Wii/Platform_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/Wii/SDFileSystemSource_nomain.o: ${OBJECTDIR}/src/Platforms/Wii/SDFileSystemSource.o src/Platforms/Wii/SDFileSystemSource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/Wii/SDFileSystemSource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/SDFileSystemSource_nomain.o src/Platforms/Wii/SDFileSystemSource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/Wii/SDFileSystemSource.o ${OBJECTDIR}/src/Platforms/Wii/SDFileSystemSource_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/Wii/SystemInformation_nomain.o: ${OBJECTDIR}/src/Platforms/Wii/SystemInformation.o src/Platforms/Wii/SystemInformation.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/Wii/SystemInformation.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/SystemInformation_nomain.o src/Platforms/Wii/SystemInformation.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/Wii/SystemInformation.o ${OBJECTDIR}/src/Platforms/Wii/SystemInformation_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/Wii/USBFileSystemSource_nomain.o: ${OBJECTDIR}/src/Platforms/Wii/USBFileSystemSource.o src/Platforms/Wii/USBFileSystemSource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/Wii/USBFileSystemSource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/USBFileSystemSource_nomain.o src/Platforms/Wii/USBFileSystemSource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/Wii/USBFileSystemSource.o ${OBJECTDIR}/src/Platforms/Wii/USBFileSystemSource_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/Wii/WiiSystemInput_nomain.o: ${OBJECTDIR}/src/Platforms/Wii/WiiSystemInput.o src/Platforms/Wii/WiiSystemInput.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/Wii/WiiSystemInput.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/WiiSystemInput_nomain.o src/Platforms/Wii/WiiSystemInput.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/Wii/WiiSystemInput.o ${OBJECTDIR}/src/Platforms/Wii/WiiSystemInput_nomain.o;\
	fi

${OBJECTDIR}/src/Platforms/Wii/Wiimote_nomain.o: ${OBJECTDIR}/src/Platforms/Wii/Wiimote.o src/Platforms/Wii/Wiimote.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Platforms/Wii
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Platforms/Wii/Wiimote.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Platforms/Wii/Wiimote_nomain.o src/Platforms/Wii/Wiimote.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Platforms/Wii/Wiimote.o ${OBJECTDIR}/src/Platforms/Wii/Wiimote_nomain.o;\
	fi

${OBJECTDIR}/src/Resource/3dsReader_nomain.o: ${OBJECTDIR}/src/Resource/3dsReader.o src/Resource/3dsReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Resource/3dsReader.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/3dsReader_nomain.o src/Resource/3dsReader.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Resource/3dsReader.o ${OBJECTDIR}/src/Resource/3dsReader_nomain.o;\
	fi

${OBJECTDIR}/src/Resource/BitmapLoader_nomain.o: ${OBJECTDIR}/src/Resource/BitmapLoader.o src/Resource/BitmapLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Resource/BitmapLoader.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/BitmapLoader_nomain.o src/Resource/BitmapLoader.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Resource/BitmapLoader.o ${OBJECTDIR}/src/Resource/BitmapLoader_nomain.o;\
	fi

${OBJECTDIR}/src/Resource/FontManager_nomain.o: ${OBJECTDIR}/src/Resource/FontManager.o src/Resource/FontManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Resource/FontManager.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/FontManager_nomain.o src/Resource/FontManager.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Resource/FontManager.o ${OBJECTDIR}/src/Resource/FontManager_nomain.o;\
	fi

${OBJECTDIR}/src/Resource/JPEGLoader_nomain.o: ${OBJECTDIR}/src/Resource/JPEGLoader.o src/Resource/JPEGLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Resource/JPEGLoader.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/JPEGLoader_nomain.o src/Resource/JPEGLoader.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Resource/JPEGLoader.o ${OBJECTDIR}/src/Resource/JPEGLoader_nomain.o;\
	fi

${OBJECTDIR}/src/Resource/MaterialManager_nomain.o: ${OBJECTDIR}/src/Resource/MaterialManager.o src/Resource/MaterialManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Resource/MaterialManager.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/MaterialManager_nomain.o src/Resource/MaterialManager.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Resource/MaterialManager.o ${OBJECTDIR}/src/Resource/MaterialManager_nomain.o;\
	fi

${OBJECTDIR}/src/Resource/MeshManager_nomain.o: ${OBJECTDIR}/src/Resource/MeshManager.o src/Resource/MeshManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Resource/MeshManager.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/MeshManager_nomain.o src/Resource/MeshManager.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Resource/MeshManager.o ${OBJECTDIR}/src/Resource/MeshManager_nomain.o;\
	fi

${OBJECTDIR}/src/Resource/MeshReader_nomain.o: ${OBJECTDIR}/src/Resource/MeshReader.o src/Resource/MeshReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Resource/MeshReader.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/MeshReader_nomain.o src/Resource/MeshReader.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Resource/MeshReader.o ${OBJECTDIR}/src/Resource/MeshReader_nomain.o;\
	fi

${OBJECTDIR}/src/Resource/OggAudioSource_nomain.o: ${OBJECTDIR}/src/Resource/OggAudioSource.o src/Resource/OggAudioSource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Resource/OggAudioSource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/OggAudioSource_nomain.o src/Resource/OggAudioSource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Resource/OggAudioSource.o ${OBJECTDIR}/src/Resource/OggAudioSource_nomain.o;\
	fi

${OBJECTDIR}/src/Resource/PNGLoader_nomain.o: ${OBJECTDIR}/src/Resource/PNGLoader.o src/Resource/PNGLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Resource/PNGLoader.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/PNGLoader_nomain.o src/Resource/PNGLoader.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Resource/PNGLoader.o ${OBJECTDIR}/src/Resource/PNGLoader_nomain.o;\
	fi

${OBJECTDIR}/src/Resource/RIFFReader_nomain.o: ${OBJECTDIR}/src/Resource/RIFFReader.o src/Resource/RIFFReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Resource/RIFFReader.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/RIFFReader_nomain.o src/Resource/RIFFReader.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Resource/RIFFReader.o ${OBJECTDIR}/src/Resource/RIFFReader_nomain.o;\
	fi

${OBJECTDIR}/src/Resource/ResourceManager_nomain.o: ${OBJECTDIR}/src/Resource/ResourceManager.o src/Resource/ResourceManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Resource/ResourceManager.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/ResourceManager_nomain.o src/Resource/ResourceManager.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Resource/ResourceManager.o ${OBJECTDIR}/src/Resource/ResourceManager_nomain.o;\
	fi

${OBJECTDIR}/src/Resource/SkeletonManager_nomain.o: ${OBJECTDIR}/src/Resource/SkeletonManager.o src/Resource/SkeletonManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Resource/SkeletonManager.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/SkeletonManager_nomain.o src/Resource/SkeletonManager.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Resource/SkeletonManager.o ${OBJECTDIR}/src/Resource/SkeletonManager_nomain.o;\
	fi

${OBJECTDIR}/src/Resource/SkeletonReader_nomain.o: ${OBJECTDIR}/src/Resource/SkeletonReader.o src/Resource/SkeletonReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Resource/SkeletonReader.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/SkeletonReader_nomain.o src/Resource/SkeletonReader.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Resource/SkeletonReader.o ${OBJECTDIR}/src/Resource/SkeletonReader_nomain.o;\
	fi

${OBJECTDIR}/src/Resource/TextureLoader_nomain.o: ${OBJECTDIR}/src/Resource/TextureLoader.o src/Resource/TextureLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Resource/TextureLoader.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/TextureLoader_nomain.o src/Resource/TextureLoader.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Resource/TextureLoader.o ${OBJECTDIR}/src/Resource/TextureLoader_nomain.o;\
	fi

${OBJECTDIR}/src/Resource/TextureManager_nomain.o: ${OBJECTDIR}/src/Resource/TextureManager.o src/Resource/TextureManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Resource/TextureManager.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/TextureManager_nomain.o src/Resource/TextureManager.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Resource/TextureManager.o ${OBJECTDIR}/src/Resource/TextureManager_nomain.o;\
	fi

${OBJECTDIR}/src/Resource/WavAudioSource_nomain.o: ${OBJECTDIR}/src/Resource/WavAudioSource.o src/Resource/WavAudioSource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Resource/WavAudioSource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/WavAudioSource_nomain.o src/Resource/WavAudioSource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Resource/WavAudioSource.o ${OBJECTDIR}/src/Resource/WavAudioSource_nomain.o;\
	fi

${OBJECTDIR}/src/Resource/XMAudioSource_nomain.o: ${OBJECTDIR}/src/Resource/XMAudioSource.o src/Resource/XMAudioSource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Resource
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Resource/XMAudioSource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resource/XMAudioSource_nomain.o src/Resource/XMAudioSource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Resource/XMAudioSource.o ${OBJECTDIR}/src/Resource/XMAudioSource_nomain.o;\
	fi

${OBJECTDIR}/src/Shell/Shell_nomain.o: ${OBJECTDIR}/src/Shell/Shell.o src/Shell/Shell.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Shell
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Shell/Shell.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Shell/Shell_nomain.o src/Shell/Shell.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Shell/Shell.o ${OBJECTDIR}/src/Shell/Shell_nomain.o;\
	fi

${OBJECTDIR}/src/Tile/TMXLoader_nomain.o: ${OBJECTDIR}/src/Tile/TMXLoader.o src/Tile/TMXLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Tile
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Tile/TMXLoader.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Tile/TMXLoader_nomain.o src/Tile/TMXLoader.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Tile/TMXLoader.o ${OBJECTDIR}/src/Tile/TMXLoader_nomain.o;\
	fi

${OBJECTDIR}/src/Tile/TileLayer_nomain.o: ${OBJECTDIR}/src/Tile/TileLayer.o src/Tile/TileLayer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Tile
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Tile/TileLayer.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Tile/TileLayer_nomain.o src/Tile/TileLayer.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Tile/TileLayer.o ${OBJECTDIR}/src/Tile/TileLayer_nomain.o;\
	fi

${OBJECTDIR}/src/Tile/TileLayerMesh_nomain.o: ${OBJECTDIR}/src/Tile/TileLayerMesh.o src/Tile/TileLayerMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Tile
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Tile/TileLayerMesh.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Tile/TileLayerMesh_nomain.o src/Tile/TileLayerMesh.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Tile/TileLayerMesh.o ${OBJECTDIR}/src/Tile/TileLayerMesh_nomain.o;\
	fi

${OBJECTDIR}/src/Tile/TileMap_nomain.o: ${OBJECTDIR}/src/Tile/TileMap.o src/Tile/TileMap.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Tile
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Tile/TileMap.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Tile/TileMap_nomain.o src/Tile/TileMap.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Tile/TileMap.o ${OBJECTDIR}/src/Tile/TileMap_nomain.o;\
	fi

${OBJECTDIR}/src/Tile/TileSet_nomain.o: ${OBJECTDIR}/src/Tile/TileSet.o src/Tile/TileSet.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Tile
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Tile/TileSet.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Tile/TileSet_nomain.o src/Tile/TileSet.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Tile/TileSet.o ${OBJECTDIR}/src/Tile/TileSet_nomain.o;\
	fi

${OBJECTDIR}/src/Util/Configuration_nomain.o: ${OBJECTDIR}/src/Util/Configuration.o src/Util/Configuration.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Util
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Util/Configuration.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Util/Configuration_nomain.o src/Util/Configuration.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Util/Configuration.o ${OBJECTDIR}/src/Util/Configuration_nomain.o;\
	fi

${OBJECTDIR}/src/Util/ContextSwitcher_nomain.o: ${OBJECTDIR}/src/Util/ContextSwitcher.o src/Util/ContextSwitcher.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Util
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Util/ContextSwitcher.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Util/ContextSwitcher_nomain.o src/Util/ContextSwitcher.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Util/ContextSwitcher.o ${OBJECTDIR}/src/Util/ContextSwitcher_nomain.o;\
	fi

${OBJECTDIR}/src/Util/Parsers_nomain.o: ${OBJECTDIR}/src/Util/Parsers.o src/Util/Parsers.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Util
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Util/Parsers.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Util/Parsers_nomain.o src/Util/Parsers.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Util/Parsers.o ${OBJECTDIR}/src/Util/Parsers_nomain.o;\
	fi

${OBJECTDIR}/src/Util/RegEx_nomain.o: ${OBJECTDIR}/src/Util/RegEx.o src/Util/RegEx.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Util
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Util/RegEx.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Util/RegEx_nomain.o src/Util/RegEx.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Util/RegEx.o ${OBJECTDIR}/src/Util/RegEx_nomain.o;\
	fi

${OBJECTDIR}/src/Util/ScenePicker_nomain.o: ${OBJECTDIR}/src/Util/ScenePicker.o src/Util/ScenePicker.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Util
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Util/ScenePicker.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Util/ScenePicker_nomain.o src/Util/ScenePicker.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Util/ScenePicker.o ${OBJECTDIR}/src/Util/ScenePicker_nomain.o;\
	fi

${OBJECTDIR}/src/Util/StringSetter_nomain.o: ${OBJECTDIR}/src/Util/StringSetter.o src/Util/StringSetter.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Util
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Util/StringSetter.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Util/StringSetter_nomain.o src/Util/StringSetter.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Util/StringSetter.o ${OBJECTDIR}/src/Util/StringSetter_nomain.o;\
	fi

${OBJECTDIR}/src/Util/StringUtils_nomain.o: ${OBJECTDIR}/src/Util/StringUtils.o src/Util/StringUtils.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Util
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Util/StringUtils.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Util/StringUtils_nomain.o src/Util/StringUtils.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Util/StringUtils.o ${OBJECTDIR}/src/Util/StringUtils_nomain.o;\
	fi

${OBJECTDIR}/src/Util/Texture_nomain.o: ${OBJECTDIR}/src/Util/Texture.o src/Util/Texture.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Util
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Util/Texture.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Util/Texture_nomain.o src/Util/Texture.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Util/Texture.o ${OBJECTDIR}/src/Util/Texture_nomain.o;\
	fi

${OBJECTDIR}/src/generated/Font.pb_nomain.o: ${OBJECTDIR}/src/generated/Font.pb.o src/generated/Font.pb.cc 
	${MKDIR} -p ${OBJECTDIR}/src/generated
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/generated/Font.pb.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/generated/Font.pb_nomain.o src/generated/Font.pb.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/generated/Font.pb.o ${OBJECTDIR}/src/generated/Font.pb_nomain.o;\
	fi

${OBJECTDIR}/src/generated/GUI.pb_nomain.o: ${OBJECTDIR}/src/generated/GUI.pb.o src/generated/GUI.pb.cc 
	${MKDIR} -p ${OBJECTDIR}/src/generated
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/generated/GUI.pb.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/generated/GUI.pb_nomain.o src/generated/GUI.pb.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/generated/GUI.pb.o ${OBJECTDIR}/src/generated/GUI.pb_nomain.o;\
	fi

${OBJECTDIR}/src/generated/MappedInputDevice.pb_nomain.o: ${OBJECTDIR}/src/generated/MappedInputDevice.pb.o src/generated/MappedInputDevice.pb.cc 
	${MKDIR} -p ${OBJECTDIR}/src/generated
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/generated/MappedInputDevice.pb.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/generated/MappedInputDevice.pb_nomain.o src/generated/MappedInputDevice.pb.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/generated/MappedInputDevice.pb.o ${OBJECTDIR}/src/generated/MappedInputDevice.pb_nomain.o;\
	fi

${OBJECTDIR}/src/generated/Material.pb_nomain.o: ${OBJECTDIR}/src/generated/Material.pb.o src/generated/Material.pb.cc 
	${MKDIR} -p ${OBJECTDIR}/src/generated
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/generated/Material.pb.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -DBOOST_REGEX_NO_LIB -DECHO_JPEG_SUPPORT_ENABLED -DECHO_PLATFORM_NINTENDO_WII -DECHO_PNG_SUPPORT_ENABLED -DGEKKO -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY -Iinclude -Iinclude/echo/generated -Idependencies/opt/wii/include/freetype2 -Idependencies/opt/wii/include/bullet -I${DEVKITPRO}/libogc/include -Idependencies/opt/wii/include -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/generated/Material.pb_nomain.o src/generated/Material.pb.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/generated/Material.pb.o ${OBJECTDIR}/src/generated/Material.pb_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f24 || true; \
	    ${TESTDIR}/TestFiles/f7 || true; \
	    ${TESTDIR}/TestFiles/f6 || true; \
	    ${TESTDIR}/TestFiles/f31 || true; \
	    ${TESTDIR}/TestFiles/f12 || true; \
	    ${TESTDIR}/TestFiles/f33 || true; \
	    ${TESTDIR}/TestFiles/f29 || true; \
	    ${TESTDIR}/TestFiles/f8 || true; \
	    ${TESTDIR}/TestFiles/f3 || true; \
	    ${TESTDIR}/TestFiles/f20 || true; \
	    ${TESTDIR}/TestFiles/f5 || true; \
	    ${TESTDIR}/TestFiles/f15 || true; \
	    ${TESTDIR}/TestFiles/f27 || true; \
	    ${TESTDIR}/TestFiles/f18 || true; \
	    ${TESTDIR}/TestFiles/f22 || true; \
	    ${TESTDIR}/TestFiles/f11 || true; \
	    ${TESTDIR}/TestFiles/f34 || true; \
	    ${TESTDIR}/TestFiles/f32 || true; \
	    ${TESTDIR}/TestFiles/f35 || true; \
	    ${TESTDIR}/TestFiles/f2 || true; \
	    ${TESTDIR}/TestFiles/f21 || true; \
	    ${TESTDIR}/TestFiles/f25 || true; \
	    ${TESTDIR}/TestFiles/f19 || true; \
	    ${TESTDIR}/TestFiles/f17 || true; \
	    ${TESTDIR}/TestFiles/f14 || true; \
	    ${TESTDIR}/TestFiles/f30 || true; \
	    ${TESTDIR}/TestFiles/f28 || true; \
	    ${TESTDIR}/TestFiles/f1 || true; \
	    ${TESTDIR}/TestFiles/f13 || true; \
	    ${TESTDIR}/TestFiles/f10 || true; \
	    ${TESTDIR}/TestFiles/f36 || true; \
	    ${TESTDIR}/TestFiles/f23 || true; \
	    ${TESTDIR}/TestFiles/f16 || true; \
	    ${TESTDIR}/TestFiles/f4 || true; \
	    ${TESTDIR}/TestFiles/f9 || true; \
	    ${TESTDIR}/TestFiles/f26 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
