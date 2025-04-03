#!/bin/bash

#Install the package
ant installd

if [ ! $? -eq 0 ]; then
	exit 1;
fi

#Run the package
adb shell am start -n ECHO_ANDROID_TEMPLATE_PACKAGE_NAME/ECHO_ANDROID_TEMPLATE_PACKAGE_NAME.ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME

if [ ! $? -eq 0 ]; then
	exit 1;
fi

