////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered AnitaEventHk_t       /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#include "AnitaHkHandler.h"

#include <iostream>
#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"
#include "RawHk.h"
#include "simpleStructs.h"


#define HACK_FOR_ROOT

#define C3PO_AVG 20

#define EVENTS_PER_FILE 100
#define EVENT_FILES_PER_DIR 100
#define HK_PER_FILE 1000

AnitaHkHandler::AnitaHkHandler(std::string rawDir,int run)
  :fRawDir(rawDir),fRun(run)
{


}

AnitaHkHandler::~AnitaHkHandler()
{


}
    
void AnitaHkHandler::addHk(HkDataStruct_t *hkPtr)
{
  fHkMap.insert(std::pair<UInt_t,HkDataStruct_t>(hkPtr->unixTime,*hkPtr));

}


void AnitaHkHandler::loopMap() 
{
  char fileName[FILENAME_MAX];
  int fileCount=0;
  std::map<UInt_t,HkDataStruct_t>::iterator it;
  FILE *outFile=NULL;
  for(it=fHkMap.begin();it!=fHkMap.end();it++) {
    HkDataStruct_t *hkPtr=&(it->second);
    //    std::cout << hkPtr->unixTime << "\t" << hkPtr->unixTime << "\t" << 100*(hkPtr->unixTime/100) << "\n";    
    
    //    processHk(hkPtr);
       
    if(outFile==NULL) {
      //      std::cout << "Here\n";
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"%s/run%d/house/hk/raw/sub_%d/sub_%d/",fRawDir.c_str(),fRun,hkPtr->unixTime,hkPtr->unixTime);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/house/hk/raw/sub_%d/sub_%d/hk_raw_%d.dat.gz",fRawDir.c_str(),fRun,hkPtr->unixTime,hkPtr->unixTime,hkPtr->unixTime);
      std::cout << fileName << "\n";
      outFile=fopen(fileName,"wb");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    fwrite(hkPtr,sizeof(HkDataStruct_t),1,outFile);
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
