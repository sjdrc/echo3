#! /bin/bash
### -====================================-
### Echo Deb Packager
### -====================================-
###
### This script will generate a deb package that can be distributed.
###
### ./PackageAllAsDeb.sh platform
###
### platform - The platform to target, defaults to "linux".

usage()
{
	sed -n 's/^### \?//p' "$0"
}

if [ "$1" = "--help" ] || [ "$1" = "?" ]; then
	usage
	exit
fi

if [ "${ECHO_ENGINE_INSTALL_DIR}" = "" ]; then
	echo "Please specify the Echo install location:"
	echo "You can avoid this prompt by setting ECHO_ENGINE_INSTALL_DIR"
	read ECHO_ENGINE_INSTALL_DIR
	if ! [ -d "${ECHO_ENGINE_INSTALL_DIR}" ]; then
		echo "${ECHO_ENGINE_INSTALL_DIR} is not a directory"
		exit 1;
	fi
else
	echo "ECHO_ENGINE_INSTALL_DIR is: ${ECHO_ENGINE_INSTALL_DIR}"
fi

if ! command -v jq &> /dev/null
then
    echo "jq could not be found"
    sudo apt install jq
fi

VERSION=$(hg log -r "." --template "{latesttag}\n")
if [ "${VERSION}" = "null" ]; then
	VERSION=1.0
fi

PLATFORM=${1:-linux}
INSTALL_LOCATION=${INSTALL_LOCATION:-/opt/echo3}
REVISION=2
NAME=echo3-${PLATFORM}-standalone
ARCHITECTURE=amd64
MAINTAINER="Sean Tasker <sean@emblem.net.au>"
DEPENDENCIES_LIST=(
	build-essential
	cmake
	nasm
	curl
	libarchive-tools
)
	
DEPENDENCIES_LIST_linux=(
	libssl-dev
	libbluetooth-dev
	libqt5opengl5-dev
	qt5-qmake
	qt5-qmake-bin
	qtbase5-dev
	qtbase5-dev-tools
	libfreetype6-dev
	libpulse-dev
	libsigc++-2.0-dev
	libgtkglext1-dev
	libgtkmm-2.4-dev
	libgtkglextmm-x11-1.2-dev
	libdrm-dev
	libicu66
)

DEPENDENCIES_LIST_windowscross=(
	mingw-w64
)

declare -n DEPENDENCIES_LIST_platform="DEPENDENCIES_LIST_${1}"

DEPENDENCIES="${DEPENDENCIES_LIST[@]} ${DEPENDENCIES_LIST_platform[@]}"
DEPENDENCIES="${DEPENDENCIES// /, }"

PACKAGE_NAME=${NAME}_${VERSION}-${REVISION}_${ARCHITECTURE}

echo "About to build: ${PACKAGE_NAME}"
echo "Install location: ${INSTALL_LOCATION}"
INSTALL_LOCATION_RELATIVE=${INSTALL_LOCATION:1}
echo "Install relative: ${INSTALL_LOCATION_RELATIVE}"

if ! [ -e "${ECHO_ENGINE_INSTALL_DIR}/${PLATFORM}" ]; then
	echo "Echo built platform does not exist: ${ECHO_ENGINE_INSTALL_DIR}/${PLATFORM}"
	exit 1
fi

if [ -e ${PACKAGE_NAME} ]; then
rm -R ${PACKAGE_NAME}
fi

mkdir -p "${PACKAGE_NAME}/${INSTALL_LOCATION_RELATIVE}/${PLATFORM}"
mkdir -p "${PACKAGE_NAME}/${INSTALL_LOCATION_RELATIVE}/${PLATFORM}/config"
mkdir -p "${PACKAGE_NAME}/${INSTALL_LOCATION_RELATIVE}/scripts"
mkdir -p "${PACKAGE_NAME}/${INSTALL_LOCATION_RELATIVE}/templates"

echo "Copying built directory structure ${ECHO_ENGINE_INSTALL_DIR}/${PLATFORM}"
cp -R "${ECHO_ENGINE_INSTALL_DIR}/${PLATFORM}" "${PACKAGE_NAME}/${INSTALL_LOCATION_RELATIVE}"

echo "Copying platform config helper"
cp ../dependencies/scripts/${PLATFORM}/${PLATFORM}.config ${PACKAGE_NAME}/${INSTALL_LOCATION_RELATIVE}/${PLATFORM}/config

echo "Copying Echo scripts"
cp ../scripts/cmakeplatform ${PACKAGE_NAME}/${INSTALL_LOCATION_RELATIVE}/scripts

echo "Copying templates"
cp -R ../templates/cmake ${PACKAGE_NAME}/${INSTALL_LOCATION_RELATIVE}/templates

mkdir -p "${PACKAGE_NAME}/DEBIAN"

cat > ${PACKAGE_NAME}/DEBIAN/control <<EOF
Package: ${NAME}
Version: ${VERSION}
Architecture: ${ARCHITECTURE}
Maintainer: ${MAINTAINER}
Description: Echo cross-platform real-time software framework.
 Beginning as a portable game engine. Echo evolved over the years into a suite of more generalised software framework for real-time applications.
Section: devel
Depends: ${DEPENDENCIES}
EOF

dpkg-deb --build --root-owner-group ${PACKAGE_NAME}
rm -R ${PACKAGE_NAME}

SCRIPTS_DIR=$(pwd)
REPO_DIR=${REPO_DIR:-repo}
DISTRO=$(lsb_release -c | awk '{print $2'})
PACKAGE_DIR=${REPO_DIR}/pool/${DISTRO}/main/${ARCHITECTURE}/echo3
PACKAGE_LIST_DIR=${REPO_DIR}/dists/${DISTRO}/main/binary-${ARCHITECTURE}

# Move created package to directory
mkdir -p ${PACKAGE_DIR}
mv ${PACKAGE_NAME}.deb ${PACKAGE_DIR}

mkdir -p ${PACKAGE_LIST_DIR}
cd ${REPO_DIR}
dpkg-scanpackages --arch ${ARCHITECTURE} pool/ > ../${PACKAGE_LIST_DIR}/Packages
cd ..

# Check for access tokens. If they aren't available then prompt for them.
ECHO_PACKAGING_DIR=$HOME/.echo/packaging
mkdir -p ${ECHO_PACKAGING_DIR}
ACCESS_CONFIG=${ECHO_PACKAGING_DIR}/access
KEYS_DIR=${ECHO_PACKAGING_DIR}/keys
mkdir -p ${KEYS_DIR}
if ! [ -e "${ACCESS_CONFIG}" ]; then
	echo "Access config not found"
	read -p "Conduit API token: " CONDUIT_API_TOKEN
	read -p "Phabricator API Endpoint (e.g. https://www.phabricator.org/api): " PHAB_ENDPOINT
	read -p "Private key PHID: " PRIVATE_KEY_PHID
	read -p "Public key PHID: " PUBLIC_KEY_PHID
	read -p "Key file base name (e.g. emblem-pgp-key): " KEY_NAME
	read -p "Company name: " COMPANY_NAME
	read -p "Admin email: " ADMIN_EMAIL
	read -p "Repository Label: " REPO_LABEL
	
	mkdir -p "$HOME/.echo/packaging"
	echo "CONDUIT_API_TOKEN=\"${CONDUIT_API_TOKEN}\"" > ${ACCESS_CONFIG}
	echo "PHAB_ENDPOINT=\"${PHAB_ENDPOINT}\"" >> ${ACCESS_CONFIG}
	echo "PRIVATE_KEY_PHID=\"${PRIVATE_KEY_PHID}\"" >> ${ACCESS_CONFIG}
	echo "PUBLIC_KEY_PHID=\"${PUBLIC_KEY_PHID}\"" >> ${ACCESS_CONFIG}
	echo "KEY_NAME=\"${KEY_NAME}\"" >> ${ACCESS_CONFIG}
	echo "COMPANY_NAME=\"${COMPANY_NAME}\"" >> ${ACCESS_CONFIG}
	echo "ADMIN_EMAIL=\"${ADMIN_EMAIL}\"" >> ${ACCESS_CONFIG}
	echo "REPO_LABEL=\"${REPO_LABEL}\"" >> ${ACCESS_CONFIG}
else
	echo "Using access config ${ACCESS_CONFIG}"
fi

source ~/.echo/packaging/access

if [ -z "${CONDUIT_API_TOKEN}" ] || [ -z "${PRIVATE_KEY_PHID}" ] || [ -z "${PUBLIC_KEY_PHID}" ]; then
	echo "Conduit not configured. If a local key is not specified then one will be generated"
else
	mkdir -p keys
	curl -s ${PHAB_ENDPOINT}/passphrase.query -d api.token=${CONDUIT_API_TOKEN} -d phids[0]=${PRIVATE_KEY_PHID} -d needSecrets=1 | jq -r ".result.data.\"${PRIVATE_KEY_PHID}\".material.note" > "${KEYS_DIR}/${KEY_NAME}.private"
	curl -s ${PHAB_ENDPOINT}/passphrase.query -d api.token=${CONDUIT_API_TOKEN} -d phids[0]=${PUBLIC_KEY_PHID} -d needSecrets=1 | jq -r ".result.data.\"${PUBLIC_KEY_PHID}\".material.note" > "${KEYS_DIR}/${KEY_NAME}.public"
fi

cd ${REPO_DIR}/dists/${DISTRO}
cat > Release << EOF
Origin: ${COMPANY_NAME}
Label: ${REPO_LABEL}
Suite: stable
Codename: ${DISTRO}
Version: ${VERSION}
Architectures: amd64
Components: main
Description: ${COMPANY_NAME} Software Repository
Date: $(date -Ru)
EOF
${SCRIPTS_DIR}/GenerateReleaseFile.sh >> Release
cd ${SCRIPTS_DIR}

# Release signing
KEY_REAL_NAME="${COMPANY_NAME}"

# If there is already a key available we'll use that, otherwise we'll generate a new one
if ! [ -e "${KEYS_DIR}/${KEY_NAME}.private" ]; then

echo "No Key found"

export GNUPGHOME=$(mktemp -d $(pwd)/PGPKeys-XXXXXX)

echo "%echo Generating a PGP key
Key-Type: RSA
Key-Length: 4096
Name-Real: ${KEY_REAL_NAME}
Name-Email: ${ADMIN_EMAIL}
Expire-Date: 0
%no-ask-passphrase
%no-protection
%commit" > /tmp/${KEY_NAME}.batch

gpg --no-tty --batch --gen-key /tmp/${KEY_NAME}.batch

mkdir -p keys
echo "Exporting new keys ${KEY_NAME}"
gpg --armor --export "${KEY_REAL_NAME}" > ${KEYS_DIR}/${KEY_NAME}.public
gpg --armor --export-secret-keys "${KEY_REAL_NAME}" > ${KEYS_DIR}/${KEY_NAME}.private

rm -R ${GNUPGHOME}
fi

echo "Creating fresh signing environment"
export GNUPGHOME=$(mktemp -d $(pwd)/PGPKeys-XXXXXX)
cat keys/${KEY_NAME}.private | gpg --import
cat ${REPO_DIR}/dists/${DISTRO}/Release | gpg --default-key "${KEY_REAL_NAME}" -abs > ${REPO_DIR}/dists/${DISTRO}/Release.gpg
cat ${REPO_DIR}/dists/${DISTRO}/Release | gpg --default-key "${KEY_REAL_NAME}" -abs --clearsign > ${REPO_DIR}/dists/${DISTRO}/InRelease

rm -R ${GNUPGHOME}

echo "Finished"
