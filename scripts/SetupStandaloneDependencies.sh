#! /bin/bash

bold=$(tput bold)
normal=$(tput sgr0)

if [ "$1" = "help" ] ; then
echo
echo "${bold}Echo dependencies setup script."
echo
echo "${normal}This script is designed to set up your environment so that you can start"
echo "developing Echo with standalone dependencies."
echo 
echo "${normal}Dependencies are built to a location and a kept isolated from the system"
echo "to avoid conflicts. The script can also build any available cross compilation"
echo "targets in the same way."
exit 0;
fi

BASE_DIRECTORY=`pwd`
cd "$ECHO_ENGINE_INSTALL_DIR/dependencies"
EXPECTED_ANDROID_TOOLCHAIN_LOCATION="/opt/android/android-9"

if [ "$PLATFORMS" = "" ] ; then
echo "Error: PLATFORMS has not been set."
echo "Please set the PLATFORMS environment variable before invoking this script."
echo "To specify the platforms as you invoke this script use:"
echo "   PLATFORMS=\"platform1 platform2 platformN\" $0"
echo "For available platforms please see what configurations are available in $ECHO_ENGINE_INSTALL_DIR/dependencies"
echo "Note: Some platforms do not require that the dependencies built. For example, Most Linux distros"
echo "have a package management system that allows you to download development packages."
exit 1;
fi

EXECUTING_USER=`who am i | awk '{print $1}'`

if [ "$EXECUTING_USER" = "" ] ; then
	#Some distros don't return with who am i
	EXECUTING_USER=`who| awk '{print $1}'`
fi

if [ "$EXECUTING_USER" = "" ] ; then
	echo "${bold}Error: Could not determine logged in user.${normal} We've had this problem before with the ${bold}who${normal} command behaving differently on different distributions. You can try modifying the script so that EXECUTING_USER is correct. Hardcoding your username here could do the trick, but please don't push the change!"
	exit 1
fi

if [ "$USERS_HOME" = "" ] ; then
	USERS_HOME=`eval echo "~$EXECUTING_USER"`
	if [ "$USERS_HOME" = "" ] ; then
		echo "${bold}Unable to determine the user's home directory.${normal}"
		exit 1;
	fi
fi

WILL_BUILD_ANDROID=0
for platform in $PLATFORMS ; do
	if [ "$platform" = "android" ] ; then
		WILL_BUILD_ANDROID=1
	fi
done

if [ $WILL_BUILD_ANDROID -eq 1 ]; then
	if [ ! -e "$EXPECTED_ANDROID_TOOLCHAIN_LOCATION" ]; then
		echo "${bold}$EXPECTED_ANDROID_TOOLCHAIN_LOCATION was not found. android build target will fail.${normal}"
		echo "    To fix this and build android manually execute the following commands:"
		echo "        ./SetupAndroidDevelopment.sh"
		echo "        cd $ECHO_ENGINE_INSTALL_DIR/dependencies"
		echo "        ./edepbuild android"
		exit 1;
	fi
fi

#Now build each platform.
cd "$ECHO_ENGINE_INSTALL_DIR/dependencies"

if [ ! -d "log" ]; then
	mkdir "log"
fi

for platform in $PLATFORMS ; do
	echo "Attempting to build for target \"$platform\", this will take a while... "
	./edepbuild $platform > log/$platform.buildoutput 2>&1
	if [ ! $? -eq 0 ]; then
		echo "${bold}Build for target \"$platform\" failed.${normal} Please see log/$platform.buildoutput. You can manually build this target with \"./edepbuild $platform\""
	else
		echo "Built for target \"$platform\" successfully"
	fi
done

cd $BASE_DIRECTORY
