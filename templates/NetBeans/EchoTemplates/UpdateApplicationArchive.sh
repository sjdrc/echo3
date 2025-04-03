#! /bin/bash

START_DIRECTORY=`pwd`
PROJECTNAME="EchoApplicationProject"
DESTINATION_ZIP="$START_DIRECTORY/src/au/net/emblem/echotemplates/application/$PROJECTNAME.zip"
SOURCE_DIR="$START_DIRECTORY/../$PROJECTNAME"
cd $SOURCE_DIR
echo Cleaning $PROJECTNAME Template
ls
rm -r build
rm -r dist
rm nbproject/private/private.xml
rm nbproject/private/cpp_standard_headers_indexer.cpp
rm nbproject/private/c_standard_headers_indexer.c
rm .dep.inc
echo Creating $DESTINATION_ZIP.zip
zip -r $DESTINATION_ZIP .
cd $START_DIRECTORY

PROJECTNAME="ThirdPersonGame"
DESTINATION_ZIP="$START_DIRECTORY/src/au/net/emblem/echotemplates/thirdpersongame/$PROJECTNAME.zip"
SOURCE_DIR="$START_DIRECTORY/../$PROJECTNAME"
cd $SOURCE_DIR
echo Cleaning $PROJECTNAME Template
ls
rm -r build
rm -r dist
rm nbproject/private/private.xml
rm nbproject/private/cpp_standard_headers_indexer.cpp
rm nbproject/private/c_standard_headers_indexer.c
rm .dep.inc
echo Creating $DESTINATION_ZIP.zip
zip -r $DESTINATION_ZIP .
cd $START_DIRECTORY
