#!/bin/bash

cd targets/Android

if [ -e "InitialiseFiles.sh" ] ; then
	echo "Template has not been initialised. Running initialisation script..."
	./InitialiseFiles.sh
	echo "Removing initialisation script."
	rm InitialiseFiles.sh
fi

echo "Setting Echo links."
#Make sure the Echo Android java folder is linked correctly.
mkdir -p src/au/com/emblementertainment
ln -s -f $ECHO_ENGINE_INSTALL_DIR/src/Platforms/Android/java/au/com/emblementertainment/echo src/au/com/emblementertainment/
ln -v -s -f ../../../resources assets

echo "Generating local.properties."
#Set the SDK location but only if our installation environment includes the configuration.
if [ ! "$ECHO_ANDROID_SDK" = "" ] ; then
	echo "sdk.dir=$ECHO_ANDROID_SDK">local.properties
else
	echo "Error: Could not generate local.properties. ECHO_ANDROID_SDK is not defined."
	exit 1;
fi

# Package the library 
ndk-build
if [ ! $? -eq 0 ]; then
	exit 1;
fi

ant debug

if [ ! $? -eq 0 ]; then
	exit 1;
fi
