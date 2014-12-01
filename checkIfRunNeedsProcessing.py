#!/usr/bin/env python
"""
This script should check if a telem run needs processing and if it does do the appropriate processing.
"""

import sys
import os
import subprocess


anitaTelemDir = os.getenv('ANITA_TELEM_DIR', "/home/radio/anita14/anitaTelem")
anitaTelemDataDir = os.getenv('ANITA_TELEM_DATA_DIR', "/anitaStorage2/antarctica14/telem")
anitaTreeMakerDir = os.getenv("ANITA_TREE_MAKER_DIR","/home/radio/anita14/simpleTreeMaker")
anitaAwareFilemakerDir= os.getenv("ANITA_AWARE_FILEMAKER_DIR","/home/radio/anita14/anitaAwareFileMaker")

site="antarctica"

def doesRawDirExist(runNum):
    rawDir=anitaTelemDataDir+"/raw/run"+str(runNum)
    if(os.path.isdir(rawDir)):
        return 1
    return 0


def doesRootDirExist(runNum):
    rawDir=anitaTelemDataDir+"/root/run"+str(runNum)
    if(os.path.isdir(rawDir)):
        return 1
    return 0

def getRawTimeModified(runNum,filePath):
    fullDir=anitaTelemDataDir+"/raw/run"+str(runNum)+"/"+filePath
    if(os.path.isfile(fullDir)):
        return os.path.getmtime(fullDir)
    return 0;

def getRootFilename(runNum,fileName):
    return anitaTelemDataDir+"/root/run"+str(runNum)+"/"+fileName+str(runNum)+".root"

def getRootTimeModified(runNum,fileName):    
    rootFile=getRootFilename(runNum,fileName)
    if(os.path.isfile(rootFile)):
        return os.path.getmtime(rootFile)
    return 1

def main():
 
    if(len(sys.argv)<2):
        print __file__+' <run>'
        sys.stdout.flush()
        sys.exit()

    runNum=sys.argv[1]
    
#Step one check raw dir exists
    if not doesRawDirExist(runNum):
        print "Raw dir does not exist for run ",runNum," <- nothing to do ... exitting script"
        sys.stdout.flush()
        sys.exit()

#Step two check root dir exits
    if not doesRootDirExist(runNum):
        print "Root dir does not exist for run ",runNum," <- generating root files"
        sys.stdout.flush()
        processCommand=anitaTreeMakerDir+"/runAnitaIIIFileMakerTelem.sh "+str(runNum)
        print "--> [1 of 2] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand=anitaAwareFilemakerDir+"/"+site+"/processRunTelem.sh "+str(runNum)
        print "--> [1 of 2] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        print "--> root dir generated ... exitting script"                                                            <
        sys.stdout.flush()
        sys.exit()

#Step three check if individual root files need to be remade

    rawDir=anitaTelemDataDir+"/raw/run"+str(runNum)   
    rootDir=anitaTelemDataDir+"/root/run"+str(runNum)
     
    rawTime=getRawTimeModified(runNum,"event/last")
    rootTime=getRootTimeModified(runNum,"eventFile")
    if(rawTime>rootTime):
        print "Need new event ROOT file for run ",runNum," - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on event/last"
        sys.stdout.flush()
        processCommand=anitaTreeMakerDir+"/runTelemEventMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 3] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand=anitaTreeMakerDir+"/runTelemHeaderMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [2 of 3] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        #Here insert call to aware file maker    
        processCommand=anitaAwareFilemakerDir+"/makeHeaderJsonFiles"
        print "--> [3 of 3] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"headFile")])
    else:
        print "event ROOT file for run ",runNum," up-to-date - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on event/last"
        sys.stdout.flush()
      
    rawTime=getRawTimeModified(runNum,"house/hk/last")
    rootTime=getRootTimeModified(runNum,"hkFile")
    if(rawTime>rootTime):
        print "Need new hk ROOT file for run ",runNum," - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/hk/last"
        sys.stdout.flush()
        processCommand=anitaTreeMakerDir+"/runTelemHkMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 3] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        #Here insert call to aware file maker
        processCommand=anitaAwareFilemakerDir+"/makeHkJsonFiles"
        print "--> [2 of 3] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"hkFile")])
        #Here insert call to aware file maker
        processCommand=anitaAwareFilemakerDir+"/makeSSHkJsonFiles"
        print "--> [3 of 3] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"sshkFile")])
    else:
        print "hk ROOT file for run ",runNum," up-to-date - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/hk/last"
        sys.stdout.flush()

    rawTime=getRawTimeModified(runNum,"house/monitor/last")
    rootTime=getRootTimeModified(runNum,"monitorFile")
    if(rawTime>rootTime):
        print "Need new monitor ROOT file for run ",runNum," - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/monitor/last"
        sys.stdout.flush()
        processCommand=anitaTreeMakerDir+"/runTelemMonitorMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 2] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand=anitaTreeMakerDir+"/runTelemOtherMonitorMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [2 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        #Here insert call to aware file maker
        processCommand=anitaAwareFilemakerDir+"/makeMonitorHkJsonFiles"
        print "--> [3 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"monitorFile")])
        processCommand=anitaAwareFilemakerDir+"/makeOtherMonitorHkJsonFiles"
        print "--> [4 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"monitorFile")])
    else:
        print "monitor ROOT file for run ",runNum," up-to-date - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/monitor/last"
        sys.stdout.flush()

    rawTime=getRawTimeModified(runNum,"house/turfhk/last")
    rootTime=getRootTimeModified(runNum,"turfRateFile")
    if(rawTime>rootTime):
        print "Need new monitor ROOT file for run ",runNum," - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/turfhk/last"
        sys.stdout.flush()
        processCommand=anitaTreeMakerDir+"/runTelemTurfRateMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand=anitaTreeMakerDir+"/runTelemSumTurfRateMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [2 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand=anitaAwareFilemakerDir+"/makeTurfRateJsonFiles"
        print "--> [3 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"turfRateFile")])
        processCommand=anitaAwareFilemakerDir+"/makeSumTurfRateJsonFiles"
        print "--> [4 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"sumTurfRateFile")])
    else:
        print "monitor ROOT file for run ",runNum," up-to-date - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/turfhk/last"
        sys.stdout.flush()

    rawTime=getRawTimeModified(runNum,"house/surfhk/last")
    rootTime=getRootTimeModified(runNum,"surfHkFile")
    if(rawTime>rootTime):
        print "Need new surf hk ROOT file for run ",runNum," - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/surfhk/last"
        sys.stdout.flush()
        processCommand=anitaTreeMakerDir+"/runTelemSurfHkMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand=anitaTreeMakerDir+"/runTelemAvgSurfHkMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [2 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand=anitaAwareFilemakerDir+"/makeSurfHkJsonFiles"
        print "--> [3 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"surfHkFile")])
        processCommand=anitaAwareFilemakerDir+"/makeAvgSurfHkJsonFiles"
        print "--> [4 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"avgSurfHkFile")])
    else:
        print "surf hk ROOT file for run ",runNum," up-to-date - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/surfhk/last"
        sys.stdout.flush()


    rawTime=getRawTimeModified(runNum,"start/last")
    rootTime=getRootTimeModified(runNum,"auxFile")
    if(rawTime>rootTime):
        print "Need new aux ROOT file for run ",runNum," - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on start/last"
        sys.stdout.flush()
        processCommand=anitaTreeMakerDir+"/runTelemAuxMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 2] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand=anitaAwareFilemakerDir+"/makeAcqdStartRunJsonFiles"
        print "--> [2 of 2] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"auxFile")])
    else:
        print "aux ROOT file for run ",runNum," up-to-date - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on start/last"
        sys.stdout.flush()

    rawTime=getRawTimeModified(runNum,"house/gps/last")
    rootTime=getRootTimeModified(runNum,"gpsFile")
    if(rawTime>rootTime):
        print "Need new GPS ROOT file"
        processCommand=anitaTreeMakerDir+"/runTelemGpsMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 9] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand=anitaAwareFilemakerDir+"/makeAdu5PatJsonFiles"
        print "--> [2 of 9] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"0"])
        processCommand=anitaAwareFilemakerDir+"/makeAdu5PatJsonFiles"
        print "--> [3 of 9] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"1"])
        processCommand=anitaAwareFilemakerDir+"/makeAdu5SatJsonFiles"
        print "--> [4 of 9] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"0"])
        processCommand=anitaAwareFilemakerDir+"/makeAdu5SatJsonFiles"
        print "--> [5 of 9] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"1"])
        processCommand=anitaAwareFilemakerDir+"/makeAdu5VtgJsonFiles"
        print "--> [6 of 9] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"0"])
        processCommand=anitaAwareFilemakerDir+"/makeAdu5VtgJsonFiles"
        print "--> [7 of 9] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"1"])
      
        processCommand=anitaAwareFilemakerDir+"/makeG12PosJsonFiles"
        print "--> [8 of 9] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile")])
        processCommand=anitaAwareFilemakerDir+"/makeG12SatJsonFiles"
        print "--> [9 of 9] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile")])
    else:
        print "GPS ROOT file for run ",runNum," up-to-date - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/gps/last"
        sys.stdout.flush()


if __name__ == "__main__":
    main()
