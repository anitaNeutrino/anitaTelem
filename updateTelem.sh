#!/bin/bash

while [ 1 ]; do
    
    cd /home/radio/anita14/anitaTelem/
    rm -rf  /anitaStorage/palestine14/telem/raw/run1002/
    ./processTelemFile 1002 /anitaStorage/palestine14/telem/raw_los/00213/*
    
    cd /home/radio/anita14/simpleTreeMaker
    ./runTelemMaker.sh 1002
    
    
    cd ~/anita14/anitaAwareFileMaker/palestine
    ./processTelemRun.sh 1002
    
    echo "Sleeping now"
    sleep 60
done
