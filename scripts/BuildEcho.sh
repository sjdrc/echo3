#! /bin/bash

bold=$(tput bold)
normal=$(tput sgr0)

if [ "0$(id -u)" = "00" ]; then
	echo "This script should not be run by root.";
	exit 1;
fi

EXECUTING_USER=`who am i | awk '{print $1}'`

if [ "$EXECUTING_USER" = "" ] ; then
	#Some distros don't return with who am i
	EXECUTING_USER=`who| awk '{print $1}'`
fi

if [ "$EXECUTING_USER" = "" ] ; then
	echo "${bold}Error: Could not determine logged in user.${normal} We've had this problem before with the ${bold}who${normal} command behaving differently on different distributions. You can try modifying the script so that ${bold}EXECUTING_USER${normal} is correct. Hardcoding your username here could do the trick, but please don't push the change!"
	exit 1
fi

if [ "$USERS_HOME" = "" ] ; then
	USERS_HOME=`eval echo "~$EXECUTING_USER"`
	if [ "$USERS_HOME" = "" ] ; then
		echo "${bold}Unable to determine the user's home directory.${normal}"
		exit 1;
	fi
fi

if [ "$BUILD_LOG_DIRECTORY" = "" ] ; then
	BUILD_LOG_DIRECTORY=`pwd`
	BUILD_LOG_DIRECTORY+="/buildlogs"
fi

if [ "$PLATFORMS" = "" ] ; then
	echo "PLATFORMS not defined. Defaulting to linux target."
	PLATFORMS="linux"
fi

LINUX_BUILD_CONFIGURATIONS="${1:-Debug Release}"

declare -A platformConfigurations
platformConfigurations["wii"]="Wii"
platformConfigurations["android"]="Android"
platformConfigurations["windowscross"]="Windows"
platformConfigurations["linux"]=$LINUX_BUILD_CONFIGURATIONS

#Make sure the environment is ok for this session.
source $USERS_HOME/.profile
if [ ! -d "$BUILD_LOG_DIRECTORY" ]; then
	echo "Creating build log directory: $BUILD_LOG_DIRECTORY"
	mkdir -p "$BUILD_LOG_DIRECTORY"
fi

for p in $PLATFORMS; do
	cd "$ECHO_ENGINE_INSTALL_DIR"
	CONFIGURATIONS=${platformConfigurations[$p]}
	for configuration in $CONFIGURATIONS; do
		echo "Building Echo: $configuration for $p..."
		BUILD_LOG_FILE="$BUILD_LOG_DIRECTORY/$p-$configuration.buildlog"
		make CONF=$configuration -j`nproc`>"$BUILD_LOG_FILE" 2>&1
		if [ ! $? -eq 0 ]; then
			echo "${bold}There were errors. Please see ${BUILD_LOG_FILE}${normal}"
			echo "Fix the errors, then rerun ${bold}BuildEcho.sh${normal} to complete the build."
		else
			echo "Done!"
		fi
	done
done

