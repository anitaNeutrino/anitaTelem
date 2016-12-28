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

nohup /home/anita/soft/anitaTelem/copyCmdSendFromTdrss.sh > $AWARE_OUTPUT_DIR/ANITA4/log/cmdCopy.log 2>&1 < /dev/null &


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

nohup /home/anita/soft/anitaAwareFileMaker/telemScripts/copyScripts/streamTDRSSdata.sh > $AWARE_OUTPUT_DIR/ANITA4/log/tdrssCopy.log 2>&1 < /dev/null &



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

nohup /home/anita/soft/anitaAwareFileMaker/telemScripts/copyScripts/streamLOSdata.sh > $AWARE_OUTPUT_DIR/ANITA4/log/losCopy.log 2>&1 < /dev/null &



# Now start SLOW Copy Script

if [ -f "/tmp/pidSlowCopy" ]; then
    echo "Old SLOW Copy PID is " `cat /tmp/pidLOSCopy`
    kill `cat /tmp/pidSlowCopy`
    sleep 1
fi

if  test `ps x | grep streamSlowRate.sh  | wc -l` -gt 1; then
    echo "streamSlowRate.sh is running."
    echo "    You should stop this before trying to start another one"
    exit 1

else
    echo "streamSlowRate.sh is not running."
fi

nohup /home/anita/soft/anitaAwareFileMaker/telemScripts/copyScripts/streamSlowRate.sh > $AWARE_OUTPUT_DIR/ANITA4/log/slowCopy.log 2>&1 < /dev/null &



