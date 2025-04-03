#! /bin/bash

MODULESUITE_DIR=`pwd`

cd ../EchoTemplates
./UpdateApplicationArchive.sh

cd $MODULESUITE_DIR

#Package the NBMs
ant nbms
