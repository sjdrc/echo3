# Create imported target libogc::libogc
add_library(libogc::libogc STATIC IMPORTED)

set_target_properties(libogc::libogc PROPERTIES
	INTERFACE_COMPILE_DEFINITIONS ""
	INTERFACE_INCLUDE_DIRECTORIES "$ENV{DEVKITPRO}/libogc/include"
	INTERFACE_LINK_DIRECTORIES "$ENV{DEVKITPRO}/libogc/lib/wii"
	INTERFACE_LINK_LIBRARIES ""
	IMPORTED_LOCATION "$ENV{DEVKITPRO}/libogc/lib/wii/libogc.a"
	IMPORTED_LOCATION_RELEASE "$ENV{DEVKITPRO}/libogc/lib/wii/libogc.a"
	IMPORTED_LOCATION_DEBUG "$ENV{DEVKITPRO}/libogc/lib/wii/libogc.a"
)

add_library(libogc::libfat STATIC IMPORTED)

set_target_properties(libogc::libfat PROPERTIES
	INTERFACE_COMPILE_DEFINITIONS ""
	INTERFACE_INCLUDE_DIRECTORIES "$ENV{DEVKITPRO}/libogc/include"
	INTERFACE_LINK_DIRECTORIES "$ENV{DEVKITPRO}/libogc/lib/wii"
	INTERFACE_LINK_LIBRARIES ""
	IMPORTED_LOCATION "$ENV{DEVKITPRO}/libogc/lib/wii/libfat.a"
	IMPORTED_LOCATION_RELEASE "$ENV{DEVKITPRO}/libogc/lib/wii/libfat.a"
	IMPORTED_LOCATION_DEBUG "$ENV{DEVKITPRO}/libogc/lib/wii/libfat.a"
)

add_library(libogc::libwiiuse STATIC IMPORTED)

set_target_properties(libogc::libwiiuse PROPERTIES
	INTERFACE_COMPILE_DEFINITIONS ""
	INTERFACE_INCLUDE_DIRECTORIES "$ENV{DEVKITPRO}/libogc/include"
	INTERFACE_LINK_DIRECTORIES "$ENV{DEVKITPRO}/libogc/lib/wii"
	INTERFACE_LINK_LIBRARIES ""
	IMPORTED_LOCATION "$ENV{DEVKITPRO}/libogc/lib/wii/libwiiuse.a"
	IMPORTED_LOCATION_RELEASE "$ENV{DEVKITPRO}/libogc/lib/wii/libwiiuse.a"
	IMPORTED_LOCATION_DEBUG "$ENV{DEVKITPRO}/libogc/lib/wii/libwiiuse.a"
)

add_library(libogc::libbte STATIC IMPORTED)

set_target_properties(libogc::libbte PROPERTIES
	INTERFACE_COMPILE_DEFINITIONS ""
	INTERFACE_INCLUDE_DIRECTORIES "$ENV{DEVKITPRO}/libogc/include"
	INTERFACE_LINK_DIRECTORIES "$ENV{DEVKITPRO}/libogc/lib/wii"
	INTERFACE_LINK_LIBRARIES ""
	IMPORTED_LOCATION "$ENV{DEVKITPRO}/libogc/lib/wii/libbte.a"
	IMPORTED_LOCATION_RELEASE "$ENV{DEVKITPRO}/libogc/lib/wii/libbte.a"
	IMPORTED_LOCATION_DEBUG "$ENV{DEVKITPRO}/libogc/lib/wii/libbte.a"
)
