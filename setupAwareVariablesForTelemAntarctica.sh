#!/bin/bash

#All of these need to be set
export ANITA_BASE_DIR=/home/anita/Code
export AWARE_BASE_DIR=/home/anita/Code/awareTelem
#export ANITA_AWARE_FILEMAKER_DIR=/home/anita/Code/anitaAwareFileMaker/
#/home/anita/Code/anitaAwareFileMaker/
export ANITA_TELEM_DIR=/home/anita/Code/anitaTelem
#export ANITA_TREE_MAKER_DIR=/usr/local/anita/bin/
export ANITA_BIN_DIR=${ANITA_UTIL_INSTALL_DIR}/bin

export AWARE_OUTPUT_DIR=/data/flight1617/aware/output/
export ANITA_TELEM_DATA_DIR=/data/flight1617/telem/

export AWARE_SITE_SCRIPT=${ANITA_TELEM_DIR}/startLDBCopyScripts.sh

export PYTHONPATH=${AWARE_BASE_DIR}/python/:${ANITA_BASE_DIR}/pyinotify/python2
