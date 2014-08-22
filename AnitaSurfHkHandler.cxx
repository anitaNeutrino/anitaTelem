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
#include "SurfHk.h"
#include "simpleStructs.h"


#define HACK_FOR_ROOT

#define C3PO_AVG 20

#define EVENTS_PER_FILE 100
#define EVENT_FILES_PER_DIR 100
#define HK_PER_FILE 1000

AnitaSurfHkHandler::AnitaSurfHkHandler(std::string rawDir,int run)
  :fRawDir(rawDir),fRun(run)
{


}

AnitaSurfHkHandler::~AnitaSurfHkHandler()
{


}
    
void AnitaSurfHkHandler::addSurfHk(FullSurfHkStruct_t *hkPtr)
{
  fSurfHkMap.insert(std::pair<UInt_t,FullSurfHkStruct_t>(hkPtr->unixTime,*hkPtr));

}

void AnitaSurfHkHandler::addAveragedSurfHk(AveragedSurfHkStruct_t *hkPtr)
{
  fAvgSurfHkMap.insert(std::pair<UInt_t,AveragedSurfHkStruct_t>(hkPtr->unixTime,*hkPtr));

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

      sprintf(fileName,"%s/run%d/house/surfhk/sub_%d/sub_%d/",fRawDir.c_str(),fRun,hkPtr->unixTime,hkPtr->unixTime);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/house/surfhk/sub_%d/sub_%d/surfhk_%d.dat.gz",fRawDir.c_str(),fRun,hkPtr->unixTime,hkPtr->unixTime,hkPtr->unixTime);
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


void AnitaSurfHkHandler::loopAvgMap() 
{
  char fileName[FILENAME_MAX];
  int fileCount=0;
  std::map<UInt_t,AveragedSurfHkStruct_t>::iterator it;
  FILE *outFile=NULL;
  for(it=fAvgSurfHkMap.begin();it!=fAvgSurfHkMap.end();it++) {
    AveragedSurfHkStruct_t *hkPtr=&(it->second);
    //    std::cout << hkPtr->unixTime << "\t" << hkPtr->unixTime << "\t" << 100*(hkPtr->unixTime/100) << "\n";    
    
    //    processSurfHk(hkPtr);
       
    if(outFile==NULL) {
      //      std::cout << "Here\n";
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"%s/run%d/house/surfhk/sub_%d/sub_%d/",fRawDir.c_str(),fRun,hkPtr->unixTime,hkPtr->unixTime);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/house/surfhk/sub_%d/sub_%d/avgsurfhk_%d.dat.gz",fRawDir.c_str(),fRun,hkPtr->unixTime,hkPtr->unixTime,hkPtr->unixTime);
      std::cout << fileName << "\n";
      outFile=fopen(fileName,"wb");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    fwrite(hkPtr,sizeof(AveragedSurfHkStruct_t),1,outFile);
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
