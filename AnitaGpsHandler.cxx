////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered AnitaEventHk_t       /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#include "AnitaGpsHandler.h"

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

AnitaGpsHandler::AnitaGpsHandler(std::string rawDir,int run)
  :fRawDir(rawDir),fRun(run)
{


}

AnitaGpsHandler::~AnitaGpsHandler()
{


}
    
void AnitaGpsHandler::addG12Pos(GpsG12PosStruct_t *gpsPtr)
{
  fG12PosMap.insert(std::pair<UInt_t,GpsG12PosStruct_t>(gpsPtr->unixTime,*gpsPtr));

}

void AnitaGpsHandler::addG12Sat(GpsG12SatStruct_t *gpsPtr)
{
  fG12SatMap.insert(std::pair<UInt_t,GpsG12SatStruct_t>(gpsPtr->unixTime,*gpsPtr));

}


void AnitaGpsHandler::loopG12PosMap() 
{
  char fileName[FILENAME_MAX];
  int fileCount=0;
  std::map<UInt_t,GpsG12PosStruct_t>::iterator it;
  FILE *outFile=NULL;
  for(it=fG12PosMap.begin();it!=fG12PosMap.end();it++) {
    GpsG12PosStruct_t *gpsPtr=&(it->second);
    //    std::cout << gpsPtr->unixTime << "\t" << gpsPtr->unixTime << "\t" << 100*(gpsPtr->unixTime/100) << "\n";    
    
    //    processHk(gpsPtr);
       
    if(outFile==NULL) {
      //      std::cout << "Here\n";
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"/anitaStorage/palestine14/telem/raw/run%d/house/gps/g12/pos/sub_%d/sub_%d/",fRun,gpsPtr->unixTime,gpsPtr->unixTime);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"/anitaStorage/palestine14/telem/raw/run%d/house/gps/g12/pos/sub_%d/sub_%d/pos_%d.dat.gz",fRun,gpsPtr->unixTime,gpsPtr->unixTime,gpsPtr->unixTime);
      std::cout << fileName << "\n";
      outFile=fopen(fileName,"wb");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    fwrite(gpsPtr,sizeof(GpsG12PosStruct_t),1,outFile);
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



void AnitaGpsHandler::loopG12SatMap() 
{
  char fileName[FILENAME_MAX];
  int fileCount=0;
  std::map<UInt_t,GpsG12SatStruct_t>::iterator it;
  FILE *outFile=NULL;
  for(it=fG12SatMap.begin();it!=fG12SatMap.end();it++) {
    GpsG12SatStruct_t *gpsPtr=&(it->second);
    //    std::cout << gpsPtr->unixTime << "\t" << gpsPtr->unixTime << "\t" << 100*(gpsPtr->unixTime/100) << "\n";    
    
    //    processHk(gpsPtr);
       
    if(outFile==NULL) {
      //      std::cout << "Here\n";
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"%s/run%d/house/gps/g12/sat/sub_%d/sub_%d/",fRawDir.c_str(),fRun,gpsPtr->unixTime,gpsPtr->unixTime);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/house/gps/g12/sat/sub_%d/sub_%d/sat_%d.dat.gz",fRawDir.c_str(),fRun,gpsPtr->unixTime,gpsPtr->unixTime,gpsPtr->unixTime);
      std::cout << fileName << "\n";
      outFile=fopen(fileName,"wb");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    fwrite(gpsPtr,sizeof(GpsG12SatStruct_t),1,outFile);
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
