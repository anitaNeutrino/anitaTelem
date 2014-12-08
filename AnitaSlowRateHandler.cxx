////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered SlowRateFull_t      /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- December 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#include "AnitaSlowRateHandler.h"

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

AnitaSlowRateHandler::AnitaSlowRateHandler(std::string rawDir)
  :fRawDir(rawDir)
{


}

AnitaSlowRateHandler::~AnitaSlowRateHandler()
{


}
void AnitaSlowRateHandler::addSlowRate(SlowRateFull_t *slowPtr,int run,AnitaTelemFileType::AnitaTelemFileType_t fileType, int seqNum)
{

  std::map<UInt_t,std::map<UInt_t, SlowRateFull_t> >::iterator it=fSlowRateMap.find(run);
  if(it!=fSlowRateMap.end()) {
    it->second.insert(std::pair<UInt_t,SlowRateFull_t>(slowPtr->unixTime,*slowPtr));
  }
  else {
    std::map<UInt_t, SlowRateFull_t> runMap;
    runMap.insert(std::pair<UInt_t,SlowRateFull_t>(slowPtr->unixTime,*slowPtr));
    fSlowRateMap.insert(std::pair<UInt_t,std::map<UInt_t, SlowRateFull_t> >(run,runMap));
  }
}

void AnitaSlowRateHandler::loopMap() 
{
  char fileName[FILENAME_MAX];

  std::map<UInt_t,std::map<UInt_t, SlowRateFull_t> >::iterator runIt;
  for(runIt=fSlowRateMap.begin();runIt!=fSlowRateMap.end();runIt++) {
    int fRun=runIt->first;

  int fileCount=0;
  std::map<UInt_t,SlowRateFull_t>::iterator it;
  FILE *outFile=NULL;
  for(it=runIt->second.begin();it!=runIt->second.end();it++) {
    SlowRateFull_t *slowPtr=&(it->second);
    //    std::cout << slowPtr->unixTime << "\t" << slowPtr->unixTime << "\t" << 100*(slowPtr->unixTime/100) << "\n";    
    
    //    processSlowRate(slowPtr);
       
    if(outFile==NULL) {
      //      std::cout << "Here\n";
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"%s/run%d/house/slowRate/sub_%d/sub_%d/",fRawDir.c_str(),fRun,slowPtr->unixTime,slowPtr->unixTime);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/house/slowRate/last",fRawDir.c_str(),fRun);
      AwareRunDatabase::touchFile(fileName);
      sprintf(fileName,"%s/run%d/house/slowRate/sub_%d/sub_%d/slow_%d.dat.gz",fRawDir.c_str(),fRun,slowPtr->unixTime,slowPtr->unixTime,slowPtr->unixTime);
      std::cout << fileName << "\n";
      outFile=fopen(fileName,"ab");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    fwrite(slowPtr,sizeof(SlowRateFull_t),1,outFile);
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
