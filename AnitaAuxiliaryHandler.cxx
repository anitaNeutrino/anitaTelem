////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered AnitaEventHk_t       /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#include "AnitaAuxiliaryHandler.h"

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

AnitaAuxiliaryHandler::AnitaAuxiliaryHandler(std::string rawDir,int run)
  :fRawDir(rawDir),fRun(run)
{


}

AnitaAuxiliaryHandler::~AnitaAuxiliaryHandler()
{


}
    
void AnitaAuxiliaryHandler::addRunStart(RunStart_t *hkPtr)
{
  fRunStartMap.insert(std::pair<UInt_t,RunStart_t>(hkPtr->unixTime,*hkPtr));

}
    
void AnitaAuxiliaryHandler::addAcqdStart(AcqdStartStruct_t *hkPtr)
{
  fAcqdStartMap.insert(std::pair<UInt_t,AcqdStartStruct_t>(hkPtr->unixTime,*hkPtr));

}

void AnitaAuxiliaryHandler::addGpsdStart(GpsdStartStruct_t *hkPtr)
{
  fGpsdStartMap.insert(std::pair<UInt_t,GpsdStartStruct_t>(hkPtr->unixTime,*hkPtr));

}

void AnitaAuxiliaryHandler::addLogWatchdStart(LogWatchdStart_t *hkPtr)
{
  fLogWatchdStartMap.insert(std::pair<UInt_t,LogWatchdStart_t>(hkPtr->unixTime,*hkPtr));

}
   


void AnitaAuxiliaryHandler::loopAcqdStartMap() 
{
  char fileName[FILENAME_MAX];
  int fileCount=0;
  std::map<UInt_t,AcqdStartStruct_t>::iterator it;
  FILE *outFile=NULL;
  for(it=fAcqdStartMap.begin();it!=fAcqdStartMap.end();it++) {
    AcqdStartStruct_t *hkPtr=&(it->second);

    if(outFile==NULL) {
      //      std::cout << "Here\n";
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"%s/run%d/start",fRawDir.c_str(),fRun);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/start/acqd_%d.dat",fRawDir.c_str(),fRun,hkPtr->unixTime);
      std::cout << fileName << "\n";
      outFile=fopen(fileName,"wb");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    fwrite(hkPtr,sizeof(AcqdStartStruct_t),1,outFile);
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


void AnitaAuxiliaryHandler::loopGpsdStartMap() 
{
  char fileName[FILENAME_MAX];
  int fileCount=0;
  std::map<UInt_t,GpsdStartStruct_t>::iterator it;
  FILE *outFile=NULL;
  for(it=fGpsdStartMap.begin();it!=fGpsdStartMap.end();it++) {
    GpsdStartStruct_t *hkPtr=&(it->second);

    if(outFile==NULL) {
      //      std::cout << "Here\n";
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"%s/run%d/start",fRawDir.c_str(),fRun);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/start/gpsd_%d.dat",fRawDir.c_str(),fRun,hkPtr->unixTime);
      std::cout << fileName << "\n";
      outFile=fopen(fileName,"wb");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    fwrite(hkPtr,sizeof(GpsdStartStruct_t),1,outFile);
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


void AnitaAuxiliaryHandler::loopLogWatchdStartMap() 
{
  char fileName[FILENAME_MAX];
  int fileCount=0;
  std::map<UInt_t,LogWatchdStart_t>::iterator it;
  FILE *outFile=NULL;
  for(it=fLogWatchdStartMap.begin();it!=fLogWatchdStartMap.end();it++) {
    LogWatchdStart_t *hkPtr=&(it->second);

    if(outFile==NULL) {
      //      std::cout << "Here\n";
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"%s/run%d/start",fRawDir.c_str(),fRun);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/start/logwatchd_%d.dat",fRawDir.c_str(),fRun,hkPtr->unixTime);
      std::cout << fileName << "\n";
      outFile=fopen(fileName,"wb");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    fwrite(hkPtr,sizeof(LogWatchdStart_t),1,outFile);
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


