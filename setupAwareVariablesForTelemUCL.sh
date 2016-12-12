#!/bin/bash
export ANITA_UTIL_INSTALL_DIR=/home/rjn/install;
LD_LIBRARY_PATH=${ANITA_UTIL_INSTALL_DIR}/lib:${LD_LIBRARY_PATH};
export LD_LIBRARY_PATH
export PATH=${ANITA_UTIL_INSTALL_DIR}/bin:${PATH}
#All of these need to be set
export ANITA_BASE_DIR=/home/rjn/anita/
export AWARE_BASE_DIR=/home/rjn/anita/aware
#export ANITA_AWARE_FILEMAKER_DIR=/home/rjn/anita/anitaAwareFileMaker/
export ANITA_TELEM_DIR=/home/rjn/anita/anitaTelem
#export ANITA_TREE_MAKER_DIR=/home/rjn/anita/anitaTreeMaker
export ANITA_BIN_DIR=${ANITA_UTIL_INSTALL_DIR}/bin


export AWARE_OUTPUT_DIR=/unix/anita3/anita4/flight1617/aware/output/
export ANITA_TELEM_DATA_DIR=/unix/anita3/anita4/flight1617/telem/


export PYTHONPATH=${AWARE_BASE_DIR}/python/:${ANITA_BASE_DIR}/pyinotify/python2
