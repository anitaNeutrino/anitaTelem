#!/bin/bash

#All of these need to be set
export ANITA_BUILD_TOOL_DIR=/home/radio/anitaCode/anitaBuildTool
export ANITA_BASE_DIR=${ANITA_BASE_DIR}
export AWARE_BASE_DIR=${ANITA_BUILD_TOOL_DIR}/components/aware
export ANITA_AWARE_FILEMAKER_DIR=${ANITA_BUILD_TOOL_DIR}/components/anitaAwareFileMaker/
export ANITA_TELEM_DIR=${ANITA_BUILD_TOOL_DIR}/components/anitaTelem


#This should be ANITA_UTIL_INSTAll_DIR/bin
export ANITA_BIN_DIR=/usr/local/anita/bin/

#This is the output directory which will need to be mounted on the web server
export AWARE_OUTPUT_DIR=/nasc/aware/output/

#This is the input directory which telemetry things will be copied to
export ANITA_TELEM_DATA_DIR=/nasc/ldb2016/telem/

#This is a local site specifc script that may or may not be needed
export AWARE_SITE_SCRIPT=${ANITA_TELEM_DIR}/startAnitaRFCopyScripts.sh

export PYTHONPATH=${AWARE_BASE_DIR}/python/:${ANITA_BUILD_TOOL_DIR}/components/pyinotify/python2
