#! /bin/bash

bold=$(tput bold)
normal=$(tput sgr0)

if [ "$1" = "help" ] ; then
echo
echo "${bold}Echo setup script."
echo
echo "${normal}This script is designed to set up your environment so that you can start"
echo "developing Echo. The script will install needed dependencies."
echo
echo "The following environment variables can be set prior to invoking the script"
echo
echo "    ${bold}ANDROID_DEVELOPMENT${normal} set to 1 or 0 to indicate whether or not you want"
echo "        to set up Android development. Specifying this disables the prompt."
echo "    ${bold}WII_DEVELOPMENT${normal} set to 1 or 0 to indicate whether or not you want"
echo "        to set up Wii development. Specifying this disables the prompt."
echo "    ${bold}WINDOWS_DEVELOPMENT${normal} set to 1 or 0 to indicate whether or not you want"
echo "        to set up Windows development. Specifying this disables the prompt."
echo "    ${bold}LOCAL_BUILD_ONLY${normal} Same as setting the above variables to 0."
echo
echo "    ${bold}AUTOMATICALLY_BUILD_ECHO${normal} set to 1 or 0 to set whether or not to build"
echo "        automatically. Default is 1"
echo
echo "    ${bold}SKIP_SYSTEM_DEPENDENCIES${normal} when not 0, system dependency installation"
echo "        and checks are disabled. This is useful if you have a system that you know has"
echo "        all dependencies already installed and you may not have Internet connectivity."
echo "        Default is 0"
echo
echo "    ${bold}IGNORE_EXISTING_INSTALL${normal} when not 0 and ECHO_ENGINE_INSTALL_DIR is set"
echo "        the build will ignore the potential conflict location. For the current terminal"
echo "        session ECHO_ENGINE_INSTALL_DIR will be modified to this Echo and be built."
echo "        Default is 0"
echo
echo "    ${bold}LINUX_BUILD_CONFIGURATIONS${normal} Specify the configurations to build"
echo "        separated by spaces. This is useful for Release only builds."
echo "        The default is \"Debug Release\""
echo
echo "The script needs to be invoked with ${bold}elevated permissions${normal} in order"
echo "to install dependencies."
exit 0;
fi

export SCRIPT_DIRECTORY="$(dirname "$(readlink -f "$0")")"

run_as_user() {
    if [ "${USER-root}" = "root" ]
    then
        /bin/su $EXECUTING_USER -c "$@"
    else
        "$@"
    fi
}

ANDROID_SETUP_SCRIPT="scripts/SetupAndroidDevelopment.sh"
DEVKITPRO_SETUP_SCRIPT="scripts/SetupDevKitPro.sh"
STANDALONE_DEPENDENCIES_SCRIPT="scripts/SetupStandaloneDependencies.sh"
EXECUTING_USER=`who am i | awk '{print $1}'`
AUTOMATICALLY_BUILD_ECHO=${AUTOMATICALLY_BUILD_ECHO:-1}
LINUX_BUILD_CONFIGURATIONS=${LINUX_BUILD_CONFIGURATIONS:-Debug Release}
IGNORE_EXISTING_INSTALL=${IGNORE_EXISTING_INSTALL:-0}
SKIP_SYSTEM_DEPENDENCIES=${SKIP_SYSTEM_DEPENDENCIES:-0}
LOCAL_BUILD_ONLY=${LOCAL_BUILD_ONLY:-0}

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
		echo "${bold}Error: Unable to determine the user's home directory.${normal}"
		exit 1;
	fi
fi

# Check convenient non-interactive feature options
if [ "$LOCAL_BUILD_ONLY" = "1" ] ; then
	ANDROID_DEVELOPMENT=0
	WII_DEVELOPMENT=0
	WINDOWS_DEVELOPMENT=0
fi

#Change to the parent directory (where echo is)
cd $SCRIPT_DIRECTORY/..

INSTALL_LOCATION=${INSTALL_LOCATION:-/opt/echo3}

NEEDS_ELEVATED_USER=0

if [ "$SKIP_SYSTEM_DEPENDENCIES" = "0" ] || [ "$ANDROID_DEVELOPMENT" = "1" ] || [ "$WII_DEVELOPMENT" = "1" ] || [ "$WINDOWS_DEVELOPMENT" = "1" ] || [ "$LOCAL_BUILD_ONLY" = "0" ]; then
	NEEDS_ELEVATED_USER=1
fi

if [ "$NEEDS_ELEVATED_USER" = "1" ] ; then
	if [ "0$(id -u)" != "00" ]; then
		echo "The combination of settings means that this script requires higher permissions to check and install dependencies."
		exit 1;
	fi
fi

if [ "$SKIP_SYSTEM_DEPENDENCIES" = "0" ] ; then

	# Ensure thirdparty dependencies are installed:
	apt-get install mercurial g++ build-essential libmodplug-dev libogg-dev libfreetype6-dev libpulse-dev libsigc++-2.0-dev libgtkglext1-dev libprotobuf-dev libvorbis-dev libglew-dev libutfcpp-dev libgtkmm-2.4-dev libgtkglextmm-x11-1.2-dev protobuf-compiler cmake jam autoconf automake libtinyxml2-dev libssl-dev libbluetooth-dev libqt5opengl5-dev qt5-qmake qt5-qmake-bin qtbase5-dev qtbase5-dev-tools nasm curl libarchive-tools libdrm-dev -y

	if [ ! $? -eq 0 ]; then
		echo "${bold}Dependencies were not installed correctly.${normal} Please fix the error above and re-run this script."
		echo "${bold}NOTE: ${normal} If you continue to experience problems at this point then this error checking may have been the cause of T752. If you have a developer account please report this in the task, otherwise please email sean+echo@emblem.net.au."
		exit 1;
	fi

	#Do these two separately because one will fail (Ubuntu 15+ uses libtool-bin)
	# Check if they exist first, to suppress error messages:
	dpkg -l libtool &> /dev/null && apt-get install libtool -y
	dpkg -l libtool-bin &> /dev/null && apt-get install libtool-bin -y

	#Do these two separately because one will fail (Ubuntu 16+ uses libpng-dev)
	# Check if they exist first, to suppress error messages:
	dpkg -l libpng-dev &> /dev/null && apt-get install libpng-dev -y
	apt-get install libpng12-dev -y &> /dev/null

	function IsPackageInstalled()
	{
		# from https://stackoverflow.com/a/10439058
		local PKG_OK=$(dpkg-query -W --showformat='${Status}\n' $1 2> /dev/null |grep "install ok installed")
		if [ "" == "$PKG_OK" ]; then
			return 0
		fi
		return 1
	}

	if !(IsPackageInstalled libtool || IsPackageInstalled libtool-bin) || !(IsPackageInstalled libpng-dev || IsPackageInstalled libpng12-dev); then
		echo "${bold}Dependencies were not installed correctly.${normal} Please fix the error above and re-run this script."
		exit 1;
	fi
fi

if [ -e "$USERS_HOME/.profile" ]; then
	echo "Backing up $USERS_HOME/.profile..."
	cp $USERS_HOME/.profile $USERS_HOME/.profile.backup
	chown $EXECUTING_USER:$EXECUTING_USER $USERS_HOME/.profile.backup
	source $USERS_HOME/.profile
else
	echo "Creating $USERS_HOME/.profile..."
	echo "# $USERS_HOME/.profile: executed by the command interpreter for login shells." > $USERS_HOME/.profile
	chown $EXECUTING_USER:$EXECUTING_USER $USERS_HOME/.profile
fi

#Echo environment variable.
if [ ! "$ECHO_ENGINE_INSTALL_DIR" = "" ] ; then
	if [ "$IGNORE_EXISTING_INSTALL" = "0" ] ; then
		if [ ! "$ECHO_ENGINE_INSTALL_DIR" = "$INSTALL_LOCATION" ] ; then
			echo "${bold}ERROR: ECHO_ENGINE_INSTALL_DIR is already defined ($ECHO_ENGINE_INSTALL_DIR) but not equal to $INSTALL_LOCATION which is what the script has been asked to set up. Please make sure you modify your environment to select the correct target.${normal}"
			echo "The easiest fix is to remove the variables from ${bold}~/.profile${normal} and then rerun this script with the same options."
			echo "Building will fail with the current setup, exiting..."
			exit 1;
		else
		echo "ECHO_ENGINE_INSTALL_DIR is already correctly set."
		fi
	else
		echo "${bold}NOTE: ${normal} Ignoring existing install. I'll build things here but other projects might use a different installed version."
		export ECHO_ENGINE_INSTALL_DIR="$INSTALL_LOCATION"
	fi
else
	echo "Adding ECHO_ENGINE_INSTALL_DIR to your environment using $USERS_HOME/.profile"...
	echo "export ECHO_ENGINE_INSTALL_DIR=\"$INSTALL_LOCATION\"" >> $USERS_HOME/.profile
	# The variable wasn't set, so lets add it
	export ECHO_ENGINE_INSTALL_DIR="$INSTALL_LOCATION"
fi

#We aways require dependencies now.
REQUIRE_DEPENDENCIES=1
PLATFORMS="linux"

if [ -z "$ANDROID_DEVELOPMENT" ] ; then
	read -p "Are you planning on doing Android development with Echo? (y/n)" ANDROID_DEVELOPMENT
fi

case ${ANDROID_DEVELOPMENT:0:1} in
	y|Y )
		PLATFORMS="$PLATFORMS android"
		./$ANDROID_SETUP_SCRIPT
		if [ ! $? -eq 0 ]; then
			echo "${bold}Android setup failed.${normal} Please fix the error above and re-run this script."
			exit 1;
		fi
		REQUIRE_DEPENDENCIES=1
	;;
	* )
		echo "If you change your mind run the following:"
		echo "    $ANDROID_SETUP_SCRIPT"
		echo "	  PLATFORMS=\"android\" ./$STANDALONE_DEPENDENCIES_SCRIPT"
		echo "    PLATFORMS=\"android\" ./BuildEcho.sh"
	;;
esac

if [ -z "$WII_DEVELOPMENT" ] ; then
	read -p "Are you planning on doing Wii development with Echo? (y/n)" WII_DEVELOPMENT
fi

case ${WII_DEVELOPMENT:0:1} in
	y|Y )
		if [ "$DEVKITPPC" = "" ] ; then
			echo "DEVKITPPC environment variable is not set. Assuming DevKitPPC is not installed. Attempting install..."

			chmod +x $DEVKITPRO_SETUP_SCRIPT
			./$DEVKITPRO_SETUP_SCRIPT wii-dev
			if [ ! $? -eq 0 ]; then
				echo "${bold}Wii development setup failed.${normal} Please fix the error above and re-run this script."
				exit 1;
			fi
		fi
		PLATFORMS="$PLATFORMS wii"
		REQUIRE_DEPENDENCIES=1
	;;
	* )
		echo "If you change your mind run the following:"
		echo "    $DEVKITPRO_SETUP_SCRIPT wii-dev"
		echo "    PLATFORMS=\"wii\" ./$STANDALONE_DEPENDENCIES_SCRIPT"
		echo "    PLATFORMS=\"wii\" ./BuildEcho.sh"
	;;
esac

if [ -z "$WINDOWS_DEVELOPMENT" ] ; then
	read -p "Are you planning on doing Windows development with Echo? (y/n)" WINDOWS_DEVELOPMENT
fi
case ${WINDOWS_DEVELOPMENT:0:1} in
	y|Y )
		apt-get install -y mingw-w64
		if [ ! $? -eq 0 ]; then
			echo "${bold}Windows MinGW installation failed.${normal} Please fix the error above and re-run this script."
			exit 1;
		fi
		PLATFORMS="$PLATFORMS windowscross"
		REQUIRE_DEPENDENCIES=1
	;;
	* )
		echo "If you change your mind run:"
		echo "    sudo apt-get install mingw-w64"
		echo "    PLATFORMS=\"windowscross\" ./$STANDALONE_DEPENDENCIES_SCRIPT"
		echo "    PLATFORMS=\"windowscross\" ./BuildEcho.sh"
	;;
esac

# Check for deprecation of ECHO_ENGINE_DEPENDENCIES_DIR
if [ ! "$ECHO_ENGINE_DEPENDENCIES_DIR" = "" ] ; then
	echo "${bold}NOTE: ${normal} ECHO_ENGINE_DEPENDENCIES_DIR has been deprecated. You can remove it from your environment (usually in $USERS_HOME/.profile). If you are using this variable manually please update your usage to '\$ECHO_ENGINE_INSTALL_DIR/dependencies'"...
fi


if [ $REQUIRE_DEPENDENCIES -eq 1 ] ; then
	export PLATFORMS=$PLATFORMS
	run_as_user "./$STANDALONE_DEPENDENCIES_SCRIPT"
	if [ ! $? -eq 0 ]; then
		echo "${bold}Building dependencies failed.${normal} Please fix the error above and re-run this script."
		exit 1;
	fi
fi

if [ $AUTOMATICALLY_BUILD_ECHO -eq 1 ] ; then
	echo "Building Echo."
	export BUILD_LOG_DIRECTORY="$ECHO_ENGINE_INSTALL_DIR/build/logs"
	cd "$ECHO_ENGINE_INSTALL_DIR"
	run_as_user ./scripts/BuildEcho.sh $LINUX_BUILD_CONFIGURATIONS
	if [ ! $? -eq 0 ]; then
		echo "${bold}Building Echo failed.${normal} Please fix the error above and re-run this script (or just the ${bold}BuildEcho.sh${normal} script)."
		exit 1;
	fi
else
	echo "Skipped building Echo."
fi

echo "NOTE: If you are using a desktop environment you will need to logout then back in before your environment includes the new variables."

echo "All done =)"

