////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered AnitaEventMonitor_t       /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#include "AnitaMonitorHandler.h"

#include <iostream>
#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"
#include "simpleStructs.h"
#include "AwareRunDatabase.h"


#define HACK_FOR_ROOT

#define C3PO_AVG 20

#define EVENTS_PER_FILE 100
#define EVENT_FILES_PER_DIR 100
#define MONITOR_PER_FILE 1000

AnitaMonitorHandler::AnitaMonitorHandler(std::string rawDir)
  :fRawDir(rawDir)
{


}

AnitaMonitorHandler::~AnitaMonitorHandler()
{


}
    
void AnitaMonitorHandler::addMonitor(MonitorStruct_t *monitorPtr,int run)
{
  unsigned int  unixTime=(monitorPtr->unixTime);
  std::map<UInt_t,std::map<UInt_t, MonitorStruct_t> >::iterator it=fMonitorMap.find(run);
  if(it!=fMonitorMap.end()) {
    it->second.insert(std::pair<UInt_t,MonitorStruct_t>(unixTime,*monitorPtr));
  }
  else {
    std::map<UInt_t, MonitorStruct_t> runMap;
    runMap.insert(std::pair<UInt_t,MonitorStruct_t>(unixTime,*monitorPtr));
    fMonitorMap.insert(std::pair<UInt_t,std::map<UInt_t, MonitorStruct_t> >(run,runMap));
  }
}

void AnitaMonitorHandler::addOtherMonitor(OtherMonitorStruct_t *monitorPtr,int run)
{
  unsigned int  unixTime=(monitorPtr->unixTime);
  std::map<UInt_t,std::map<UInt_t, OtherMonitorStruct_t> >::iterator it=fOtherMonitorMap.find(run);
  if(it!=fOtherMonitorMap.end()) {
    it->second.insert(std::pair<UInt_t,OtherMonitorStruct_t>(unixTime,*monitorPtr));
  }
  else {
    std::map<UInt_t, OtherMonitorStruct_t> runMap;
    runMap.insert(std::pair<UInt_t,OtherMonitorStruct_t>(unixTime,*monitorPtr));
    fOtherMonitorMap.insert(std::pair<UInt_t,std::map<UInt_t, OtherMonitorStruct_t> >(run,runMap));
  }

}


void AnitaMonitorHandler::loopMap() 
{
  char fileName[FILENAME_MAX];

  std::map<UInt_t,std::map<UInt_t, MonitorStruct_t> >::iterator runIt;
  for(runIt=fMonitorMap.begin();runIt!=fMonitorMap.end();runIt++) {
    int fRun=runIt->first;

  int fileCount=0;
  std::map<UInt_t,MonitorStruct_t>::iterator it;
  FILE *outFile=NULL;
  for(it=runIt->second.begin();it!=runIt->second.end();it++) {
    MonitorStruct_t *monitorPtr=&(it->second);
    //    std::cout << monitorPtr->unixTime << "\t" << monitorPtr->unixTime << "\t" << 100*(monitorPtr->unixTime/100) << "\n";    
    
    //    processMonitor(monitorPtr);
       
    if(outFile==NULL) {
      //      std::cout << "Here\n";
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"%s/run%d/house/monitor/sub_%d/sub_%d/",fRawDir.c_str(),fRun,monitorPtr->unixTime,monitorPtr->unixTime);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/house/monitor/last",fRawDir.c_str(),fRun);
      AwareRunDatabase::touchFile(fileName);
      sprintf(fileName,"%s/run%d/house/monitor/sub_%d/sub_%d/mon_%d.dat.gz",fRawDir.c_str(),fRun,monitorPtr->unixTime,monitorPtr->unixTime,monitorPtr->unixTime);
      std::cout << fileName << "\n";
      outFile=fopen(fileName,"ab");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    fwrite(monitorPtr,sizeof(MonitorStruct_t),1,outFile);
    fileCount++;
    if(fileCount>=MONITOR_PER_FILE) {
      fileCount=0;
      fclose(outFile);
      outFile=NULL;
    }
  }
  
  if(outFile) fclose(outFile);
  outFile=NULL;
  }

}


void AnitaMonitorHandler::loopOtherMap() 
{
  char fileName[FILENAME_MAX];

  std::map<UInt_t,std::map<UInt_t, OtherMonitorStruct_t> >::iterator runIt;
  for(runIt=fOtherMonitorMap.begin();runIt!=fOtherMonitorMap.end();runIt++) {
    int fRun=runIt->first;
  int fileCount=0;
  std::map<UInt_t,OtherMonitorStruct_t>::iterator it;
  FILE *outFile=NULL;
  for(it=runIt->second.begin();it!=runIt->second.end();it++) {
    OtherMonitorStruct_t *monitorPtr=&(it->second);
    //    std::cout << monitorPtr->unixTime << "\t" << monitorPtr->unixTime << "\t" << 100*(monitorPtr->unixTime/100) << "\n";    
    
    //    processOtherMonitor(monitorPtr);
       
    if(outFile==NULL) {
      //      std::cout << "Here\n";
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"%s/run%d/house/monitor/sub_%d/sub_%d/",fRawDir.c_str(),fRun,monitorPtr->unixTime,monitorPtr->unixTime);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/house/monitor/last",fRawDir.c_str(),fRun);
      AwareRunDatabase::touchFile(fileName);
      sprintf(fileName,"%s/run%d/house/monitor/sub_%d/sub_%d/othermon_%d.dat.gz",fRawDir.c_str(),fRun,monitorPtr->unixTime,monitorPtr->unixTime,monitorPtr->unixTime);
      std::cout << fileName << "\n";
      outFile=fopen(fileName,"ab");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    fwrite(monitorPtr,sizeof(OtherMonitorStruct_t),1,outFile);
    fileCount++;
    if(fileCount>=MONITOR_PER_FILE) {
      fileCount=0;
      fclose(outFile);
      outFile=NULL;
    }
  }
  
  if(outFile) fclose(outFile);
  outFile=NULL;
  }
}
