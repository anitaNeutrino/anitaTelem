#!/bin/bash

#All of these need to be set
export ANITA_UTIL_INSTALL_DIR=/usr/local/anita
export LD_LIBRARY_PATH=${ANITA_UTIL_INSTALL_DIR}/lib:${LD_LIBRARY_PATH};
export PATH=${ANITA_UTIL_INSTALL_DIR}/bin:${PATH}
export AWARE_BASE_DIR=/home/anita/soft/awareTelem
export ANITA_TELEM_DIR=/home/anita/soft/anitaTelem
export ANITA_BIN_DIR=${ANITA_UTIL_INSTALL_DIR}/bin
export AWARE_OUTPUT_DIR=/data/flight1617/aware/output/
export ANITA_TELEM_DATA_DIR=/data/flight1617/telem/
export PYTHONPATH=${AWARE_BASE_DIR}/python/:${ANITA_BASE_DIR}/pyinotify/python2
