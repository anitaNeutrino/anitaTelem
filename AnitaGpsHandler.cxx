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


void AnitaGpsHandler::addGpsGga(GpsGgaStruct_t *gpsPtr)
{
  fGpsGgaMap[whichGps(gpsPtr->gHdr.code)].insert(std::pair<UInt_t,GpsGgaStruct_t>(gpsPtr->unixTime,*gpsPtr));
}


void AnitaGpsHandler::addAdu5Pat(GpsAdu5PatStruct_t *gpsPtr)
{
  fAdu5PatMap[whichGps(gpsPtr->gHdr.code)].insert(std::pair<UInt_t,GpsAdu5PatStruct_t>(gpsPtr->unixTime,*gpsPtr));
}


void AnitaGpsHandler::addAdu5Sat(GpsAdu5SatStruct_t *gpsPtr)
{
  fAdu5SatMap[whichGps(gpsPtr->gHdr.code)].insert(std::pair<UInt_t,GpsAdu5SatStruct_t>(gpsPtr->unixTime,*gpsPtr));
}


void AnitaGpsHandler::addAdu5Vtg(GpsAdu5VtgStruct_t *gpsPtr)
{
  fAdu5VtgMap[whichGps(gpsPtr->gHdr.code)].insert(std::pair<UInt_t,GpsAdu5VtgStruct_t>(gpsPtr->unixTime,*gpsPtr));
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

      sprintf(fileName,"%s/run%d/house/gps/g12/pos/sub_%d/sub_%d/",fRawDir.c_str(),fRun,gpsPtr->unixTime,gpsPtr->unixTime);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/house/gps/g12/pos/sub_%d/sub_%d/pos_%d.dat.gz",fRawDir.c_str(),fRun,gpsPtr->unixTime,gpsPtr->unixTime,gpsPtr->unixTime);
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



void AnitaGpsHandler::loopGpsGgaMaps() 
{
  char fileName[FILENAME_MAX];

  for(int gpsId=0;gpsId<3;gpsId++) {
    
    int fileCount=0;
    std::map<UInt_t,GpsGgaStruct_t>::iterator it;
    FILE *outFile=NULL;
    for(it=fGpsGgaMap[gpsId].begin();it!=fGpsGgaMap[gpsId].end();it++) {
      GpsGgaStruct_t *gpsPtr=&(it->second);
      //    std::cout << gpsPtr->unixTime << "\t" << gpsPtr->unixTime << "\t" << 100*(gpsPtr->unixTime/100) << "\n";    
      
      //    processHk(gpsPtr);
      
      if(outFile==NULL) {
	//      std::cout << "Here\n";
	//Create a file
	if(outFile) fclose(outFile);
	outFile=NULL;
	
	sprintf(fileName,"%s/run%d/house/gps/%s/gga/sub_%d/sub_%d/",fRawDir.c_str(),fRun,getGpsName(gpsId),gpsPtr->unixTime,gpsPtr->unixTime);       
	gSystem->mkdir(fileName,kTRUE);
	sprintf(fileName,"%s/run%d/house/gps/%s/gga/sub_%d/sub_%d/gga_%d.dat.gz",fRawDir.c_str(),fRun,getGpsName(gpsId),gpsPtr->unixTime,gpsPtr->unixTime,gpsPtr->unixTime);
	std::cout << fileName << "\n";
	outFile=fopen(fileName,"wb");
	if(!outFile ) {
	  printf("Couldn't open: %s\n",fileName);
	  return;
      }
      }
      fwrite(gpsPtr,sizeof(GpsGgaStruct_t),1,outFile);
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



void AnitaGpsHandler::loopAdu5PatMaps() 
{
  char fileName[FILENAME_MAX];

  for(int gpsId=0;gpsId<2;gpsId++) {
    
    int fileCount=0;
    std::map<UInt_t,GpsAdu5PatStruct_t>::iterator it;
    FILE *outFile=NULL;
    for(it=fAdu5PatMap[gpsId].begin();it!=fAdu5PatMap[gpsId].end();it++) {
      GpsAdu5PatStruct_t *gpsPtr=&(it->second);
      //    std::cout << gpsPtr->unixTime << "\t" << gpsPtr->unixTime << "\t" << 100*(gpsPtr->unixTime/100) << "\n";    
      
      //    processHk(gpsPtr);
      
      if(outFile==NULL) {
	//      std::cout << "Here\n";
	//Create a file
	if(outFile) fclose(outFile);
	outFile=NULL;
	
	sprintf(fileName,"%s/run%d/house/gps/%s/pat/sub_%d/sub_%d/",fRawDir.c_str(),fRun,getGpsName(gpsId),gpsPtr->unixTime,gpsPtr->unixTime);       
	gSystem->mkdir(fileName,kTRUE);
	sprintf(fileName,"%s/run%d/house/gps/%s/pat/sub_%d/sub_%d/pat_%d.dat.gz",fRawDir.c_str(),fRun,getGpsName(gpsId),gpsPtr->unixTime,gpsPtr->unixTime,gpsPtr->unixTime);
	std::cout << fileName << "\n";
	outFile=fopen(fileName,"wb");
	if(!outFile ) {
	  printf("Couldn't open: %s\n",fileName);
	  return;
      }
      }
      fwrite(gpsPtr,sizeof(GpsAdu5PatStruct_t),1,outFile);
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


void AnitaGpsHandler::loopAdu5SatMaps() 
{
  char fileName[FILENAME_MAX];

  for(int gpsId=0;gpsId<2;gpsId++) {
    
    int fileCount=0;
    std::map<UInt_t,GpsAdu5SatStruct_t>::iterator it;
    FILE *outFile=NULL;
    for(it=fAdu5SatMap[gpsId].begin();it!=fAdu5SatMap[gpsId].end();it++) {
      GpsAdu5SatStruct_t *gpsPtr=&(it->second);
      //    std::cout << gpsPtr->unixTime << "\t" << gpsPtr->unixTime << "\t" << 100*(gpsPtr->unixTime/100) << "\n";    
      
      //    processHk(gpsPtr);
      
      if(outFile==NULL) {
	//      std::cout << "Here\n";
	//Create a file
	if(outFile) fclose(outFile);
	outFile=NULL;
	
	sprintf(fileName,"%s/run%d/house/gps/%s/sat/sub_%d/sub_%d/",fRawDir.c_str(),fRun,getGpsName(gpsId),gpsPtr->unixTime,gpsPtr->unixTime);       
	gSystem->mkdir(fileName,kTRUE);
	sprintf(fileName,"%s/run%d/house/gps/%s/sat/sub_%d/sub_%d/sat_%d.dat.gz",fRawDir.c_str(),fRun,getGpsName(gpsId),gpsPtr->unixTime,gpsPtr->unixTime,gpsPtr->unixTime);
	std::cout << fileName << "\n";
	outFile=fopen(fileName,"wb");
	if(!outFile ) {
	  printf("Couldn't open: %s\n",fileName);
	  return;
      }
      }
      fwrite(gpsPtr,sizeof(GpsAdu5SatStruct_t),1,outFile);
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


void AnitaGpsHandler::loopAdu5VtgMaps() 
{
  char fileName[FILENAME_MAX];

  for(int gpsId=0;gpsId<2;gpsId++) {
    
    int fileCount=0;
    std::map<UInt_t,GpsAdu5VtgStruct_t>::iterator it;
    FILE *outFile=NULL;
    for(it=fAdu5VtgMap[gpsId].begin();it!=fAdu5VtgMap[gpsId].end();it++) {
      GpsAdu5VtgStruct_t *gpsPtr=&(it->second);
      //    std::cout << gpsPtr->unixTime << "\t" << gpsPtr->unixTime << "\t" << 100*(gpsPtr->unixTime/100) << "\n";    
      
      //    processHk(gpsPtr);
      
      if(outFile==NULL) {
	//      std::cout << "Here\n";
	//Create a file
	if(outFile) fclose(outFile);
	outFile=NULL;
	
	sprintf(fileName,"%s/run%d/house/gps/%s/vtg/sub_%d/sub_%d/",fRawDir.c_str(),fRun,getGpsName(gpsId),gpsPtr->unixTime,gpsPtr->unixTime);       
	gSystem->mkdir(fileName,kTRUE);
	sprintf(fileName,"%s/run%d/house/gps/%s/vtg/sub_%d/sub_%d/vtg_%d.dat.gz",fRawDir.c_str(),fRun,getGpsName(gpsId),gpsPtr->unixTime,gpsPtr->unixTime,gpsPtr->unixTime);
	std::cout << fileName << "\n";
	outFile=fopen(fileName,"wb");
	if(!outFile ) {
	  printf("Couldn't open: %s\n",fileName);
	  return;
      }
      }
      fwrite(gpsPtr,sizeof(GpsAdu5VtgStruct_t),1,outFile);
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



int AnitaGpsHandler::whichGps(PacketCode_t code) {
  ///< 0 is ADU5A, 1 is ADU5B, 2 is G12
  if(code&PACKET_FROM_ADU5B) {
    return 1;
  }      
  else if(code&PACKET_FROM_G12) {
    return 2;
  }
  else {
    return 0;
  }
}

const char *AnitaGpsHandler::getGpsName(int gpsId) {
  switch(gpsId) {
  case 0: return "adu5a";
  case 1: return "adu5b";
  case 2: return "g12";
  default: return "unknown";
  } 
}
