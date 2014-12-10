#!/bin/bash

#All of these need to be set
export ANITA_BASE_DIR=/home/radio/anita14
export AWARE_BASE_DIR=/home/radio/anita14/awareTelem
export ANITA_AWARE_FILEMAKER_DIR=/home/radio/anita14/anitaAwareFileMaker/
export ANITA_TELEM_DIR=/home/radio/anita14/anitaTelem
export ANITA_TREE_MAKER_DIR=/home/radio/anita14/simpleTreeMaker



export AWARE_OUTPUT_DIR=/storage/flight1415/telem/aware/output/
export ANITA_TELEM_DATA_DIR=/storage/flight1415/telem/


export PYTHONPATH=${AWARE_BASE_DIR}/python/:${ANITA_BASE_DIR}/pyinotify/python2
