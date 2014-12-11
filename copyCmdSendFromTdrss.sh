#!/bin/bash
source /home/radio/anita14/anitaTelem/setupAwareVariablesForTelem.sh
TDRSS_HOST=tdrss
TDRSS_USER=anita
TDRSS_DIR=/home/anita/cmdSend/jsonLog/
TDRSS_DEST_DIR=$AWARE_OUTPUT_DIR/ANITA3/cmdSend/palestine
TDRSS_SRC_DIR=${TDRSS_USER}@${TDRSS_HOST}:${TDRSS_DIR}/*.json

LOS_HOST=192.168.1.6
LOS_USER=anita
LOS_DIR=/home/anita/cmdSend/jsonLog/
LOS_DEST_DIR=$AWARE_OUTPUT_DIR/ANITA3/cmdSend/los
LOS_SRC_DIR=${LOS_USER}@${LOS_HOST}:${LOS_DIR}/*.json


while [ 1 ]; do
    date
    rsync -av $TDRSS_SRC_DIR $TDRSS_DEST_DIR
    rsync -av $LOS_SRC_DIR $LOS_DEST_DIR
    sleep 30
done
