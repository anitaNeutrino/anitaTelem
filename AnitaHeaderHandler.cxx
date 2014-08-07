////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered AnitaEventHeader_t       /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#include "AnitaHeaderHandler.h"

#include <iostream>
#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"
#include "RawAnitaHeader.h"
#include "simpleStructs.h"


#define HACK_FOR_ROOT

#define C3PO_AVG 20



AnitaHeaderHandler::AnitaHeaderHandler()
{


}

AnitaHeaderHandler::~AnitaHeaderHandler()
{


}
    
void AnitaHeaderHandler::addHeader(AnitaEventHeader_t *hdPtr)
{
  fHeadMap.insert(std::pair<UInt_t,AnitaEventHeader_t>(hdPtr->eventNumber,*hdPtr));

}


void AnitaHeaderHandler::loopMap() 
{
  char fileName[FILENAME_MAX];
  int lastFileNumber=-1;
  std::map<UInt_t,AnitaEventHeader_t>::iterator it;
  FILE *outFile;
  for(it=fHeadMap.begin();it!=fHeadMap.end();it++) {
    AnitaEventHeader_t *hdPtr=&(it->second);
    std::cout << hdPtr->unixTime << "\t" << hdPtr->eventNumber << "\t" << 100*(hdPtr->eventNumber/100) << "\n";    
    int fileNumber=100*(hdPtr->eventNumber/100);
    //    processHeader(hdPtr);
    
    if(fileNumber!=lastFileNumber) {
      //Create a file
      sprintf(fileName,"/anitaStorage/palestine14/telem/raw/run1000/hd_%d.dat",fileNumber);
      outFile=fopen(fileName,"wb");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return -1;
      }
    }
    fwrite(


  }
  


}
