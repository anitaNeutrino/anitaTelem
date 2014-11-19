#!/usr/bin/env python
"""
This script should check if a telem run needs processing and if it does do the appropriate processing.
"""
import sys
import os
import subprocess

anitaTelemDir = os.getenv('ANITA_TELEM_DIR', "/home/radio/anita14/anitaTelem")
anitaTelemDataDir = os.getenv('ANITA_TELEM_DATA_DIR', "/anitaStorage/antarctica14/telem")
anitaTreeMakerDir = os.getenv("ANITA_TREE_MAKER_DIR","/home/radio/anita14/simpleTreeMaker")



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
    return os.path.getmtime(fullDir)

def getRootTimeModified(runNum,fileName):
    fullDir=anitaTelemDataDir+"/root/run"+str(runNum)+"/"+fileName+str(runNum)+".root"
    return os.path.getmtime(fullDir)

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

      
    rawTime=getRawTimeModified(runNum,"house/hk/last")
    rootTime=getRootTimeModified(runNum,"hkFile")
    if(rawTime>rootTime):
        print "Need new hk ROOT file"
        processCommand=anitaTreeMakerDir+"/runTelemHkMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)

    rawTime=getRawTimeModified(runNum,"house/monitor/last")
    rootTime=getRootTimeModified(runNum,"hkFile")
    if(rawTime>rootTime):
        print "Need new monitor ROOT file"
        processCommand=anitaTreeMakerDir+"/runTelemMonitorMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)
        processCommand=anitaTreeMakerDir+"/runTelemOtherMonitorMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)

    rawTime=getRawTimeModified(runNum,"house/turfhk/last")
    rootTime=getRootTimeModified(runNum,"turfRateFile")
    if(rawTime>rootTime):
        print "Need new monitor ROOT file"
        processCommand=anitaTreeMakerDir+"/runTelemTurfRateMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)
        processCommand=anitaTreeMakerDir+"/runTelemSumTurfRateMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)

    rawTime=getRawTimeModified(runNum,"house/surfhk/last")
    rootTime=getRootTimeModified(runNum,"surfHkFile")
    if(rawTime>rootTime):
        print "Need new surf hk ROOT file"
        processCommand=anitaTreeMakerDir+"/runTelemSurfHkMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)
        processCommand=anitaTreeMakerDir+"/runTelemAvgSurfHkMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)


    rawTime=getRawTimeModified(runNum,"start")
    rootTime=getRootTimeModified(runNum,"auxFile")
    if(rawTime>rootTime):
        print "Need new aux ROOT file"
        processCommand=anitaTreeMakerDir+"/runTelemAuxMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)

    rawTime=getRawTimeModified(runNum,"house/gps")
    rootTime=getRootTimeModified(runNum,"gpsFile")
    if(rawTime>rootTime):
        print "Need new GPS ROOT file"
        processCommand=anitaTreeMakerDir+"/runTelemGpsMaker.sh "+str(runNum)+" "+rawDir+" "+rootDir
        subprocess.call([processCommand],shell=True)

    





if __name__ == "__main__":
    main()
