# Cross-compiling requires CMake 2.6 or newer. To use it from build/, call it
# like this:
# cmake .. -DCMAKE_TOOLCHAIN_FILE=../XCompile.txt -DHOST=i686-pc-mingw32
# Where 'i686-pc-mingw32' is the host prefix for your cross-compiler. If you
# already have a toolchain file setup, you may use that instead of this file.

# the name of the target operating system
set(CMAKE_SYSTEM_NAME Generic)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER "$ENV{DEVKITPPC}/bin/powerpc-eabi-gcc")
set(CMAKE_CC_COMPILER "$ENV{DEVKITPPC}/bin/powerpc-eabi-gcc")
set(CMAKE_CXX_COMPILER "$ENV{DEVKITPPC}/bin/powerpc-eabi-g++")
set(CMAKE_AR "$ENV{DEVKITPPC}/bin/powerpc-eabi-gcc-ar")
set(CMAKE_RANLIB "$ENV{DEVKITPPC}/bin/powerpc-eabi-gcc-ranlib")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_C_FLAGS "-O0 -g -Wall -DGEKKO -DECHO_DEBUG -mrvl -mcpu=750 -meabi -mhard-float")
	set(CMAKE_CXX_FLAGS "-O0 -g -Wall -DGEKKO -DECHO_DEBUG -mrvl -mcpu=750 -meabi -mhard-float")
else()
	set(CMAKE_C_FLAGS "-O2 -Wall -DGEKKO -mrvl -mcpu=750 -meabi -mhard-float")
	set(CMAKE_CXX_FLAGS "-O2 -Wall -DGEKKO -mrvl -mcpu=750 -meabi -mhard-float")
endif()

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-DGEKKO -mrvl -mcpu=750 -meabi -mhard-float")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "-DGEKKO -mrvl -mcpu=750 -meabi -mhard-float")

set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS FALSE)

# here is the target environment located
#SET(CMAKE_FIND_ROOT_PATH "/usr/${HOST}")

# here is where stuff gets installed to
#SET(CMAKE_INSTALL_PREFIX "${CMAKE_FIND_ROOT_PATH}/usr" CACHE STRING "Install path prefix, prepended onto install directories." FORCE)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_SIZEOF_VOID_P "4")

# set env vars so that pkg-config will look in the appropriate directory for
# .pc files (as there seems to be no way to force using ${HOST}-pkg-config)
set(ENV{PKG_CONFIG_LIBDIR} "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
set(ENV{PKG_CONFIG_PATH} "")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
