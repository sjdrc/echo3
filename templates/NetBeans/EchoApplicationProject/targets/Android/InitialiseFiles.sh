#! /bin/bash

ECHO_ANDROID_TEMPLATE_APPLICATION_LOWER_CLASS_NAME="myapplication"
ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME="MyApplication"
ECHO_ANDROID_TEMPLATE_APPLICATION_DISPLAY_NAME="My Application"
ECHO_ANDROID_TEMPLATE_PACKAGE_NAME="au.com.emblementertainment.myapplication"

######################
#Use package name to generate a path.
######################

echo "Calculating package path frame dot notation..."
echo $ECHO_ANDROID_TEMPLATE_PACKAGE_NAME > pathtemp
export ECHO_ANDROID_TEMPLATE_PACKAGE_PATH=`sed "s/\./\//g" pathtemp`
rm pathtemp
echo "$ECHO_ANDROID_TEMPLATE_PACKAGE_NAME > $ECHO_ANDROID_TEMPLATE_PACKAGE_PATH"

echo "Checking directories exist."
######################
#make sure the directories we need exist
######################
mkdir -p src/$ECHO_ANDROID_TEMPLATE_PACKAGE_PATH
mkdir -p src/au/com/emblementertainment

echo "Parsing files."
######################
#Parse files
######################

echo "Android.mk"
sed -i "s/ECHO_ANDROID_TEMPLATE_APPLICATION_LOWER_CLASS_NAME/$ECHO_ANDROID_TEMPLATE_APPLICATION_LOWER_CLASS_NAME/g" jni/Android.mk
sed -i "s/ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME/$ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME/g" jni/Android.mk

echo "res/values/strings.xml"
sed -i "s/ECHO_ANDROID_TEMPLATE_APPLICATION_DISPLAY_NAME/$ECHO_ANDROID_TEMPLATE_APPLICATION_DISPLAY_NAME/g" res/values/strings.xml

echo "AndroidManifest.xml"
sed -i "s/ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME/$ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME/g" AndroidManifest.xml
sed -i "s/ECHO_ANDROID_TEMPLATE_PACKAGE_NAME/$ECHO_ANDROID_TEMPLATE_PACKAGE_NAME/g" AndroidManifest.xml

echo "build.xml"
sed -i "s/ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME/$ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME/g" build.xml

echo "InstallAndRun.sh"
sed -i "s/ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME/$ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME/g" InstallAndRun.sh
sed -i "s/ECHO_ANDROID_TEMPLATE_PACKAGE_NAME/$ECHO_ANDROID_TEMPLATE_PACKAGE_NAME/g" InstallAndRun.sh

echo "ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME.java"
sed -i "s/ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME/$ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME/g" ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME.java
sed -i "s/ECHO_ANDROID_TEMPLATE_PACKAGE_NAME/$ECHO_ANDROID_TEMPLATE_PACKAGE_NAME/g" ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME.java

#Update the sdk-location in this file.
#This assumes that the AndroidDevelopment folder is next to echo3.
echo "Calculating escaped ECHO_ENGINE_INSTALL_DIR path."
echo $ECHO_ENGINE_INSTALL_DIR > escapedpathtemp
export ECHO_ENGINE_INSTALL_DIR_ESCAPED=`sed "s/\\//\\\\\\\\\\//g" escapedpathtemp`
rm escapedpathtemp
echo "$ECHO_ENGINE_INSTALL_DIR > $ECHO_ENGINE_INSTALL_DIR_ESCAPED"

#Move files to appropriate locations
echo "Moving files"
mv -v ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME.java src/$ECHO_ANDROID_TEMPLATE_PACKAGE_PATH/$ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME.java
mv -v jni/ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME.cpp jni/$ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME.cpp

#Now symlink Echo's java files
echo "Creating symbolic links..."
ln -v -s $ECHO_ENGINE_INSTALL_DIR/src/Platforms/Android/java/au/com/emblementertainment/echo src/au/com/emblementertainment
ln -v -s ../../../resources assets
