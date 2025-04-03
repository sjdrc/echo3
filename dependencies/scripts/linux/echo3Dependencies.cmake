
#################################################################
# Provided dependencies

set(BOOST_ROOT "${DEPS_ROOT}") # dumb hack until we update boost to 1.71
find_package(Boost REQUIRED chrono date_time filesystem random regex system thread timer)

if(BUILD_WITH_EFSW)
	find_package(efsw CONFIG REQUIRED)
endif()


# --define-prefix will cause the prefix in the .pc file to be set to the grandparent of the .pc file. This allows us to
# provide .pc files in our deps opt folder and it will use ${DEPS_ROOT} for libs and includes. This can't be set for
# dependencies on the system since the system configuration can vary from what pkg-config would expect.

set(PKG_CONFIG_EXECUTABLE pkg-config --define-prefix)
set(ENV{PKG_CONFIG_PATH} "${DEPS_ROOT}/lib/pkgconfig")
find_package(PkgConfig REQUIRED)
pkg_check_modules(zlib REQUIRED IMPORTED_TARGET zlib)
pkg_check_modules(bullet REQUIRED IMPORTED_TARGET bullet)

if(BUILD_WITH_FREETYPE)
	pkg_check_modules(freetype2 REQUIRED IMPORTED_TARGET freetype2)
endif()
if(BUILD_WITH_SSL)
    pkg_check_modules(libssl REQUIRED IMPORTED_TARGET libssl)
    pkg_check_modules(libcrypto REQUIRED IMPORTED_TARGET libcrypto)
    pkg_check_modules(libtls REQUIRED IMPORTED_TARGET libtls)
endif()
if(BUILD_WITH_OGG)
    pkg_check_modules(ogg REQUIRED IMPORTED_TARGET ogg)
endif()
if(BUILD_WITH_TMXPARSER)
    pkg_check_modules(tmxparser REQUIRED IMPORTED_TARGET tmxparser)
endif()
if(BUILD_WITH_VORBIS)
    pkg_check_modules(vorbis REQUIRED IMPORTED_TARGET vorbis)
    pkg_check_modules(vorbisfile REQUIRED IMPORTED_TARGET vorbisfile)
endif()
if(BUILD_WITH_OPENGL)
	find_package(glew REQUIRED)
endif()

if(BUILD_WITH_JPEG)
	pkg_check_modules(libjpeg REQUIRED IMPORTED_TARGET libjpeg)
endif()

#################################################################
# System dependencies
set(PKG_CONFIG_EXECUTABLE pkg-config --dont-define-prefix)
if(BUILD_WITH_PNG)
	pkg_check_modules(libpng REQUIRED IMPORTED_TARGET libpng)
endif()

if (ECHO_UI_FRAMEWORK STREQUAL "GTK")
	pkg_check_modules(gtkmm REQUIRED IMPORTED_TARGET gtkmm-2.4)
	pkg_check_modules(gtkglextmm REQUIRED IMPORTED_TARGET gtkglextmm-1.2)
	pkg_check_modules(x11 REQUIRED IMPORTED_TARGET x11)
endif()
