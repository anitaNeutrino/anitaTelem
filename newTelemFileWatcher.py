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

# The watch manager stores the watches and provides operations on watches
wm = pyinotify.WatchManager()

# Mask for the two events that we will watch for
mask = pyinotify.IN_DELETE | pyinotify.IN_CREATE | pyinotify.IN_MOVED_TO   # watched events

anitaTelemDir = os.getenv('ANITA_TELEM_DIR', "/home/radio/anita14/anitaTelem")
anitaTelemDataDir = os.getenv('ANITA_TELEM_DATA_DIR', "/anitaStorage/antarctica14/telem")
awareOutputDir = os.getenv('AWARE_OUTPUT_DIR',"/anitaStorage/antarctica14/telem")
print "Using ANITA_TELEM_DIR="+anitaTelemDir
print "Using ANITA_TELEM_DATA_DIR="+anitaTelemDataDir
processTelemCommand=anitaTelemDir+"/processTelemFile"
print "Telem processing command: "+processTelemCommand
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
        processLogFile=open(awareOutputDir+"/ANITA3/log/processTelem.log","w")
                    
        print ["processTelemFile.sh",event.pathname]
        subprocess.call([processTelemCommand,event.pathname],stdout=processLogFile,stderr=processLogFile)

    def process_IN_CREATE(self, event):
        # # print out the message and the name of the file in question
        print "process_IN_CREATE:", event.pathname
        if(os.path.isdir(event.pathname)):
            print "New directory detected:", event.pathname, "will watch it"
            wdd = wm.add_watch(event.pathname, mask, rec=False)

    def process_IN_DELETE(self, event):
        print "Removing:", event.pathname
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

    # Start watching a new directory
    losDirList=os.listdir(losDir)
    currentLosDir=losDir+losDirList[len(losDirList)-1]
    if(os.path.isfile(currentLosDir)):
        currentLosDir=losDir+losDirList[len(losDirList)-2]
        


    print currentLosDir
    tdrssDirList=os.listdir(tdrssDir)
    currentTdrssDir=tdrssDir+tdrssDirList[len(tdrssDirList)-1]
    print currentTdrssDir
    openportDirList=os.listdir(openportDir)
    currentOpenportDir=openportDir+openportDirList[len(openportDirList)-1]
    print currentOpenportDir
    #Need to watch the directories for new runs
    wdd4 = wm.add_watch(openportDir, mask, rec=False)
    wdd5 = wm.add_watch(losDir, mask, rec=False)
    wdd6 = wm.add_watch(tdrssDir, mask, rec=False)

    #Need to watch the most recent directories for new files
    wdd1 = wm.add_watch(currentOpenportDir, mask, rec=False)
    wdd2 = wm.add_watch(currentLosDir, mask, rec=False)
    wdd3 = wm.add_watch(currentTdrssDir, mask, rec=False)
    # Loop forever catching and dealing wth the events
    notifier.loop()

if __name__ == "__main__":
    main()
