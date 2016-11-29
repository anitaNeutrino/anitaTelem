#!/bin/bash
if [ "$2" = "" ]
then
   echo "usage: `basename $0` <start run> <end run>" 1>&2
   exit 1
fi

if [ "$ANITA_TELEM_DATA_DIR" = "" ]
then
   echo "ANITA_TELEM_DATA_DIR must be set to where the raw_los, openport and tdrss directories are"
   exit 1
fi


if [ "$ANITA_BIN_DIR" = "" ]
then
   echo "ANITA_BIN_DIR must be set"
   exit 1
fi


PATH=${ANITA_BIN_DIR}:${PATH}


for run in `seq $1 $2`; do
    rm $ANITA_TELEM_DATA_DIR/root/run${run}/eventHeadFile${run}.root
    rm $ANITA_TELEM_DATA_DIR/root/run${run}/eventFile${run}.root

    runTelemEventMaker.sh ${run} $ANITA_TELEM_DATA_DIR/raw/run${run} $ANITA_TELEM_DATA_DIR/root/run${run}

    
    makeWaveformSummaryJsonFiles $ANITA_TELEM_DATA_DIR/root/run${run}/eventHeadFile${run}.root $ANITA_TELEM_DATA_DIR/root/run${run}/eventFile${run}.root


done




