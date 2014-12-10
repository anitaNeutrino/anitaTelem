#!/bin/bash
source /home/radio/anita14/anitaTelem/setupAwareVariablesForTelem.sh
TDRSS_HOST=192.168.1.6
TDRSS_USER=anita
TDRSS_DIR=/home/anita/cmdSend/jsonLog/
DEST_DIR=$AWARE_OUTPUT_DIR/ANITA3/cmdSend/tdrss/
SRC_DIR=${TDRSS_USER}@${TDRSS_HOST}:${TDRSS_DIR}/*.json


while [ 1 ]; do
    rsync -av $SRC_DIR $DEST_DIR
    sleep 30
done
