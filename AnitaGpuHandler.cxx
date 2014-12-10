////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered GpuPhiSectorPowerSpectrumStruct_t /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- December 2014                         /////////
////////////////////////////////////////////////////////////////////////////

#include "AnitaGpuHandler.h"

#include <iostream>
#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"
#include "simpleStructs.h"
#include "AwareRunDatabase.h"


#define HACK_FOR_ROOT

#define C3PO_AVG 20

#define EVENTS_PER_FILE 100
#define EVENT_FILES_PER_DIR 100
#define MONITOR_PER_FILE 1000

AnitaGpuHandler::AnitaGpuHandler(std::string rawDir)
  :fRawDir(rawDir)
{


}

AnitaGpuHandler::~AnitaGpuHandler()
{


}
    
void AnitaGpuHandler::addGpu(GpuPhiSectorPowerSpectrumStruct_t *gpuPtr,int run)
{

  std::map<UInt_t,std::map<UInt_t, GpuPhiSectorPowerSpectrumStruct_t> >::iterator it=fGpuMap.find(run);
  if(it!=fGpuMap.end()) {
    it->second.insert(std::pair<UInt_t,GpuPhiSectorPowerSpectrumStruct_t>(gpuPtr->unixTimeFirstEvent,*gpuPtr));
  }
  else {
    std::map<UInt_t, GpuPhiSectorPowerSpectrumStruct_t> runMap;
    runMap.insert(std::pair<UInt_t,GpuPhiSectorPowerSpectrumStruct_t>(gpuPtr->unixTimeFirstEvent,*gpuPtr));
    fGpuMap.insert(std::pair<UInt_t,std::map<UInt_t, GpuPhiSectorPowerSpectrumStruct_t> >(run,runMap));
  }
}



void AnitaGpuHandler::loopMap() 
{
  char fileName[FILENAME_MAX];

  std::map<UInt_t,std::map<UInt_t, GpuPhiSectorPowerSpectrumStruct_t> >::iterator runIt;
  for(runIt=fGpuMap.begin();runIt!=fGpuMap.end();runIt++) {
    int fRun=runIt->first;

  int fileCount=0;
  std::map<UInt_t,GpuPhiSectorPowerSpectrumStruct_t>::iterator it;
  FILE *outFile=NULL;
  for(it=runIt->second.begin();it!=runIt->second.end();it++) {
    GpuPhiSectorPowerSpectrumStruct_t *gpuPtr=&(it->second);
    //    std::cout << gpuPtr->unixTimeFirstEvent << "\t" << gpuPtr->unixTimeFirstEvent << "\t" << 100*(gpuPtr->unixTimeFirstEvent/100) << "\n";    
    
    //    processGpu(gpuPtr);
       
    if(outFile==NULL) {
      //      std::cout << "Here\n";
      //Create a file
      if(outFile) fclose(outFile);
      outFile=NULL;

      sprintf(fileName,"%s/run%d/house/gpu/sub_%d/sub_%d/",fRawDir.c_str(),fRun,gpuPtr->unixTimeFirstEvent,gpuPtr->unixTimeFirstEvent);       
      gSystem->mkdir(fileName,kTRUE);
      sprintf(fileName,"%s/run%d/house/gpu/last",fRawDir.c_str(),fRun);
      AwareRunDatabase::touchFile(fileName);
      sprintf(fileName,"%s/run%d/house/gpu/sub_%d/sub_%d/gpu_%d.dat.gz",fRawDir.c_str(),fRun,gpuPtr->unixTimeFirstEvent,gpuPtr->unixTimeFirstEvent,gpuPtr->unixTimeFirstEvent);
      std::cout << fileName << "\n";
      outFile=fopen(fileName,"ab");
      if(!outFile ) {
	printf("Couldn't open: %s\n",fileName);
	return;
      }
    }
    fwrite(gpuPtr,sizeof(GpuPhiSectorPowerSpectrumStruct_t),1,outFile);
    fileCount++;
    if(fileCount>=MONITOR_PER_FILE) {
      fileCount=0;
      fclose(outFile);
      outFile=NULL;
    }
  }
  
  if(outFile) fclose(outFile);
  outFile=NULL;
  }

}



