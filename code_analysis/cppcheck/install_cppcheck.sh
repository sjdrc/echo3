#!/bin/bash

# This script installs the latest version of Cppcheck, an open source static code analysis tool for C++ projects.
# See https://developer.emblem.net.au/w/tools/cppcheck/ and http://cppcheck.sourceforge.net/

echo Installing Git version control...
sudo apt-get install git
cd ${ECHO_ENGINE_INSTALL_DIR}/code_analysis/cppcheck/

echo Downloading Cppcheck source code repository using Git...
git clone git://github.com/danmar/cppcheck.git cppcheck_build

echo Compiling Cppcheck...
# Needs C++11 compatible compiler. g++, et cetera, presumed already installed for Echo
cd cppcheck_build
make SRCDIR=build

