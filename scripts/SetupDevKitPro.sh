#! /bin/bash

bold=$(tput bold)
normal=$(tput sgr0)

if [ "$1" = "help" ] ; then
echo
echo "${bold}Echo DevKitPro setup script."
echo
echo "${normal}This script is designed to set up your environment so that Echo 3"
echo "and Echo 3 applications can be built using DevKitPro targets."
echo "The script sets up a custom version of pacman (provided by"
echo "DevKitPro) that operates entirely within the devkitpro install"
echo "location."
echo
echo "${bold}$0 package"
echo "    ${bold}package${normal} is the packages to install for the target."
echo "        gba-dev"
echo "        gp32-dev"
echo "        nds-dev"
echo "        3ds-dev"
echo "        gamecube-dev"
echo "        wii-dev"
echo "        wiiu-dev"
echo "        switch-dev"
exit 0;
fi

PACKAGE=$1

if [ "0$(id -u)" != "00" ]; then echo "This script requires higher permissions to set up some parts of the script!"; exit 1; fi

BASE_DIRECTORY=`pwd`
EXECUTING_USER=`who am i | awk '{print $1}'`

if [ "$EXECUTING_USER" = "" ] ; then
	#Some distros don't return with who am i
	EXECUTING_USER=`who| awk '{print $1}'`
fi

if [ "$EXECUTING_USER" = "" ] ; then
	echo "Error: Could not determine logged in user. We've had this problem before with the who command behaving differently on different distributions. You can try modifying the script so that EXECUTING_USER is correct. Hard coding your username here could do the trick (but please don't push the change)"
	exit 1
fi

if [ "$USERS_HOME" = "" ] ; then
	USERS_HOME=`eval echo "~$EXECUTING_USER"`
	if [ "$USERS_HOME" = "" ] ; then
		echo "Unable to determine the users home directory."
		exit 1;
	fi
fi

WHICH_DKP_PACMAN=`which dkp-pacman`
if [ "$WHICH_DKP_PACMAN" = "" ]; then
	WHICH_DPKG=`which dpkg`
	if [ "$WHICH_DPKG" == "" ]; then
		echo "If you are attempting to use this script on a non-debian system please modify it as"
		echo "needed to setup pacman for your system based on instructions here:"
		echo "    https://devkitpro.org/wiki/devkitPro_pacman"
		echo "Consider contributing a patch upstream Echo to support your system."
		exit 1;
	fi
	wget https://github.com/devkitPro/pacman/releases/download/v1.0.2/devkitpro-pacman.amd64.deb
	sudo apt-get install gdebi-core -y
	gdebi devkitpro-pacman.amd64.deb
	rm devkitpro-pacman.amd64.deb
fi
dkp-pacman -S --noconfirm $PACKAGE 
source /etc/profile.d/devkit-env.sh
