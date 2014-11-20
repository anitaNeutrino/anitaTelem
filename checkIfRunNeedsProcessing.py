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
        sys.exit()

    runNum=sys.argv[1]
    
#Step one check raw dir exists
    if not doesRawDirExist(runNum):
        print "Raw dir does not exist for run ",runNum
        sys.exit()

#Step two check root dir exits
    if not doesRootDirExist(runNum):
        print "Root dir does not exist for run ",runNum
        processCommand=anitaTreeMakerDir+"/runAnitaIIIFileMakerTelem.sh "+str(runNum)
        subprocess.call([processCommand],shell=True)
        processCommand=anitaAwareFilemakerDir+"/antarctica/processRunTelem.sh "+str(runNum)
        subprocess.call([processCommand],shell=True)
        sys.exit()

#Step three check if individual root files need to be remade

    rawDir=anitaTelemDataDir+"/raw/run"+str(runNum)   
    rootDir=anitaTelemDataDir+"/root/run"+str(runNum)
     
    rawTime=getRawTimeModified(runNum,"event/last")
    rootTime=getRootTimeModified(runNum,"eventFile")
    if(rawTime>rootTime):
        print "Need new event ROOT file"
        processCommand=anitaTreeMakerDir+"/runTelemEventMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)
        processCommand=anitaTreeMakerDir+"/runTelemHeaderMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)
        #Here insert call to aware file maker    
        processCommand=anitaAwareFilemakerDir+"/makeHeaderJsonFiles"
        print processCommand
        subprocess.call([processCommand,getRootFilename(runNum,"headFile")])

      
    rawTime=getRawTimeModified(runNum,"house/hk/last")
    rootTime=getRootTimeModified(runNum,"hkFile")
    if(rawTime>rootTime):
        print "Need new hk ROOT file"
        processCommand=anitaTreeMakerDir+"/runTelemHkMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)
        #Here insert call to aware file maker
        processCommand=anitaAwareFilemakerDir+"/makePrettyHkJsonFiles"
        subprocess.call([processCommand,getRootFilename(runNum,"hkFile")])

    rawTime=getRawTimeModified(runNum,"house/monitor/last")
    rootTime=getRootTimeModified(runNum,"hkFile")
    if(rawTime>rootTime):
        print "Need new monitor ROOT file"
        processCommand=anitaTreeMakerDir+"/runTelemMonitorMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)
        processCommand=anitaTreeMakerDir+"/runTelemOtherMonitorMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)
        #Here insert call to aware file maker
        processCommand=anitaAwareFilemakerDir+"/makeMonitorHkJsonFiles"
        subprocess.call([processCommand,getRootFilename(runNum,"monitorFile")])
        processCommand=anitaAwareFilemakerDir+"/makeOtherMonitorHkJsonFiles"
        subprocess.call([processCommand,getRootFilename(runNum,"monitorFile")])

    rawTime=getRawTimeModified(runNum,"house/turfhk/last")
    rootTime=getRootTimeModified(runNum,"turfRateFile")
    if(rawTime>rootTime):
        print "Need new monitor ROOT file"
        processCommand=anitaTreeMakerDir+"/runTelemTurfRateMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)
        processCommand=anitaTreeMakerDir+"/runTelemSumTurfRateMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)
        processCommand=anitaAwareFilemakerDir+"/makeTurfRateJsonFiles"
        subprocess.call([processCommand,getRootFilename(runNum,"turfRateFile")])
        processCommand=anitaAwareFilemakerDir+"/makeSumTurfRateJsonFiles"
        print processCommand
        subprocess.call([processCommand,getRootFilename(runNum,"sumTurfRateFile")])

    rawTime=getRawTimeModified(runNum,"house/surfhk/last")
    rootTime=getRootTimeModified(runNum,"surfHkFile")
    if(rawTime>rootTime):
        print "Need new surf hk ROOT file"
        processCommand=anitaTreeMakerDir+"/runTelemSurfHkMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)
        processCommand=anitaTreeMakerDir+"/runTelemAvgSurfHkMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)
        processCommand=anitaAwareFilemakerDir+"/makeSurfHkJsonFiles"
        subprocess.call([processCommand,getRootFilename(runNum,"surfHkFile")])
        processCommand=anitaAwareFilemakerDir+"/makeAvgSurfHkJsonFiles"
        subprocess.call([processCommand,getRootFilename(runNum,"avgSurfHkFile")])


    rawTime=getRawTimeModified(runNum,"start/last")
    rootTime=getRootTimeModified(runNum,"auxFile")
    if(rawTime>rootTime):
        print "Need new aux ROOT file"
        processCommand=anitaTreeMakerDir+"/runTelemAuxMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)
        processCommand=anitaAwareFilemakerDir+"/makeAcqdStartRunJsonFiles"
        subprocess.call([processCommand,getRootFilename(runNum,"auxFile")])


    rawTime=getRawTimeModified(runNum,"house/gps/last")
    rootTime=getRootTimeModified(runNum,"gpsFile")
    if(rawTime>rootTime):
        print "Need new GPS ROOT file"
        processCommand=anitaTreeMakerDir+"/runTelemGpsMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)
        processCommand=anitaAwareFilemakerDir+"/makeAdu5PatJsonFiles"
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"0"])
        processCommand=anitaAwareFilemakerDir+"/makeAdu5PatJsonFiles"
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"1"])
        processCommand=anitaAwareFilemakerDir+"/makeAdu5SatJsonFiles"
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"0"])
        processCommand=anitaAwareFilemakerDir+"/makeAdu5SatJsonFiles"
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"1"])
        processCommand=anitaAwareFilemakerDir+"/makeAdu5VtgJsonFiles"
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"0"])
        processCommand=anitaAwareFilemakerDir+"/makeAdu5VtgJsonFiles"
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile"),"1"])
      
        processCommand=anitaAwareFilemakerDir+"/makeG12PosJsonFiles"
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile")])
        processCommand=anitaAwareFilemakerDir+"/makeG12SatJsonFiles"
        subprocess.call([processCommand,getRootFilename(runNum,"gpsFile")])

    





if __name__ == "__main__":
    main()
