/// tuff telemetry handler 
#include "AnitaTuffHandler.h"
#include "AwareRunDatabase.h"
#include "TSystem.h"
#include <iostream>


#define EVENTS_PER_FILE 100
#define EVENT_FILES_PER_DIR 100
#define MONITOR_PER_FILE 1000



AnitaTuffHandler::AnitaTuffHandler(std::string rawDir)
  : fRawDir(rawDir) 
{ } 


AnitaTuffHandler::~AnitaTuffHandler()
{ }


void AnitaTuffHandler::addTuffStatus(TuffNotchStatus_t * tuffPtr, int run)
{

  std::map<UInt_t,std::map<UInt_t,TuffNotchStatus_t> >::iterator it = fNotchMap.find(run); 

  if (it!=fNotchMap.end())
  {
    unsigned time = tuffPtr->unixTime;
    it->second.insert(std::pair<UInt_t, TuffNotchStatus_t> ( time , *tuffPtr)); 
  }

  else
  {
    std::map<UInt_t, TuffNotchStatus_t> runMap;
    unsigned time = tuffPtr->unixTime; 
    runMap.insert(std::pair<UInt_t,TuffNotchStatus_t>(time,*tuffPtr));
    fNotchMap.insert(std::pair<UInt_t,std::map<UInt_t, TuffNotchStatus_t> >(run,runMap));
  }
}


void AnitaTuffHandler::addRawCommand(TuffRawCmd_t * tuffPtr, int run)
{
  std::map<UInt_t,std::map<UInt_t,TuffRawCmd_t> >::iterator it = fRawMap.find(run); 

  if (it!=fRawMap.end())
  {
    unsigned time = tuffPtr->enactedTime; 
    it->second.insert(std::pair<UInt_t, TuffRawCmd_t> ( time, *tuffPtr)); 
  }

  else
  {
    std::map<UInt_t, TuffRawCmd_t> runMap;
    unsigned time = tuffPtr->enactedTime;
    runMap.insert(std::pair<UInt_t,TuffRawCmd_t>(time,*tuffPtr));
    fRawMap.insert(std::pair<UInt_t,std::map<UInt_t, TuffRawCmd_t> >(run,runMap));
  }
}


void AnitaTuffHandler::loopNotchMap() 
{
  char fileName[FILENAME_MAX];

  std::map<UInt_t,std::map<UInt_t, TuffNotchStatus_t> >::iterator runIt;
  for(runIt=fNotchMap.begin();runIt!=fNotchMap.end();runIt++) {
    int fRun=runIt->first;

  int fileCount=0;
  std::map<UInt_t,TuffNotchStatus_t>::iterator it;
  FILE *outFile=NULL;
  for(it=runIt->second.begin();it!=runIt->second.end();it++) {
    TuffNotchStatus_t *notchPtr=&(it->second);
    
    //    processNotch(notchPtr);
       
    if(outFile==NULL) {
      //      std::cout << "Here\n";
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"%s/run%d/house/tuff/sub_%d/sub_%d/",fRawDir.c_str(),fRun,notchPtr->unixTime,notchPtr->unixTime);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/house/tuff/last",fRawDir.c_str(),fRun);
      AwareRunDatabase::touchFile(fileName);
      sprintf(fileName,"%s/run%d/house/tuff/sub_%d/sub_%d/tuff%d.dat.gz",fRawDir.c_str(),fRun,notchPtr->unixTime,notchPtr->unixTime,notchPtr->unixTime);
      std::cout << fileName << "\n";
      outFile=fopen(fileName,"ab");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    fwrite(notchPtr,sizeof(TuffNotchStatus_t),1,outFile);
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
 
void AnitaTuffHandler::loopRawCmdMap() 
{
  char fileName[FILENAME_MAX];

  std::map<UInt_t,std::map<UInt_t, TuffRawCmd_t> >::iterator runIt;
  for(runIt=fRawMap.begin();runIt!=fRawMap.end();runIt++) {
    int fRun=runIt->first;

  int fileCount=0;
  std::map<UInt_t,TuffRawCmd_t>::iterator it;
  FILE *outFile=NULL;
  for(it=runIt->second.begin();it!=runIt->second.end();it++) {
    TuffRawCmd_t *rawPtr=&(it->second);
    
    //    processRaw(rawPtr);
       
    if(outFile==NULL) {
      //      std::cout << "Here\n";
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"%s/run%d/house/tuff/sub_%d/sub_%d/",fRawDir.c_str(),fRun,rawPtr->enactedTime,rawPtr->enactedTime);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/house/tuff/last",fRawDir.c_str(),fRun);
      AwareRunDatabase::touchFile(fileName);
      sprintf(fileName,"%s/run%d/house/tuff/sub_%d/sub_%d/rawcmd_%d.dat.gz",fRawDir.c_str(),fRun,rawPtr->enactedTime,rawPtr->enactedTime,rawPtr->enactedTime);
      std::cout << fileName << "\n";
      outFile=fopen(fileName,"ab");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    fwrite(rawPtr,sizeof(TuffRawCmd_t),1,outFile);
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
