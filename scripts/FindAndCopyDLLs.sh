#!/bin/bash
# This is a custom version of the script found here: https://blog.rubenwardy.com/2018/05/07/mingw-copy-dlls/
# This version of the script:
#	- Uses an ignore list to not display output on Windows dlls
#	- Includes Echo's search path
#	- Allows modification of the toolchain prefix and version by setting environment variables
#
# To use this script:
#
# 	$0 binary destination
#
# binary is the dll or exe to scan for dependencies
# destination is the destination directory to copy the dlls to

TOOLCHAIN_PREFIX=${TOOLCHAIN_PREFIX:-"x86_64-w64-mingw32"}
TOOLCHAIN_VERSION=${TOOLCHAIN_VERSION:-"9.3"}
PLATFORM=${PLATFORM:-windowscross}
DESTINATION=$2

ignoreList=(
	"KERNEL32.dll"
	"msvcrt.dll"
	"GDI32.dll"
	"ADVAPI32.dll"
	"OPENGL32.dll"
	"WS2_32.dll"
	"USERENV.dll"
	"ole32.dll"
	"SHELL32.dll"
	"USER32.dll"
	"WINMM.dll"
)

paths=(
	"/usr/local/mingw64/bin"
	"/usr/local/mingw64/bin/x64"
	"/usr/${TOOLCHAIN_PREFIX}/bin"
	"/usr/lib/gcc/${TOOLCHAIN_PREFIX}/${TOOLCHAIN_VERSION}-posix"
	"/usr/${TOOLCHAIN_PREFIX}/lib"
	"${ECHO_ENGINE_INSTALL_DIR}/${PLATFORM}/lib"
	"${ECHO_ENGINE_INSTALL_DIR}/${PLATFORM}/bin"
)

function findAndCopyDLL() {
	for i in "${paths[@]}"
	do
		FILE="$i/$1"
		if [ -f $FILE ]; then
            if ! [ -f "${DESTINATION}/$1" ]; then
                cp "${FILE}" "${DESTINATION}"
                echo "Found $1 in $i"
                copyForOBJ $FILE
            #else
                #echo "Skipping $filename (already exists)"
            fi
			return 0
        else
            # Is it part of the ignore list
            for x in ${ignoreList[@]}; do
                if [ $x = "$1" ]; then
                    return 0
                fi
            done
		fi
	done

	return 1
}

function copyForOBJ() {
	dlls=`${TOOLCHAIN_PREFIX}-objdump -p $1 | grep 'DLL Name:' | sed -e "s/\t*DLL Name: //g"`
	while read -r filename; do
		findAndCopyDLL $filename || echo "Unable to find $filename"
	done <<< "$dlls"
}

echo "Echo directory: ${ECHO_ENGINE_INSTALL_DIR}/${PLATFORM}"
echo "Binary: $1"
echo "Destination: ${DESTINATION}"
echo "Search paths:"
for i in "${paths[@]}"
do
	echo "    $i"
done

copyForOBJ $1 $2
echo "Finished"
