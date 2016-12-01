#!/usr/bin/env python
"""
This script should check if a telem run needs processing and if it does do the appropriate processing.
"""

import sys
import os
import subprocess


anitaTelemDir = os.getenv('ANITA_TELEM_DIR', "/home/radio/anita14/anitaTelem")
anitaTelemDataDir = os.getenv('ANITA_TELEM_DATA_DIR', "/anitaStorage2/antarctica14/telem")
#anitaAwareFilemakerDir= os.getenv("ANITA_AWARE_FILEMAKER_DIR","/home/radio/anita14/anitaAwareFileMaker")

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
        processCommand="runAnita4FileMakerTelem.sh "+str(runNum)
        print "--> [1 of 2] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand="processRunTelem.sh "+str(runNum)
        print "--> [1 of 2] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        print "--> root dir generated ... exitting script"             
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
        processCommand="runTelemEventMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand="runTelemHeaderMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [2 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        #Here insert call to aware file maker    
        processCommand="makeHeaderJsonFiles"
        print "--> [3 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"headFile")])
        #Now make the waveform summary files
        processCommand="makeWaveformSummaryJsonFiles"
        print "--> [4 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"eventHeadFile"),getRootFilename(runNum,"eventFile")])
    else:
        print "event ROOT file for run ",runNum," up-to-date - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on event/last"
        sys.stdout.flush()
      
    rawTime=getRawTimeModified(runNum,"house/hk/last")
    rootTime=getRootTimeModified(runNum,"hkFile")
    if(rawTime>rootTime):
        print "Need new hk ROOT file for run ",runNum," - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/hk/last"
        sys.stdout.flush()
        processCommand="runTelemHkMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 3] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        #Here insert call to aware file maker
        processCommand="makeHkJsonFiles"
        print "--> [2 of 3] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"hkFile")])
        #Here insert call to aware file maker
        processCommand="makeSSHkJsonFiles"
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
        processCommand="runTelemMonitorMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 2] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand="runTelemOtherMonitorMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [2 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        #Here insert call to aware file maker
        processCommand="makeMonitorHkJsonFiles"
        print "--> [3 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"monitorFile")])
        processCommand="makeOtherMonitorHkJsonFiles"
        print "--> [4 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"monitorFile")])
    else:
        print "monitor ROOT file for run ",runNum," up-to-date - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/monitor/last"
        sys.stdout.flush()


    rawTime=getRawTimeModified(runNum,"house/gpu/last")
    rootTime=getRootTimeModified(runNum,"gpuFile")
    if(rawTime>rootTime):
        print "Need new gpu ROOT file for run ",runNum," - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/gpu/last"
        sys.stdout.flush()
        processCommand="runTelemGpuMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 2] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        #Here insert call to aware file maker
        processCommand="makeGpuJsonFiles"
        print "--> [2 of 2] - running ",processCommand
        sys.stdout.flush()
#RJN hack        subprocess.call([processCommand,getRootFilename(runNum,"gpuFile")])
    else:
        print "gpu ROOT file for run ",runNum," up-to-date - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/gpu/last"
        sys.stdout.flush()

    rawTime=getRawTimeModified(runNum,"house/tuff/last")
    rootTime=getRootTimeModified(runNum,"tuffFile")
    if(rawTime>rootTime):
        print "Need new tuff ROOT file for run ",runNum," - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/tuff/last"
        sys.stdout.flush()
        processCommand="runTelemTuffStatusMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 2] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        #Here insert call to aware file maker
        processCommand="makeTuffStatusJsonFiles"
        print "--> [2 of 2] - running ",processCommand
        sys.stdout.flush()
#RJN hack        subprocess.call([processCommand,getRootFilename(runNum,"tuffFile")])
    else:
        print "tuff ROOT file for run ",runNum," up-to-date - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/tuff/last"
        sys.stdout.flush()



    rawTime=getRawTimeModified(runNum,"house/turfhk/last")
    rootTime=getRootTimeModified(runNum,"turfRateFile")
    if(rawTime>rootTime):
        print "Need new monitor ROOT file for run ",runNum," - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/turfhk/last"
        sys.stdout.flush()
        processCommand="runTelemTurfRateMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 2] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand="makeTurfRateJsonFiles"
        print "--> [2 of 2] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"turfRateFile")])
    else:
        print "Turf Rate ROOT file for run ",runNum," up-to-date - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/turfhk/last"
        sys.stdout.flush()



        
    rawTime=getRawTimeModified(runNum,"house/turfhk/last")
    rootTime=getRootTimeModified(runNum,"sumTurfRateFile")
    if(rawTime>rootTime):
        print "Need new monitor ROOT file for run ",runNum," - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/turfhk/last"
        sys.stdout.flush()
        processCommand="runTelemSumTurfRateMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 2] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand="makeSumTurfRateJsonFiles"
        print "--> [2 of 2] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"sumTurfRateFile")])
    else:
        print "SummedTurfRate ROOT file for run ",runNum," up-to-date - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/turfhk/last"
        sys.stdout.flush()

    rawTime=getRawTimeModified(runNum,"house/surfhk/last")
    rootTime=getRootTimeModified(runNum,"surfHkFile")
    if(rawTime>rootTime):
        print "Need new surf hk ROOT file for run ",runNum," - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/surfhk/last"
        sys.stdout.flush()
        processCommand="runTelemSurfHkMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand="runTelemAvgSurfHkMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [2 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand="makeSurfHkJsonFiles"
        print "--> [3 of 4] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"surfHkFile")])
        processCommand="makeAvgSurfHkJsonFiles"
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
        processCommand="runTelemAuxMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 2] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand="makeAcqdStartRunJsonFiles"
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
        processCommand="runTelemGpsMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 12] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand="makeAdu5PatJsonFiles"
        print "--> [2 of 12] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"0"])
        processCommand="makeAdu5PatJsonFiles"
        print "--> [3 of 12] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"1"])
        processCommand="makeAdu5SatJsonFiles"
        print "--> [4 of 12] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"0"])
        processCommand="makeAdu5SatJsonFiles"
        print "--> [5 of 12] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"1"])
        processCommand="makeAdu5VtgJsonFiles"
        print "--> [6 of 12] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"0"])
        processCommand="makeAdu5VtgJsonFiles"
        print "--> [7 of 12] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"1"])      
        processCommand="makeG12PosJsonFiles"
        print "--> [8 of 12] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile")])
        processCommand="makeG12SatJsonFiles"
        print "--> [9 of 12] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile")])
        processCommand="makeGpsGgaJsonFiles"
        print "--> [10 of 12] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"0"])
        processCommand="makeGpsGgaJsonFiles"
        print "--> [11 of 12] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"1"])
        processCommand="makeGpsGgaJsonFiles"
        print "--> [12 of 12] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"2"])
    else:
        print "GPS ROOT file for run ",runNum," up-to-date - rawTime = ",rawTime," / rootTime = ",rootTime," <- from check on house/gps/last"
        sys.stdout.flush()

    rawTime=getRawTimeModified(runNum,"house/slowRate/last")
    rootTime=getRootTimeModified(runNum,"slowFile")
    if(rawTime>rootTime):
        print "Need new Slow ROOT file"
        processCommand="runTelemSlowMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        print "--> [1 of 2] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand],shell=True)
        processCommand="makeSlowRateHkJsonFiles"
        print "--> [2 of 3] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"slowFile")])
        processCommand="addRunToMapFile"
        print "--> [3 of 3] - running ",processCommand
        sys.stdout.flush()
        subprocess.call([processCommand,getRootFilename(runNum,"slowFile")])

    if(1):
        print "Reprocess config file"
        processCommand="processConfigTelem.sh"
        print "--> [1 of 1] - running ",processCommand
        subprocess.call([processCommand,str(runNum)])
        sys.stdout.flush()


if __name__ == "__main__":
    main()
