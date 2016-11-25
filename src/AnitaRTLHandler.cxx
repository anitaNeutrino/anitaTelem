#include "AnitaRTLHandler.h"

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

AnitaRTLHandler::AnitaRTLHandler(std::string rawDir)
  :fRawDir(rawDir)
{


}

AnitaRTLHandler::~AnitaRTLHandler()
{


}
    
void AnitaRTLHandler::addRTL(RtlSdrPowerSpectraStruct_t *rtlPtr,int run)
{

  std::map<UInt_t,std::map<UInt_t, RtlSdrPowerSpectraStruct_t> >::iterator it=fRTLMap.find(run);
  if(it!=fRTLMap.end()) {
    it->second.insert(std::pair<UInt_t,RtlSdrPowerSpectraStruct_t>(rtlPtr->scanTime,*rtlPtr));
  }
  else {
    std::map<UInt_t, RtlSdrPowerSpectraStruct_t> runMap;
    runMap.insert(std::pair<UInt_t,RtlSdrPowerSpectraStruct_t>(rtlPtr->scanTime,*rtlPtr));
    fRTLMap.insert(std::pair<UInt_t,std::map<UInt_t, RtlSdrPowerSpectraStruct_t> >(run,runMap));
  }
}



void AnitaRTLHandler::loopMap() 
{
  char fileName[FILENAME_MAX];

  std::map<UInt_t,std::map<UInt_t, RtlSdrPowerSpectraStruct_t> >::iterator runIt;
  for(runIt=fRTLMap.begin();runIt!=fRTLMap.end();runIt++) {
    int fRun=runIt->first;

  int fileCount=0;
  std::map<UInt_t,RtlSdrPowerSpectraStruct_t>::iterator it;
  FILE *outFile=NULL;
  for(it=runIt->second.begin();it!=runIt->second.end();it++) {
    RtlSdrPowerSpectraStruct_t *rtlPtr=&(it->second);
    
       
    if(outFile==NULL) {
      //      std::cout << "Here\n";
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"%s/run%d/house/rtl/sub_%d/sub_%d/",fRawDir.c_str(),fRun,rtlPtr->scanTime,rtlPtr->scanTime);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/house/rtl/last",fRawDir.c_str(),fRun);
      AwareRunDatabase::touchFile(fileName);
      sprintf(fileName,"%s/run%d/house/rtl/sub_%d/sub_%d/rtl_%d.dat.gz",fRawDir.c_str(),fRun,rtlPtr->scanTime,rtlPtr->scanTime,rtlPtr->scanTime);
      std::cout << fileName << "\n";
      outFile=fopen(fileName,"ab");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    fwrite(rtlPtr,sizeof(RtlSdrPowerSpectraStruct_t),1,outFile);
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



