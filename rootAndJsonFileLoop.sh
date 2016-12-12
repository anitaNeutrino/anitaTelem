#!/bin/bash

if [ "$AWARE_OUTPUT_DIR" = "" ]
then
   echo "AWARE_OUTPUT_DIR must be set to where the output directory is"
   exit 1
fi

if [ "$ANITA_TELEM_DATA_DIR" = "" ]
then
   echo "ANITA_TELEM_DATA_DIR must be set to where the raw_los,openport and tdrss directories are"
   exit 1
fi

if [ "$ANITA_TELEM_DIR" = "" ]
then
   echo "ANITA_TELEM_DIR must be set to where the anitaTelem library is"
   exit 1
fi

if [ "$ANITA_BIN_DIR" = "" ]
then
   echo "ANITA_BIN_DIR must be set to where the ANITA binaries are"
   exit 1
fi

mkdir -p $AWARE_OUTPUT_DIR/ANITA4/log

while [ 1 ]; do
    
    COUNT=0
    
    RUN_LIST=`mktemp`
    RUN_LIST2=`mktemp`
    for rundir in $ANITA_TELEM_DATA_DIR/raw/run*; 
    do 
	echo ${rundir#*run} >> $RUN_LIST
    done 
    
    cat $RUN_LIST | sort -nr > $RUN_LIST2
    
#    cat $RUN_LIST2

    for run in `cat $RUN_LIST2`;
    do
	$ANITA_TELEM_DIR/checkIfRunNeedsProcessing.py $run > $AWARE_OUTPUT_DIR/ANITA4/log/rootAndJson.log 2>&1
	if  test `cat $AWARE_OUTPUT_DIR/ANITA4/log/rootAndJson.log | wc -l` -gt 10 ; then
	    echo "Processed run $run -- COUNT $COUNT"
	    let COUNT=COUNT+1 
	    if [ $COUNT -gt 4 ]; then
		break;
	    fi
	else 
	    echo "Already done run $run"
	fi	
    done
    rm $RUN_LIST2 $RUN_LIST
    sleep 10
done

