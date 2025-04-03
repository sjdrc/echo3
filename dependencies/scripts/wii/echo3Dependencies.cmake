
#################################################################
# Provided dependencies
set(BOOST_ROOT "${DEPS_ROOT}") # dumb hack until we update boost to 1.71
set(Boost_INCLUDE_DIR "${BOOST_ROOT}/include")
set(Boost_LIBRARY_DIR "${BOOST_ROOT}/lib")
message("Attempting to find boost with BOOST_ROOT=" ${BOOST_ROOT})

find_package(Boost REQUIRED regex)
if(BUILD_WITH_FREETYPE)
	find_package(freetype REQUIRED)
endif()
#pkg_check_modules(freetype2 REQUIRED IMPORTED_TARGET freetype2)

# --define-prefix will cause the prefix in the .pc file to be set to the grandparent of the .pc file. This allows us to
# provide .pc files in our deps opt folder and it will use ${DEPS_ROOT} for libs and includes. This can't be set for
# dependencies on the system since the system configuration can vary from what pkg-config would expect.

set(PKG_CONFIG_EXECUTABLE pkg-config --define-prefix)
set(ENV{PKG_CONFIG_PATH} "${DEPS_ROOT}/lib/pkgconfig")
find_package(PkgConfig REQUIRED)
#pkg_check_modules(freetype2 REQUIRED IMPORTED_TARGET freetype2)
pkg_check_modules(bullet REQUIRED IMPORTED_TARGET bullet)
pkg_check_modules(libmodplug REQUIRED IMPORTED_TARGET libmodplug)
#pkg_check_modules(libssl REQUIRED IMPORTED_TARGET libssl)
#pkg_check_modules(libcrypto REQUIRED IMPORTED_TARGET libcrypto)
#pkg_check_modules(libtls REQUIRED IMPORTED_TARGET libtls)
pkg_check_modules(ogg REQUIRED IMPORTED_TARGET ogg)
pkg_check_modules(tmxparser REQUIRED IMPORTED_TARGET tmxparser)
pkg_check_modules(vorbis REQUIRED IMPORTED_TARGET vorbis)
pkg_check_modules(vorbisfile REQUIRED IMPORTED_TARGET vorbisfile)

pkg_check_modules(zlib REQUIRED IMPORTED_TARGET zlib)

if(BUILD_WITH_JPEG)
	pkg_check_modules(libjpeg REQUIRED IMPORTED_TARGET libjpeg)
endif()

if(BUILD_WITH_PNG)
	pkg_check_modules(libpng REQUIRED IMPORTED_TARGET libpng)
endif()

#################################################################
# System dependencies - None, so we don't reset this
#set(PKG_CONFIG_EXECUTABLE pkg-config --dont-define-prefix)

#Really crap hack that requires linux to be built first.
message("DEPS_ROOT/bin/protoc " ${DEPS_ROOT}/../linux/bin/protoc)

find_program(PROTOC_EXEC ${DEPS_ROOT}/../linux/bin/protoc REQUIRED)
add_executable(protobuf::protoc IMPORTED GLOBAL)
set_target_properties(protobuf::protoc PROPERTIES
	IMPORTED_LOCATION ${PROTOC_EXEC}
)

set(ECHO_SSL_ENABLED OFF)
