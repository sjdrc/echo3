#! /bin/bash
set -e

PLATFORM=${1:-linux}
# JOBS = NCores - 1
JOBS=${2:-$(($(nproc)-1))}

echo "Building Echo3 for ${PLATFORM}"
BUILD_DIR="build-${PLATFORM}"
echo Running cmakeplatform ${PLATFORM}
./cmakeplatform ${PLATFORM}
cd ${BUILD_DIR}
echo 
echo 
echo "Running make... (${JOBS} parallel jobs)"
if [ -e "Makefile" ] ; then
	make -j${JOBS}
else
	ninja -j${JOBS}
fi
echo 
echo 
