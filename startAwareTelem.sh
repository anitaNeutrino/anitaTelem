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

if [ "$ANITA_TREE_MAKER_DIR" = "" ]
then
   echo "ANITA_TREE_MAKER_DIR must be set to where the simpleTreeMaker code is"
   exit 1
fi

if [ "$ANITA_AWARE_FILEMAKER_DIR" = "" ]
then
   echo "ANITA_AWARE_FILEMAKER_DIR must be set to where the anitaAwareFileMaker code is"
   exit 1
fi

if [ "$AWARE_SITE_SCRIPT" = "" ]
then
    echo "AWARE_SITE_SCRIPT must be set to start local copy scripts, or an empty script is necessary"
    exit 1
fi

if  test `ps x | grep newTelemFileWatcher.py  | wc -l` -gt 1; then
  echo "newTelemFileWatcher.py is running."
  echo "    You should stop this before trying to start another one"
  exit 1
else
  echo "newTelemFileWatcher.py is not running."
fi


if  test `ps x | grep rootAndJsonFileLoop.sh  | wc -l` -gt 1; then
  echo "rootAndJsonFileLoop.sh is running."
  echo "    You should stop this before trying to start another one"
  exit 1
else
  echo "rootandJsonFileLoop.sh is not running."
fi

echo "Starting newTelemFileWatcher.py"
nohup ./newTelemFileWatcher.py > $AWARE_OUTPUT_DIR/ANITA4/log/filewatcher.log 2>&1 < /dev/null &
echo "Starting rootAndJsonFileLoop.sh"
nohup ./rootAndJsonFileLoop.sh > $AWARE_OUTPUT_DIR/ANITA4/log/rootJson.log 2>&1 < /dev/null &

echo "Starting local script"
nohup ${AWARE_SITE_SCRIPT} > $AWARE_OUTPUT_DIR/ANITA4/log/siteScript.log 2>&1 < /dev/null &
