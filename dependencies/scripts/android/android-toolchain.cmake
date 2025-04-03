SET(CMAKE_SYSTEM_NAME Generic)

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER "${HOST}-gcc")
SET(CMAKE_CXX_COMPILER "${HOST}-g++")

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

# set env vars so that pkg-config will look in the appropriate directory for
# .pc files (as there seems to be no way to force using ${HOST}-pkg-config)
set(ENV{PKG_CONFIG_LIBDIR} "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
set(ENV{PKG_CONFIG_PATH} "")

SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS true)

