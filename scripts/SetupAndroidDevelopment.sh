#! /bin/bash

InstallUniversalAndroidUdevRule () {
    RULE='ACTION=="add", SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", ENV{ID_USB_INTERFACES}=="*:ff420?:*", MODE="0666", GROUP="plugdev", SYMLINK+="android/$env{ID_SERIAL_SHORT}"'
    RULEFILE="/etc/udev/rules.d/${1:-90}-universal-android.rules"
    if [ "0$(id -u)" != "00" ]; then echo "No permission to add the universal Android udev rule!"; return 1; fi
    echo "$RULE" > $RULEFILE
    udevadm control --reload-rules
    udevadm trigger --action=add --subsystem-match=usb
    return 0
}

bold=$(tput bold)
normal=$(tput sgr0)

if [ "$1" = "help" ] ; then
echo
echo "${bold}Echo Android development setup script."
echo
echo "${normal}This script is designed to set up your environment so that Echo 3"
echo "and Echo 3 applications can be built for Android platforms."
echo "The script, by default, is configured to allow Echo 3 to build"
echo "for the lowest version of Android as officially supported."
echo "However if you want to use features from newer versions of the"
echo "SDK or NDK you can modify the variables at the beginning of this"
echo "script to target a higher version of Android. If you do this you"
echo "will need to make sure Echo 3 and your projects are configured to"
echo "point to the appropriate toolkit."
echo
echo "${bold}$0 ${normal}[BASE_DIRECTORY]"
echo "    ${bold}BASE_DIRECTORY${normal} is the location that downloaded files will be"
echo "    placed and extracted to."
echo
echo "You may also define ${bold}TOOLCHAIN_INSTALL_DIRECTORY${normal} before invoking the"
echo "script to set the installation directory. The default is"
echo "/opt/android which is also where Echo 3 makefiles usually expect"
echo "the toolkit to be."
echo 
echo "The script needs to be invoked with ${bold}elevated permissions${normal} in order to"
echo "install some dependencies and install the toolkit to the default"
echo "destination."
exit 0;
fi

if [ "0$(id -u)" != "00" ]; then echo "This script requires higher permissions to set up some parts of the script!"; exit 1; fi

BASE_DIRECTORY=`pwd`
ANDROID_NDK_VERSION="android-ndk-r12"
ANDROID_NDK_SYSTEM="linux-x86_64"
NDK_PLATFORM_VERSIONS="android-9 android-15 android-19"
NDK_STL="stlport"
NDK_TOOLCHAINS="arm-linux-androideabi-4.9"
NDK_FILENAME="$ANDROID_NDK_VERSION-$ANDROID_NDK_SYSTEM.zip"
SDK_FILENAME="android-sdk_r24.0.2-linux.tgz"
ANDROID_SDK_PACKAGES="platform-tools,android-10,android-15,android-19"
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

if [ "$TOOLCHAIN_INSTALL_DIRECTORY" = "" ] ; then
	TOOLCHAIN_INSTALL_DIRECTORY="/opt/android"
fi

echo "TOOLCHAIN_INSTALL_DIRECTORY is $TOOLCHAIN_INSTALL_DIRECTORY"

if [ ! "$1" = "" ] ; then
	BASE_DIRECTORY=$1
	echo "Checking for $BASE_DIRECTORY..."
	if [ ! -d "$BASE_DIRECTORY" ]; then
		mkdir "$BASE_DIRECTORY"
	fi
fi

cd $BASE_DIRECTORY
#Make sure the base directory is the full path now.
BASE_DIRECTORY=`pwd`

ANDROID_DIRECTORY="$BASE_DIRECTORY/AndroidDevelopment"

if [[ ! "$TOOLCHAIN_INSTALL_DIRECTORY" = /* ]]; then
echo TOOLCHAIN_INSTALL_DIRECTORY is relative, updating to $BASE_DIRECTORY/$TOOLCHAIN_INSTALL_DIRECTORY
TOOLCHAIN_INSTALL_DIRECTORY="$BASE_DIRECTORY/$TOOLCHAIN_INSTALL_DIRECTORY"
fi

if [ ! -e "$ANDROID_DIRECTORY" ]; then
	echo "Creating $ANDROID_DIRECTORY..."
	mkdir $ANDROID_DIRECTORY
fi

cd $ANDROID_DIRECTORY
ANDROID_ABSOLUTE_DIRECTORY=`pwd`

echo "Checking for existing SDK folder android-sdk-linux..."
if [ ! -e "android-sdk-linux" ]; then
	if [ ! -e "$SDK_FILENAME" ]; then
		echo "Downloading Android SDK..."
		wget http://dl.google.com/android/$SDK_FILENAME
	else
		echo "$SDK_FILENAME already found. Skipping download."
	fi

	echo "Extracting Android SDK..."
	tar zxf $SDK_FILENAME
	echo "$SDK_FILENAME is no longer required, you can remove it if you like."
else
	echo "The folder 'android-sdk-linux' already exists, I will attempt to download the specified packages if required."
fi

cd "android-sdk-linux/tools"
./android update sdk --all --no-ui --filter $ANDROID_SDK_PACKAGES

cd $ANDROID_ABSOLUTE_DIRECTORY

if [ -e "$USERS_HOME/.profile" ]; then
	echo "Backing up $USERS_HOME/.profile..."
	cp $USERS_HOME/.profile $USERS_HOME/.profile.backup
else
	echo "Creating $USERS_HOME/.profile..."
	echo "# $USERS_HOME/.profile: executed by the command interpreter for login shells." > $USERS_HOME/.profile
fi

echo "Checking for adb..."
if hash adb 2>/dev/null; then
	echo "It seems that you already have adb installed. Skipping including Android SDK to PATH".
else
	echo "adb was not detected. adb is the Android Debug Bridge and is required to install and launch applications from the commandline. A version of adb comes with the Android SDK and I can add the appropriate path to your PATH variable."
	read -p "Would you like me to add Android SDK platform tools folder to your PATH variable? (y/n)" answer
	case ${answer:0:1} in
		y|Y )
			echo "Adding $ANDROID_ABSOLUTE_DIRECTORY/android-sdk-linux/platform-tools to PATH using $USERS_HOME/.profile"...
			echo "export PATH=\"$ANDROID_ABSOLUTE_DIRECTORY/android-sdk-linux/platform-tools:\$PATH\"" >> $USERS_HOME/.profile
		;;
		* )
			echo "adb will not be available. Run scripts may not work correctly. Please ensure you set up adb manually."
		;;
	esac
fi

echo -n "Checking for ant..."
if hash ant 2>/dev/null; then
	echo "Found ant."
else
	echo "ant not found. Attempting to install..."
	apt-get install ant
fi

echo "Attempting to add generic udev rules for Android development devices..."
InstallUniversalAndroidUdevRule

cd $ANDROID_DIRECTORY

echo "Checking for existing NDK folder $ANDROID_NDK_VERSION..."

if [ ! -e "$ANDROID_NDK_VERSION" ]; then
	
	echo "$NDK_FILENAME"
	if [ ! -e "$NDK_FILENAME" ]; then
		echo "Downloading Android NDK..."
		wget https://dl.google.com/android/repository/$NDK_FILENAME
	else
		echo "$NDK_FILENAME already found. Skipping download."
	fi
	
	echo "Extracting Android NDK..."
	unzip $NDK_FILENAME
	echo "$NDK_FILENAME is no longer required, you can remove it if you like."
else
	echo "Found folder $ANDROID_NDK_VERSION already exists."
fi

cd $ANDROID_NDK_VERSION/build/tools

#Set the invoking user as the owner of the folder because the script will be run as root.
chown $EXECUTING_USER:$EXECUTING_USER -R $ANDROID_ABSOLUTE_DIRECTORY
chown $EXECUTING_USER:$EXECUTING_USER -R ~/.android

THIS_ECHO_ANDROID_NDK_TOOLCHAIN_BASE=$TOOLCHAIN_INSTALL_DIRECTORY

for NDK_PLATFORM_VERSION in $NDK_PLATFORM_VERSIONS; do
	for NDK_TOOLCHAIN in $NDK_TOOLCHAINS; do
		echo "Building standalone toolchain: $NDK_PLATFORM_VERSION..."
		cd "$ANDROID_ABSOLUTE_DIRECTORY/$ANDROID_NDK_VERSION/build/tools/"
		THIS_ECHO_ANDROID_NDK_TOOLCHAIN=$THIS_ECHO_ANDROID_NDK_TOOLCHAIN_BASE/$NDK_PLATFORM_VERSION/$NDK_TOOLCHAIN
		#MAKETOOLCHAIN_PARAMETERS="--install-dir $THIS_ECHO_ANDROID_NDK_TOOLCHAIN --api $NDK_PLATFORM_VERSION --stl $NDK_STL -v --arch $NDK_ARCH"
		#echo "Attempting to create a standalone toolchain with parameters: $MAKETOOLCHAIN_PARAMETERS"
		#python make_standalone_toolchain.py $MAKETOOLCHAIN_PARAMETERS
		MAKETOOLCHAIN_PARAMETERS="--install-dir=\"$THIS_ECHO_ANDROID_NDK_TOOLCHAIN\" --ndk-dir=\"$ANDROID_ABSOLUTE_DIRECTORY/$ANDROID_NDK_VERSION\" --platform=$NDK_PLATFORM_VERSION --stl=$NDK_STL --toolchain=$NDK_TOOLCHAIN"
		echo "Attempting to create a standalone toolchain with parameters: $MAKETOOLCHAIN_PARAMETERS"
		./make-standalone-toolchain.sh $MAKETOOLCHAIN_PARAMETERS
		#sudo ./make-standalone-toolchain.sh --install-dir=$ANDROID_NDK_VERSION --platform=android-9 --arch=linux-x86_64 --stl=stlport
	done
done
chown $EXECUTING_USER:$EXECUTING_USER -R $TOOLCHAIN_INSTALL_DIRECTORY

#Set ECHO_ANDROID_NDK_TOOLCHAINS
#Echo environment variable.
if [ ! "$ECHO_ANDROID_NDK_TOOLCHAIN" = "" ] ; then
	echo "Removing old ECHO_ANDROID_NDK_TOOLCHAIN variable from $USERS_HOME/.profile"
	# remove existing ECHO_ANDROID_NDK_TOOLCHAIN(S) lines because we want to set the new value.
	sed -i '/export ECHO_ANDROID_NDK_TOOLCHAIN/d' $USERS_HOME/.profile
fi

#remove lines with export ECHO_ANDROID_NDK_TOOLCHAINS since we will add it.
sed -i '/export ECHO_ANDROID_NDK_TOOLCHAINS/d' $USERS_HOME/.profile

if [ ! "$ECHO_ANDROID_NDK_TOOLCHAINS" = "" ] ; then
	# remove existing ECHO_ANDROID_NDK_TOOLCHAINS lines because we want to set the new value, if it is
	# the same then it isn't a problem. It just gets added again after this check.
	sed -i '/export ECHO_ANDROID_NDK_TOOLCHAINS/d' $USERS_HOME/.profile
	if [ ! "$ECHO_ANDROID_NDK_TOOLCHAINS" = "$THIS_ECHO_ANDROID_NDK_TOOLCHAIN_BASE" ] ; then
		echo "WARNING: ECHO_ANDROID_NDK_TOOLCHAINS was defined ($ECHO_ANDROID_NDK_TOOLCHAINS) but was not equal to $THIS_ECHO_ANDROID_NDK_TOOLCHAIN_BASE.. I have modified the entry to the new value."
	fi
fi
echo "Adding ECHO_ANDROID_NDK_TOOLCHAINS to your environment using $USERS_HOME/.profile"...
echo "export ECHO_ANDROID_NDK_TOOLCHAINS=\"$THIS_ECHO_ANDROID_NDK_TOOLCHAIN_BASE\"" >> $USERS_HOME/.profile

#remove lines with export ECHO_ANDROID_NDK= since we will add it.
sed -i '/export ECHO_ANDROID_NDK=/d' $USERS_HOME/.profile

#Set ECHO_ANDROID_NDK
#Echo environment variable.
if [ ! "$ECHO_ANDROID_NDK" = "" ] ; then
	if [ ! "$ECHO_ANDROID_NDK" = "$ANDROID_ABSOLUTE_DIRECTORY/$ANDROID_NDK_VERSION" ] ; then
		echo "WARNING: ECHO_ANDROID_NDK wass already defined ($ECHO_ANDROID_NDK) but not equal to the new value $ANDROID_ABSOLUTE_DIRECTORY/$ANDROID_NDK_VERSION."
	fi
fi
echo "Adding ECHO_ANDROID_NDK to your environment using $USERS_HOME/.profile"...
echo "export ECHO_ANDROID_NDK=\"$ANDROID_ABSOLUTE_DIRECTORY/$ANDROID_NDK_VERSION\"" >> $USERS_HOME/.profile

echo "Remove old NDK toolchain from PATH if it exists in $USERS_HOME/.profile."
# export PATH="$HOME/AndroidDevelopment/android-ndk-r9d:$PATH"
sed -i '/export PATH="\$HOME\/AndroidDevelopment\/android-ndk-r9d:\$PATH"/d' $USERS_HOME/.profile
sed -i '/export PATH="\$ECHO_ANDROID_NDK:\$PATH\"/d' $USERS_HOME/.profile

echo "Remove old NDK toolchain from PATH if it exists in $USERS_HOME/.profile."
echo "export PATH=\"\$ECHO_ANDROID_NDK:\$PATH\"" >> $USERS_HOME/.profile

# Remove ECHO_ANDROID_SDK from .profile
sed -i '/export ECHO_ANDROID_SDK/d' $USERS_HOME/.profile

#Set ECHO_ANDROID_SDK
#Echo environment variables.
if [ ! "$ECHO_ANDROID_SDK" = "" ] ; then
	if [ ! "$ECHO_ANDROID_SDK" = "$ANDROID_ABSOLUTE_DIRECTORY/android-sdk-linux" ] ; then
		echo "WARNING: ECHO_ANDROID_SDK was previously defined as ($ECHO_ANDROID_SDK) which is not the same as the new value $ANDROID_ABSOLUTE_DIRECTORY/android-sdk-linux. Previous installations may not work as expected."
	fi
fi

echo "Adding ECHO_ANDROID_SDK to your environment using $USERS_HOME/.profile"
echo "export ECHO_ANDROID_SDK=\"$ANDROID_ABSOLUTE_DIRECTORY/android-sdk-linux\"" >> $USERS_HOME/.profile
