#!/bin/bash

echo "Starting the copy scripts needed for AWARE in Palestine"


if [ -f "/tmp/pidCmdCopy" ]; then
  echo "Old cmdCopy PID is " `cat /tmp/pidCmdCopy`
  kill `cat /tmp/pidCmdCopy`
  sleep 1
fi

if  test `ps x | grep copyCmdSendFromTdrss.sh  | wc -l` -gt 1; then
    echo "copyCmdSendFromTdrss.sh is running."
      echo "    You should stop this before trying to start another one"
        exit 1
      else
          echo "copyCmdSendFromTdrss.sh is not running."
        fi

nohup /home/anita/Code/anitaTelem/copyCmdSendFromTdrss.sh > $AWARE_OUTPUT_DIR/ANITA4/log/cmdCopy.log 2>&1 < /dev/null &


# Now start TDRSS Copy Script

if [ -f "/tmp/pidTDRSSCopy" ]; then
    echo "Old TDRSS Copy PID is " `cat /tmp/pidTDRSSCopy`
    kill `cat /tmp/pidTDRSSCopy`
    sleep 1
fi

if  test `ps x | grep streamTDRSSdata.sh  | wc -l` -gt 1; then
    echo "streamTDRSSdata.sh is running."
    echo "    You should stop this before trying to start another one"
    exit 1

else
    echo "streamTDRSSdata.sh is not running."
fi

nohup /home/anita/Code/anitaAwareFileMaker/antarctica/copyScripts/streamTDRSSdata.sh > $AWARE_OUTPUT_DIR/ANITA4/log/tdrssCopy.log 2>&1 < /dev/null &



# Now start LOS Copy Script

if [ -f "/tmp/pidLOSCopy" ]; then
    echo "Old LOS Copy PID is " `cat /tmp/pidLOSCopy`
    kill `cat /tmp/pidLOSCopy`
    sleep 1
fi

if  test `ps x | grep streamLOSdata.sh  | wc -l` -gt 1; then
    echo "streamLOSdata.sh is running."
    echo "    You should stop this before trying to start another one"
    exit 1

else
    echo "streamLOSdata.sh is not running."
fi

nohup /home/anita/Code/anitaAwareFileMaker/antarctica/copyScripts/streamLOSdata.sh > $AWARE_OUTPUT_DIR/ANITA4/log/losCopy.log 2>&1 < /dev/null &



# Now start Openport Copy Script

echo "Not bothering to start openport copying script"
exit 1

if [ -f "/tmp/pidOpenportCopy" ]; then
    echo "Old Openport Copy PID is " `cat /tmp/pidOpenportCopy`
    kill `cat /tmp/pidOpenportCopy`
    sleep 1
fi

if  test `ps x | grep streamOpenportDataSouth.sh  | wc -l` -gt 1; then
    echo "streamOpenportDataSouth.sh is running."
    echo "    You should stop this before trying to start another one"
    exit 1

else
    echo "streamOpenportDataSouth.sh is not running."
fi

nohup /home/anita/Code/anitaAwareFileMaker/antarctica/copyScripts/streamOpenportDataSouth.sh > $AWARE_OUTPUT_DIR/ANITA4/log/openportCopy.log 2>&1 < /dev/null &



