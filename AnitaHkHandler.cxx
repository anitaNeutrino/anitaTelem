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
#include "AwareRunDatabase.h"


#define HACK_FOR_ROOT

#define C3PO_AVG 20

#define EVENTS_PER_FILE 100
#define EVENT_FILES_PER_DIR 100
#define HK_PER_FILE 1000

AnitaHkHandler::AnitaHkHandler(std::string rawDir)
  :fRawDir(rawDir)
{


}

AnitaHkHandler::~AnitaHkHandler()
{


}
    
void AnitaHkHandler::addHk(HkDataStruct_t *hkPtr, int run)
{
  std::map<UInt_t,std::map<UInt_t, HkDataStruct_t> >::iterator it=fHkMap.find(run);
  if(it!=fHkMap.end()) {
    it->second.insert(std::pair<UInt_t,HkDataStruct_t>(hkPtr->unixTime,*hkPtr));
  }
  else {
    std::map<UInt_t, HkDataStruct_t> runMap;
    runMap.insert(std::pair<UInt_t,HkDataStruct_t>(hkPtr->unixTime,*hkPtr));
    fHkMap.insert(std::pair<UInt_t,std::map<UInt_t, HkDataStruct_t> >(run,runMap));
  }
   
}

void AnitaHkHandler::addSSHk(SSHkDataStruct_t *hkPtr, int run)
{
  std::map<UInt_t,std::map<UInt_t, SSHkDataStruct_t> >::iterator it=fSSHkMap.find(run);
  if(it!=fSSHkMap.end()) {
    it->second.insert(std::pair<UInt_t,SSHkDataStruct_t>(hkPtr->unixTime,*hkPtr));
  }
  else {
    std::map<UInt_t, SSHkDataStruct_t> runMap;
    runMap.insert(std::pair<UInt_t,SSHkDataStruct_t>(hkPtr->unixTime,*hkPtr));
    fSSHkMap.insert(std::pair<UInt_t,std::map<UInt_t, SSHkDataStruct_t> >(run,runMap));
  }
   
}

void AnitaHkHandler::loopMap() 
{
  char fileName[FILENAME_MAX];
  
  std::map<UInt_t,std::map<UInt_t, HkDataStruct_t> >::iterator runIt;
  for(runIt=fHkMap.begin();runIt!=fHkMap.end();runIt++) {
    int fRun=runIt->first;

    int fileCount=0;
    std::map<UInt_t,HkDataStruct_t>::iterator it;
    FILE *outFile=NULL;    for(it=runIt->second.begin();it!=runIt->second.end();it++) {
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

	sprintf(fileName,"%s/run%d/house/hk/last",fRawDir.c_str(),fRun);
	AwareRunDatabase::touchFile(fileName);

	sprintf(fileName,"%s/run%d/house/hk/raw/sub_%d/sub_%d/hk_raw_%d.dat.gz",fRawDir.c_str(),fRun,hkPtr->unixTime,hkPtr->unixTime,hkPtr->unixTime);
	std::cout << fileName << "\n";
	outFile=fopen(fileName,"ab");
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

}



void AnitaHkHandler::loopSSMap() 
{
  char fileName[FILENAME_MAX];


  std::map<UInt_t,std::map<UInt_t, SSHkDataStruct_t> >::iterator runIt;
  for(runIt=fSSHkMap.begin();runIt!=fSSHkMap.end();runIt++) {
    int fRun=runIt->first;
    
    int fileCount=0;
    std::map<UInt_t,SSHkDataStruct_t>::iterator it;
    FILE *outFile=NULL;
    for(it=runIt->second.begin();it!=runIt->second.end();it++) {
      SSHkDataStruct_t *hkPtr=&(it->second);
      //    std::cout << hkPtr->unixTime << "\t" << hkPtr->unixTime << "\t" << 100*(hkPtr->unixTime/100) << "\n";    
      
      //    processHk(hkPtr);
      
      if(outFile==NULL) {
	//      std::cout << "Here\n";
	//Create a file
	if(outFile) fclose(outFile);
	outFile=NULL;

	sprintf(fileName,"%s/run%d/house/hk/raw/sub_%d/sub_%d/",fRawDir.c_str(),fRun,hkPtr->unixTime,hkPtr->unixTime);       
	gSystem->mkdir(fileName,kTRUE);

	sprintf(fileName,"%s/run%d/house/hk/last",fRawDir.c_str(),fRun);
	AwareRunDatabase::touchFile(fileName);

	sprintf(fileName,"%s/run%d/house/hk/raw/sub_%d/sub_%d/sshk_raw_%d.dat.gz",fRawDir.c_str(),fRun,hkPtr->unixTime,hkPtr->unixTime,hkPtr->unixTime);
	std::cout << fileName << "\n";
	outFile=fopen(fileName,"ab");
	if(!outFile ) {
	  printf("Couldn't open: %s\n",fileName);
	return;
	}
      }
      fwrite(hkPtr,sizeof(SSHkDataStruct_t),1,outFile);
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
}
