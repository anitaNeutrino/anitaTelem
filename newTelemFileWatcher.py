#!/usr/bin/env python
'''
Created on 23 Oct 2014

This script watches the telemetry directories and calls processTelemFile whenever a new TDRSS, LOS or Openport file is received. The selection of environmental variables need to be set in order for this to work.


@author: Chris Davenport, since then heavily modified by Ryan Nichol
'''
# Mostly based on the examples given in the pyinotify github

import pyinotify
import subprocess
import os
import sys
import re

# The watch manager stores the watches and provides operations on watches
wm = pyinotify.WatchManager()

# Mask for the two events that we will watch for
mask = pyinotify.IN_DELETE | pyinotify.IN_CREATE | pyinotify.IN_MOVED_TO   # watched events

anitaTelemDir = os.getenv('ANITA_TELEM_DIR', "/home/radio/anita14/anitaTelem")
anitaTelemDataDir = os.getenv('ANITA_TELEM_DATA_DIR', "/anitaStorage/antarctica14/telem")
awareOutputDir = os.getenv('AWARE_OUTPUT_DIR',"/anitaStorage/antarctica14/telem")
print "Using ANITA_TELEM_DIR="+anitaTelemDir
sys.stdout.flush()
print "Using ANITA_TELEM_DATA_DIR="+anitaTelemDataDir
sys.stdout.flush()
processTelemCommand=anitaTelemDir+"/processTelemFile"
print "Telem processing command: "+processTelemCommand
sys.stdout.flush()

NameOfStorCommentsFile = "/stor_comments$"
print "Ignoring stor_comments files via RegEx match to "+NameOfStorCommentsFile
stor_comments_filename_re = re.compile( NameOfStorCommentsFile )

def dot_filter(pathname):
    # return True to stop processing of event (to "stop chaining")
    return os.path.basename(pathname)[0]=='.'

# Class to handle the events thrown by the framework
class EventHandler(pyinotify.ProcessEvent):
    def __call__(self, event):
        if not dot_filter(event.name):
            super(EventHandler, self).__call__(event)

    def process_IN_MOVED_TO(self, event):
        # # print out the message and the name of the file in question
        print "Moved file detected:", event.pathname, " launching telem...."
        sys.stdout.flush()
        processLogFile=open(awareOutputDir+"/ANITA4/log/processTelem.log","w")

        print ["processTelemFile.sh",event.pathname]
        sys.stdout.flush()
        subprocess.call([processTelemCommand,event.pathname],stdout=processLogFile,stderr=processLogFile)

    def process_IN_CREATE(self, event):
        # # print out the message and the name of the file in question
        print "process_IN_CREATE:", event.pathname
        sys.stdout.flush()
        if(os.path.isdir(event.pathname)):
            print "New directory detected:", event.pathname, "will watch it"
            sys.stdout.flush()
            wdd = wm.add_watch(event.pathname, mask, rec=False)
        else :
            match_check = stor_comments_filename_re.search( event.pathname )
            if ( match_check ) :                                                          # no match
               print "New file detected:", event.pathname, " <- match to stor_comments ... ignoring it ..."
               sys.stdout.flush()
            else :
               print "New file detected:", event.pathname, " launching telem...."
               sys.stdout.flush()
               processLogFile=open(awareOutputDir+"/ANITA4/log/processTelem.log","w")
               print ["processTelemFile.sh",event.pathname]
               sys.stdout.flush()
               subprocess.call([processTelemCommand,event.pathname],stdout=processLogFile,stderr=processLogFile)

    def process_IN_DELETE(self, event):
        print "Removing:", event.pathname
        sys.stdout.flush()
        # print out the message and the name of the file in question
   
def main():      

    # Start a new handler object
    handler = EventHandler()
    # Add a notifier to the event handler to pass the event through to the event handler
    notifier = pyinotify.Notifier(wm, handler)
    # Internally, 'handler' is a callable object which on new events will be called like this: handler(new_event)
 
    losDir=anitaTelemDataDir+"raw_los/"
    tdrssDir=anitaTelemDataDir+"fast_tdrss/"
    openportDir=anitaTelemDataDir+"openport/"
    slowTdrssDir=anitaTelemDataDir+"slow_tdrss/"
    iridiumDir=anitaTelemDataDir+"iridium/"

    # Start watching a new directory
    losDirList=os.listdir(losDir)
    losDirList.sort()
    currentLosDir=losDir+losDirList[len(losDirList)-1]
    if(os.path.isfile(currentLosDir)):
        currentLosDir=losDir+losDirList[len(losDirList)-2]
    print currentLosDir
    print "config: currentLosDir      = ",currentLosDir
    sys.stdout.flush()

    tdrssDirList=os.listdir(tdrssDir)
    tdrssDirList.sort()
    currentTdrssDir=tdrssDir+tdrssDirList[len(tdrssDirList)-1]
    if(os.path.isfile(currentTdrssDir)):
        currentTdrssDir=tdrssDir+tdrssDirList[len(tdrssDirList)-2]
    print "config: currentTdrssDir    = ",currentTdrssDir
    sys.stdout.flush()

    iridiumDirList=os.listdir(iridiumDir)
    iridiumDirList.sort()
    currentIridiumDir=iridiumDir+iridiumDirList[len(iridiumDirList)-1]
    if(os.path.isfile(currentIridiumDir)):
        currentIridiumDir=iridiumDir+iridiumDirList[len(iridiumDirList)-2]
    print "config: currentIridiumDir    = ",currentIridiumDir
    sys.stdout.flush()

    slowTdrssDirList=os.listdir(slowTdrssDir)
    slowTdrssDirList.sort()
    currentSlowTdrssDir=slowTdrssDir+slowTdrssDirList[len(slowTdrssDirList)-1]
    if(os.path.isfile(currentSlowTdrssDir)):
        currentSlowTdrssDir=slowTdrssDir+slowTdrssDirList[len(slowTdrssDirList)-2]
    print "config: currentTdrssDir    = ",currentTdrssDir
    sys.stdout.flush()

    openportDirList=os.listdir(openportDir)
    openportDirList.sort()
    currentOpenportDir=openportDir+openportDirList[len(openportDirList)-1]
    print "config: currentOpenportDir = ",currentOpenportDir
    sys.stdout.flush()

    #Need to watch the directories for new runs
    wdd4 = wm.add_watch(openportDir, mask, rec=False)
    print "WatchSet: dir ", openportDir
    sys.stdout.flush()
    wdd5 = wm.add_watch(losDir, mask, rec=False)
    print "WatchSet: dir ", losDir
    sys.stdout.flush()
    wdd6 = wm.add_watch(tdrssDir, mask, rec=False)
    print "WatchSet: dir ", tdrssDir
    sys.stdout.flush()
    wdd7 = wm.add_watch(slowTdrssDir, mask, rec=False)
    print "WatchSet: dir ", tdrssDir
    sys.stdout.flush()
    wdd8 = wm.add_watch(iridiumDir, mask, rec=False)
    print "WatchSet: dir ", tdrssDir
    sys.stdout.flush()

    #Need to watch the most recent directories for new files
    wdd1 = wm.add_watch(currentOpenportDir, mask, rec=False)
    print "WatchSet: dir ", currentOpenportDir
    sys.stdout.flush()
    wdd2 = wm.add_watch(currentLosDir, mask, rec=False)
    print "WatchSet: dir ", currentLosDir
    sys.stdout.flush()
    wdd3 = wm.add_watch(currentTdrssDir, mask, rec=False)
    print "WatchSet: dir ", currentTdrssDir
    sys.stdout.flush()
    wdd3 = wm.add_watch(currentSlowTdrssDir, mask, rec=False)
    print "WatchSet: dir ", currentSlowTdrssDir
    sys.stdout.flush()
    wdd3 = wm.add_watch(currentIridiumDir, mask, rec=False)
    print "WatchSet: dir ", currentIridiumDir
    sys.stdout.flush()
    # Loop forever catching and dealing wth the events
    notifier.loop()

if __name__ == "__main__":
    main()

