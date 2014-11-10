////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered AnitaEventHeader_t       /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#include "AnitaGenericHeaderHandler.h"

#include <iostream>
#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"
#include "AnitaPacketUtil.h"
#include "simpleStructs.h"


#define HACK_FOR_ROOT




AnitaGenericHeaderHandler::AnitaGenericHeaderHandler(std::string rawDir,int run)
  :fRawDir(rawDir),fRun(run)
{
 

}

AnitaGenericHeaderHandler::~AnitaGenericHeaderHandler()
{


}
    
void AnitaGenericHeaderHandler::addHeader(GenericHeader_t *hdPtr)
{
  fHeadMap.insert(std::pair<UInt_t,GenericHeader_t>(hdPtr->packetNumber,*hdPtr));

}


void AnitaGenericHeaderHandler::loopMap() 
{
  char fileName[FILENAME_MAX];
  int lastFileNumber=-1;
  std::map<UInt_t,AnitaEventHeader_t>::iterator it;
  FILE *outFile=NULL;
  for(it=fHeadMap.begin();it!=fHeadMap.end();it++) {
    AnitaEventHeader_t *hdPtr=&(it->second);
    //    std::cout << hdPtr->unixTime << "\t" << hdPtr->eventNumber << "\t" << 100*(hdPtr->eventNumber/100) << "\n";    
    int fileNumber=100*(hdPtr->eventNumber/100);
    //    processHeader(hdPtr);
    
    //Make base dir
    int dirNumber=(EVENTS_PER_FILE*EVENT_FILES_PER_DIR*EVENT_FILES_PER_DIR)*(hdPtr->eventNumber/(EVENTS_PER_FILE*EVENT_FILES_PER_DIR*EVENT_FILES_PER_DIR));    
    //Make sub dir
    int subDirNumber=(EVENTS_PER_FILE*EVENT_FILES_PER_DIR)*(hdPtr->eventNumber/(EVENTS_PER_FILE*EVENT_FILES_PER_DIR));

    if(fileNumber!=lastFileNumber) {
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"%s/run%d/event/ev%d/ev%d",fRawDir.c_str(),fRun,dirNumber,subDirNumber);
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/event/ev%d/ev%d/hd_%d.dat.gz",fRawDir.c_str(),fRun,dirNumber,subDirNumber,fileNumber);
      outFile=fopen(fileName,"wb");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    lastFileNumber=fileNumber;
    fwrite(hdPtr,sizeof(AnitaEventHeader_t),1,outFile);
  }
  
  if(outFile) fclose(outFile);
  outFile=NULL;

}
