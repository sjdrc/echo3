#! /bin/bash
### -====================================-
### Echo Project Generator
### -====================================-
###
### This script will generate a project for you.
###
### You must specify a project name and template.
###
### ./GenerateProject.sh name template destination
###
### NOTE: Any characters in name that are invalid for C++ type names will be replaced with underscores.
###
### destination is the destination directory for the project. The script will create a folder here for
### the generated project files.
###
### template is one of the following:
### 

usage()
{
	sed -n 's/^### \?//p' "$0"
	for templateDirectory in *; do
		if [ -d "${templateDirectory}" ]; then
			echo "    ${templateDirectory}"
			if [ -e "${templateDirectory}/description" ]; then
				cat "${templateDirectory}/description" | awk '{print "\t"$0}'
			fi
		fi
	done
}

if [ "$1" = "" ] || [ "$2" = "" ] || [ "$3" = "" ] ; then
	usage
	exit
fi

PROJECT_NAME=$(echo "${1}" | sed -E 's/^([0-9])|[^a-zA-Z0-9]/_\1/g')
TEMPLATE=${2}
DESTINATION="${3}"

if ! [ -d "${TEMPLATE}" ]; then
	echo "ERROR: ${TEMPLATE}" is not a valid template.
	usage
	exit 1
fi

DESTINATION_RESOLVED=$(readlink -f "${DESTINATION}")

if [ -e "${DESTINATION}" ] && ! [ -d "${DESTINATION}" ] ; then
	echo "ERROR: destination must be a directory"
	exit 1
fi

if ! [ "${1}" = "${PROJECT_NAME}" ]; then
	echo "Project name contains invalid characters: ${1}"
	echo "Project name changed to: ${PROJECT_NAME}"
else
	echo "Project name is: ${PROJECT_NAME}"
fi
echo "Template is: ${TEMPLATE}"

echo "Destination is: ${DESTINATION}"
if ! [ "${DESTINATION}" = "${DESTINATION_RESOLVED}" ]; then
	echo "Destination resolved to: ${DESTINATION_RESOLVED}" 
fi

TEMPLATE_DIR=$(dirname "$(readlink -f "$0")")
echo "Template dir: ${TEMPLATE_DIR}"
if [ "${TEMPLATE_DIR}" = "${DESTINATION}" ]; then
	echo "ERROR: The desitation directory should be a different directory to the template directory"
	exit 1;
fi

DESTINATION_RESOLVED="${DESTINATION_RESOLVED}/${PROJECT_NAME}"

if [ -e "${DESTINATION_RESOLVED}" ]; then
	echo "ERROR: The folder \"${DESTINATION_RESOLVED}\" already exists. Aborting."
	exit 1
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

echo "Creating directory ${DESTINATION_RESOLVED}"
mkdir -p ${DESTINATION_RESOLVED}

echo "Copying template ${TEMPLATE}"
if [ -e "${TEMPLATE}/description" ]; then
	cat "${TEMPLATE}/description" | awk '{print "\t"$0}'
fi
rsync -qav ${TEMPLATE_DIR}/${TEMPLATE}/* ${DESTINATION_RESOLVED} --exclude build

echo "Switching to project directory ${DESTINATION_RESOLVED}"
cd ${DESTINATION_RESOLVED}

echo "Applying template modifications"
source "${TEMPLATE_DIR}/apply_common"
if [ -e "${DESTINATION_RESOLVED}/apply" ]; then
	source "${DESTINATION_RESOLVED}/apply"
	rm ${DESTINATION_RESOLVED}/apply
fi

echo "Project generation complete"
echo "Changed to ${DESTINATION_RESOLVED} and run the following commands:"
echo "    mkdir build"
echo "    cd build"
echo "    cmake -DECHO_ENGINE_INSTALL_DIR=\"${ECHO_ENGINE_INSTALL_DIR}\" .."
echo "    make"
echo "A .vscode/settings.json file has been generated to set ECHO_ENGINE_INSTALL_DIR for you."
echo ""
echo "Use the script cmakeplatform to generate build files for a specific platform"
