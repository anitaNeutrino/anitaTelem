#!/bin/bash

if [ "$1" == "" ]
then
    echo "usage `basename $0` <los run>"
    exit 0
fi

LOS_RUN=$1


    
cd /home/radio/anita14/anitaTelem/
lastNum=0
while [ 1 ]; do
#    echo "Here";
    gotData=0
    for num in $(seq -f "%06g" $lastNum 9999); do
	echo $num
	if [ -f "/anitaStorage/antarctica14/telem/raw_los/00${LOS_RUN}/${num}" ]; then
	    echo "Here" $num
	    lastNum=$num
	    gotData=1
	    ./processTelemFile  /anitaStorage/antarctica14/telem/raw_los/00${LOS_RUN}/${num}
	else
	    break;
	fi	
    done

    sleep 60
 done   
