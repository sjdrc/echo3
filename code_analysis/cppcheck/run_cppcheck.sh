#!/bin/bash

# This script runs Cppcheck, an open source static code analysis tool for C++ projects, over the Echo project with the desired configuration.
# See https://developer.emblem.net.au/w/tools/cppcheck/ and http://cppcheck.sourceforge.net/

cd ${ECHO_ENGINE_INSTALL_DIR}/code_analysis/cppcheck

# TODO: Add environment variable for ECHO_CPPCHECK_INSTALL_LOCATION or such, then test it:
# TODO: 	if not ECHO_CPPCHECK_INSTALL_LOCATION exists
# TODO: 		run install_cppcheck.sh
# TODO: 	run cppcheck using ECHO_CPPCHECK_INSTALL_LOCATION

CPPCHECK_VERSION="$(./cppcheck_build/cppcheck --version)"
OUTPUT_XML_FILE="echo_analysis.xml"

echo Running ${CPPCHECK_VERSION} on Echo and saving to ${OUTPUT_XML_FILE}...

# Don't use "-I /usr/include" because that takes forever
# T290: #define BOOST_VERSION manually because it will #error if it does not find it, thus giving poor results
./cppcheck_build/cppcheck --enable=all -DBOOST_VERSION=105400 --error-exitcode=2 --std=c++03 --xml-version=2 --force --library=${ECHO_ENGINE_INSTALL_DIR}/code_analysis/cppcheck/echo_cppcheck.cfg -I ${ECHO_ENGINE_INSTALL_DIR}/include ${ECHO_ENGINE_INSTALL_DIR}/src 2> ${OUTPUT_XML_FILE}

echo ${CPPCHECK_VERSION} static code analysis results saved to ${OUTPUT_XML_FILE}
