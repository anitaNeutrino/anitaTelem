#!/bin/bash
source /home/anita/Code/anitaTelem/setupAwareVariablesForTelemAntarctica.sh

echo "PID = $$"
echo $$ > /tmp/pidCmdCopy

TDRSS_HOST=tdrss1
TDRSS_USER=anita
TDRSS_DIR=/home/anita/cmdSend/jsonLog/
TDRSS_DEST_DIR=$AWARE_OUTPUT_DIR/ANITA4/cmdSend/palestine
TDRSS_SRC_DIR=${TDRSS_USER}@${TDRSS_HOST}:${TDRSS_DIR}/*.json

LOS_HOST=localhost
LOS_USER=anita
LOS_DIR=/home/anita/cmdSend/jsonLog/
LOS_DEST_DIR=$AWARE_OUTPUT_DIR/ANITA4/cmdSend/los
LOS_SRC_DIR=${LOS_USER}@${LOS_HOST}:${LOS_DIR}/*.json

mkdir -p ${LOS_DEST_DIR}
mkdir -p ${TDRSS_DEST_DIR}

echo "$LOS_SRC_DIR"
echo "$LOS_DEST_DIR"


while [ 1 ]; do
    date
    rsync -av $TDRSS_SRC_DIR $TDRSS_DEST_DIR
    rsync -av $LOS_SRC_DIR $LOS_DEST_DIR
    sleep 30
done
