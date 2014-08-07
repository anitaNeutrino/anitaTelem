////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered AnitaEventSurfHk_t       /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#include "AnitaSurfHkHandler.h"

#include <iostream>
#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"
#include "RawSurfHk.h"
#include "simpleStructs.h"


#define HACK_FOR_ROOT

#define C3PO_AVG 20

#define EVENTS_PER_FILE 100
#define EVENT_FILES_PER_DIR 100
#define HK_PER_FILE 1000

AnitaSurfHkHandler::AnitaSurfHkHandler(int run)
  :fRun(run)
{


}

AnitaSurfHkHandler::~AnitaSurfHkHandler()
{


}
    
void AnitaSurfHkHandler::addSurfHk(FullSurfHkStruct_t *hkPtr)
{
  fSurfHkMap.insert(std::pair<UInt_t,FullSurfHkStruct_t>(hkPtr->unixTime,*hkPtr));

}


void AnitaSurfHkHandler::loopMap() 
{
  char fileName[FILENAME_MAX];
  int fileCount=0;
  std::map<UInt_t,FullSurfHkStruct_t>::iterator it;
  FILE *outFile=NULL;
  for(it=fSurfHkMap.begin();it!=fSurfHkMap.end();it++) {
    FullSurfHkStruct_t *hkPtr=&(it->second);
    //    std::cout << hkPtr->unixTime << "\t" << hkPtr->unixTime << "\t" << 100*(hkPtr->unixTime/100) << "\n";    
    
    //    processSurfHk(hkPtr);
       
    if(outFile==NULL) {
      //      std::cout << "Here\n";
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"/anitaStorage/palestine14/telem/raw/run%d/house/hk/raw/sub_%d/sub_%d/",fRun,hkPtr->unixTime,hkPtr->unixTime);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"/anitaStorage/palestine14/telem/raw/run%d/house/hk/raw/sub_%d/sub_%d/hk_raw_%d.dat.gz",fRun,hkPtr->unixTime,hkPtr->unixTime,hkPtr->unixTime);
      std::cout << fileName << "\n";
      outFile=fopen(fileName,"wb");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    fwrite(hkPtr,sizeof(FullSurfHkStruct_t),1,outFile);
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
