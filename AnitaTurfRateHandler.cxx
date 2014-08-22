////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered TurfRateStruct_t       /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#include "AnitaTurfRateHandler.h"

#include <iostream>
#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"
#include "TurfRate.h"
#include "simpleStructs.h"


#define HACK_FOR_ROOT

#define C3PO_AVG 20

#define EVENTS_PER_FILE 100
#define EVENT_FILES_PER_DIR 100
#define HK_PER_FILE 1000

AnitaTurfRateHandler::AnitaTurfRateHandler(std::string rawDir,int run)
  :fRawDir(rawDir),fRun(run)
{


}

AnitaTurfRateHandler::~AnitaTurfRateHandler()
{


}
    
void AnitaTurfRateHandler::addTurfRate(TurfRateStruct_t *hkPtr)
{
  fTurfRateMap.insert(std::pair<UInt_t,TurfRateStruct_t>(hkPtr->unixTime,*hkPtr));

}

void AnitaTurfRateHandler::addSumTurfRate(SummedTurfRateStruct_t *hkPtr)
{
  fSumTurfRateMap.insert(std::pair<UInt_t,SummedTurfRateStruct_t>(hkPtr->unixTime,*hkPtr));

}


void AnitaTurfRateHandler::loopMap() 
{
  char fileName[FILENAME_MAX];
  int fileCount=0;
  std::map<UInt_t,TurfRateStruct_t>::iterator it;
  FILE *outFile=NULL;
  for(it=fTurfRateMap.begin();it!=fTurfRateMap.end();it++) {
    TurfRateStruct_t *hkPtr=&(it->second);
    //    std::cout << hkPtr->unixTime << "\t" << hkPtr->unixTime << "\t" << 100*(hkPtr->unixTime/100) << "\n";    
    
    //    processTurfRate(hkPtr);
       
    if(outFile==NULL) {
      //      std::cout << "Here\n";
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"%s/run%d/house/turfhk/sub_%d/sub_%d/",fRawDir.c_str(),fRun,hkPtr->unixTime,hkPtr->unixTime);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/house/turfhk/sub_%d/sub_%d/turfrate_%d.dat.gz",fRawDir.c_str(),fRun,hkPtr->unixTime,hkPtr->unixTime,hkPtr->unixTime);
      std::cout << fileName << "\n";
      outFile=fopen(fileName,"wb");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    fwrite(hkPtr,sizeof(TurfRateStruct_t),1,outFile);
    fileCount++;
    if(fileCount>=HK_PER_FILE) {
      fileCount=0;
      fclose(outFile);
      outFile=NULL;
    }
  }
  
  if(outFile) fclose(outFile);
  outFile=NULL;

}


void AnitaTurfRateHandler::loopSumMap() 
{
  char fileName[FILENAME_MAX];
  int fileCount=0;
  std::map<UInt_t,SummedTurfRateStruct_t>::iterator it;
  FILE *outFile=NULL;
  for(it=fSumTurfRateMap.begin();it!=fSumTurfRateMap.end();it++) {
    SummedTurfRateStruct_t *hkPtr=&(it->second);
    //    std::cout << hkPtr->unixTime << "\t" << hkPtr->unixTime << "\t" << 100*(hkPtr->unixTime/100) << "\n";    
    
    //    processTurfRate(hkPtr);
       
    if(outFile==NULL) {
      //      std::cout << "Here\n";
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"%s/run%d/house/turfhk/sub_%d/sub_%d/",fRawDir.c_str(),fRun,hkPtr->unixTime,hkPtr->unixTime);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/house/turfhk/sub_%d/sub_%d/sumturfrate_%d.dat.gz",fRawDir.c_str(),fRun,hkPtr->unixTime,hkPtr->unixTime,hkPtr->unixTime);
      std::cout << fileName << "\n";
      outFile=fopen(fileName,"wb");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    fwrite(hkPtr,sizeof(SummedTurfRateStruct_t),1,outFile);
    fileCount++;
    if(fileCount>=HK_PER_FILE) {
      fileCount=0;
      fclose(outFile);
      outFile=NULL;
    }
  }
  
  if(outFile) fclose(outFile);
  outFile=NULL;

}
