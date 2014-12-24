////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered AnitaEventHeader_t       /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#include "AnitaHeaderHandler.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <utime.h>
#include <sys/stat.h>
#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TPad.h"
#include "AnitaPacketUtil.h"
#include "simpleStructs.h"
#include "RawAnitaEvent.h"
#include "RawAnitaHeader.h"
#include "UsefulAnitaEvent.h"
#include "AnitaCanvasMaker.h"
#include "AwareRunDatabase.h"


#define HACK_FOR_ROOT

#define C3PO_AVG 20

#define EVENTS_PER_DIR 1000
#define EVENTS_PER_FILE 100
#define EVENT_FILES_PER_DIR 100

const char *telemTypeForFile[5]={"Los","Tdrss","Openport","SlowTdrss","Iridium"};

AnitaHeaderHandler::AnitaHeaderHandler(std::string rawDir,std::string awareDir,int makeEventPngsForAware)
  :fRawDir(rawDir),fAwareDir(awareDir),fMakeEventDisplaysForAware(makeEventPngsForAware),startedEvent(0)
{
  zeroCounters();
  fHeaderTouchFile=fAwareDir+"/ANITA3/lastHeader";
  fEventTouchFile=fAwareDir+"/ANITA3/lastEvent";
  fReadLastPartialEvent=0;

  

}

void AnitaHeaderHandler::newFile(AnitaTelemFileType::AnitaTelemFileType_t fileType) {
  if(!fReadLastPartialEvent) {
    char fileName[FILENAME_MAX];
    sprintf(fileName,"%s/ANITA3/db/currentEvent%s.dat",fAwareDir.c_str(),telemTypeForFile[fileType]);
    
    FILE *fp = fopen(fileName,"rb");
    if(fp) {
      std::cout << "Reading from : " << fileName << "\n";
      fread(&curPSBody,sizeof(PedSubbedEventBody_t),1,fp);
      fread(&startedEvent,sizeof(int),1,fp);
      fread(&currentEventRun,sizeof(int),1,fp);
      fread(gotSurf,sizeof(int),ACTIVE_SURFS,fp);
      fread(gotWave,sizeof(int),ACTIVE_SURFS*9,fp);
      fread(&curEventHeader,sizeof(AnitaEventHeader_t),1,fp);
      fclose(fp);
      std::cout << "Got header " << curEventHeader.eventNumber << "\n";
      addHeader(&curEventHeader,currentEventRun);
      for(int surf=0;surf<ACTIVE_SURFS;surf++) {
	std::cout << gotSurf[surf]  << " ";
      }
    }        
    std::cout << "\n";
  }

  fReadLastPartialEvent=1;
  fLastFileType=fileType;
}



AnitaHeaderHandler::~AnitaHeaderHandler()
{
  std::cout << "AnitaHeaderHandler::~AnitaHeaderHandler() " << startedEvent << "\n";
  //RJN Add something here to store curPSBody and gotSurf,gotWave
  char fileName[FILENAME_MAX];
  sprintf(fileName,"%s/ANITA3/db/currentEvent%s.dat",fAwareDir.c_str(),telemTypeForFile[fLastFileType]);
  
  if(startedEvent) {
    std::cout << fileName << "\n";
    //Need to save the current state of the event
    FILE *fp = fopen(fileName,"wb");
    if(fp) {
      fwrite(&curPSBody,sizeof(PedSubbedEventBody_t),1,fp);
      fwrite(&startedEvent,sizeof(int),1,fp);
      fwrite(&currentEventRun,sizeof(int),1,fp);
      fwrite(gotSurf,sizeof(int),ACTIVE_SURFS,fp);
      fwrite(gotWave,sizeof(int),ACTIVE_SURFS*9,fp);
      fwrite(&curEventHeader,sizeof(AnitaEventHeader_t),1,fp);
      fclose(fp);
    }
    else unlink(fileName);
  }
  else unlink(fileName);
}
    
void AnitaHeaderHandler::addHeader(AnitaEventHeader_t *hdPtr, UInt_t run)
{
  static unsigned int lastTime=0;
  if(hdPtr->unixTime>lastTime) {
    lastTime=hdPtr->unixTime;
    //    AwareRunDatabase::updateTouchFile(fHeaderTouchFile.c_str(),run,hdPtr->unixTime);
  }

  std::map<UInt_t,std::map<UInt_t, AnitaEventHeader_t> >::iterator it= fHeadMap.find(run);
  if(it!=fHeadMap.end()) {
    it->second.insert(std::pair<UInt_t,AnitaEventHeader_t>(hdPtr->eventNumber,*hdPtr));
  }
  else {
    std::map<UInt_t, AnitaEventHeader_t> runMap;
    runMap.insert(std::pair<UInt_t,AnitaEventHeader_t>(hdPtr->eventNumber,*hdPtr));
    fHeadMap.insert(std::pair<UInt_t,std::map<UInt_t, AnitaEventHeader_t> >(run,runMap));    
  }

}


void AnitaHeaderHandler::loopMap() 
{
  char fileName[FILENAME_MAX];
  int lastFileNumber=-1;
  std::map<UInt_t,std::map<UInt_t, AnitaEventHeader_t> >::iterator runIt;
  for(runIt=fHeadMap.begin();runIt!=fHeadMap.end();runIt++) {    
    std::map<UInt_t,AnitaEventHeader_t>::iterator it;
    FILE *outFile=NULL;
    UInt_t run=runIt->first;
    //    std::cerr << "Loop Map Run " << run << "\n";
    for(it=(runIt->second).begin();it!=(runIt->second).end();it++) {

      AnitaEventHeader_t *hdPtr=&(it->second);
      //      std::cout << hdPtr->unixTime << "\t" << hdPtr->eventNumber << "\t" << 100*(hdPtr->eventNumber/100) << "\n";    
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
	
	sprintf(fileName,"%s/run%d/event/ev%d/ev%d",fRawDir.c_str(),run,dirNumber,subDirNumber);
	gSystem->mkdir(fileName,kTRUE);
	
	sprintf(fileName,"%s/run%d/event/last",fRawDir.c_str(),run);
	//	std::cerr << fileName << "\n";
	AwareRunDatabase::touchFile(fileName);

	sprintf(fileName,"%s/run%d/event/ev%d/ev%d/hd_%d.dat.gz",fRawDir.c_str(),run,dirNumber,subDirNumber,fileNumber);
	outFile=fopen(fileName,"ab");
	//	std::cout << fileName << "\t" << outFile << "\n";
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
    lastFileNumber=0;
  }
}


void AnitaHeaderHandler::loopEventMap() 
{
  char fileName[FILENAME_MAX];
  int lastFileNumber=-1;


  std::map<UInt_t,std::map<UInt_t, PedSubbedEventBody_t> >::iterator runIt;
  std::map<UInt_t,std::map<UInt_t, AnitaEventHeader_t> >::iterator headRunIt;
  //  std::cerr << "loopEventMap: " << fEventMap.size() << "\n";
  for(runIt=fEventMap.begin();runIt!=fEventMap.end();runIt++) { 

    std::map<UInt_t,PedSubbedEventBody_t>::iterator it;
    FILE *eventFile=NULL;
    FILE *headFile=NULL;
    UInt_t run=runIt->first;
   
    headRunIt=fHeadMap.find(run);
  
    //    std::cerr << "Run: " << run << "\t" << runIt->second.size() << "\n";
    AnitaEventHeader_t tempHeader; 

    for(it=(runIt->second).begin();it!=(runIt->second).end();it++) {
      PedSubbedEventBody_t *bdPtr=&(it->second);
      AnitaEventHeader_t *hdPtr=NULL;
      //      std::cout << bdPtr->unixTime << "\t" << bdPtr->eventNumber << "\t" << 100*(bdPtr->eventNumber/100) << "\n";    
      if(curEventHeader.eventNumber==bdPtr->eventNumber) {
	std::cout << "Got current event header " << curEventHeader.eventNumber << "\n";
	hdPtr=&curEventHeader;
      } 
      else {
	std::map<UInt_t,AnitaEventHeader_t>::iterator headIt;
	if(headRunIt!=fHeadMap.end()) {
	  headIt=headRunIt->second.find(bdPtr->eventNumber);
	  if(headIt!=headRunIt->second.end()) {
	    hdPtr=&(headIt->second);
	  }
	}
	
	if(!hdPtr) {	
	  memset(&tempHeader,0,sizeof(AnitaEventHeader_t));
	  tempHeader.eventNumber=bdPtr->eventNumber;
	  fillGenericHeader(&tempHeader,PACKET_HD,sizeof(AnitaEventHeader_t));
	  hdPtr=&tempHeader;
	}
      }

      if(fMakeEventDisplaysForAware)
	plotEvent(hdPtr,bdPtr,run);

      int fileNumber=100*(bdPtr->eventNumber/100);
      //    processHeader(bdPtr);
      
      //Make base dir
      int dirNumber=(EVENTS_PER_FILE*EVENT_FILES_PER_DIR*EVENT_FILES_PER_DIR)*(bdPtr->eventNumber/(EVENTS_PER_FILE*EVENT_FILES_PER_DIR*EVENT_FILES_PER_DIR));    
      //Make sub dir
      int subDirNumber=(EVENTS_PER_FILE*EVENT_FILES_PER_DIR)*(bdPtr->eventNumber/(EVENTS_PER_FILE*EVENT_FILES_PER_DIR));
      //      std::cerr << fileNumber << "\t" << lastFileNumber << "\n";
      if(fileNumber!=lastFileNumber) {
	//Create a file
	if(eventFile) fclose(eventFile);
	if(headFile) fclose(headFile);
	eventFile=NULL;
	headFile=NULL;
	
	sprintf(fileName,"%s/run%d/event/ev%d/ev%d",fRawDir.c_str(),run,dirNumber,subDirNumber);
	gSystem->mkdir(fileName,kTRUE);
	
	sprintf(fileName,"%s/run%d/event/last",fRawDir.c_str(),run);
	AwareRunDatabase::touchFile(fileName);

	//	std::cerr << fileName << "\n";
	sprintf(fileName,"%s/run%d/event/ev%d/ev%d/psev_%d.dat.gz",fRawDir.c_str(),run,dirNumber,subDirNumber,fileNumber);
	eventFile=fopen(fileName,"ab");
	if(!eventFile ) {
	  printf("Couldn't open: %s\n",fileName);
	  return;
	}
	sprintf(fileName,"%s/run%d/event/ev%d/ev%d/evhd_%d.dat.gz",fRawDir.c_str(),run,dirNumber,subDirNumber,fileNumber);
	headFile=fopen(fileName,"ab");
	if(!headFile ) {
	  printf("Couldn't open: %s\n",fileName);
	  return;
	}
      }
      lastFileNumber=fileNumber;
      //      std::cerr << "Before write\t" << eventFile  << "\t" << bdPtr << "\n";
      fwrite(bdPtr,sizeof(PedSubbedEventBody_t),1,eventFile);
      if(hdPtr->eventNumber!=bdPtr->eventNumber) {
	std::cout << "Event number mismatch: " << hdPtr->eventNumber << "\t" << bdPtr->eventNumber << "\n";
	hdPtr->eventNumber=bdPtr->eventNumber;
      }
      fwrite(hdPtr,sizeof(AnitaEventHeader_t),1,headFile);
    }
    
    if(eventFile) fclose(eventFile);
    eventFile=NULL;
    if(headFile) fclose(headFile);
    headFile=NULL;
    lastFileNumber=0;
  }
}





void AnitaHeaderHandler::addRawSurfPacket(RawSurfPacket_t *rsPkt, UInt_t run) {
  if(rsPkt->eventNumber == curBody.eventNumber) {
    startedEvent=1;
    currentEventRun=run;
    int finished=processRawSurfPacket(rsPkt);
    if(finished) {
      addCurPSBody();
      zeroCounters();
      //Do something
    }
  }
  else { 
    if(startedEvent) {
      addCurPSBody();
      zeroCounters();
    }
    //Do something
    processRawSurfPacket(rsPkt);
  }

}

void AnitaHeaderHandler::addRawWavePacket(RawWaveformPacket_t *rwPkt, UInt_t run) {
  if(rwPkt->eventNumber == curBody.eventNumber) {
    startedEvent=1;
    currentEventRun=run;
    int finished=processRawWavePacket(rwPkt);
    if(finished) {
      //Do something
      addCurPSBody();
      zeroCounters();
    }
  }
  else { 
    //Do something
    if(startedEvent) {
      addCurPSBody();
      zeroCounters();
    }
    processRawWavePacket(rwPkt);
  }
}
    
void AnitaHeaderHandler::addEncSurfPacket(EncodedSurfPacketHeader_t *esPkt, UInt_t run) {
  if(esPkt->eventNumber == curBody.eventNumber) {
    startedEvent=1;
    currentEventRun=run;
    int finished=processEncSurfPacket(esPkt);
    if(finished) {
      addCurPSBody();
      zeroCounters();
      //Do something
    }
  }
  else { 
    if(startedEvent) {
      addCurPSBody();
      zeroCounters();
    }
    //Do something
    processEncSurfPacket(esPkt);
  }
}

void AnitaHeaderHandler::addEncPedSubbedSurfPacket(EncodedPedSubbedSurfPacketHeader_t *epssPkt, UInt_t run)
{
  //    std::cout << "addEncPedSubbedSurfPacket\t" << epssPkt->eventNumber
  //       << "\t" << curPSBody.eventNumber << std::endl;
  if(epssPkt->eventNumber == curPSBody.eventNumber) {
    startedEvent=1;
    currentEventRun=run;
    int finished=processPedSubbedEncSurfPacket(epssPkt);
    //    std::cout << startedEvent << "\t" << finished << std::endl;
    if(finished) {
      addCurPSBody();
      zeroCounters();
      //Do something
    }
  }
  else { 
    if(startedEvent) {
      addCurPSBody();
    }
    zeroCounters();
    //Do something
    processPedSubbedEncSurfPacket(epssPkt);
    startedEvent=1;
    currentEventRun=run;
    //Find link to current header
    memset(&curEventHeader,0,sizeof(AnitaEventHeader_t));
    std::map<UInt_t,std::map<UInt_t, AnitaEventHeader_t> >::iterator headRunIt;
    headRunIt=fHeadMap.find(run);
    if(headRunIt!=fHeadMap.end()) {
      std::map<UInt_t,AnitaEventHeader_t>::iterator headIt;
      headIt=headRunIt->second.find(curPSBody.eventNumber);
      if(headIt!=headRunIt->second.end()) {
	memcpy(&curEventHeader,&(headIt->second),sizeof(AnitaEventHeader_t));
      }
    }    
  }
}

void AnitaHeaderHandler::addEncPedSubbedWavePacket(EncodedPedSubbedChannelPacketHeader_t *epscPkt, UInt_t run)
{
  if(epscPkt->eventNumber == curPSBody.eventNumber) {
    startedEvent=1;    
    currentEventRun=run;
    int finished=processPedSubbedEncWavePacket(epscPkt);
    if(finished) {
      addCurPSBody();
      zeroCounters();
      //Do something
    }
  }
  else { 
    if(startedEvent) {
      addCurPSBody();
      zeroCounters();
    }
    //Do something
    processPedSubbedEncWavePacket(epscPkt);
  }
}


int AnitaHeaderHandler::processRawSurfPacket(RawSurfPacket_t *rsPkt)
{
  curBody.eventNumber=rsPkt->eventNumber;
  for(int chan=0;chan<9;chan++) {
    memcpy(&(curBody.channel[rsPkt->waveform[chan].header.chanId]),
	   &(rsPkt->waveform[chan]),sizeof(SurfChannelFull_t));	
  }
  int surf=rsPkt->waveform[0].header.chanId/9;
  gotSurf[surf]=1;
  for(int sid=0;sid<ACTIVE_SURFS;sid++) {
    if(gotSurf[sid]==0) return 0; //Still missing a SURF
  }
  //Else time to plot
  //    subtractCurrentPeds(&curBody,&curPSBody);
  return 1;
    
}

int AnitaHeaderHandler::processRawWavePacket(RawWaveformPacket_t *rwPkt)
{

  curBody.eventNumber=rwPkt->eventNumber;
  memcpy(&(curBody.channel[rwPkt->waveform.header.chanId]),
	 &(rwPkt->waveform),sizeof(SurfChannelFull_t));	
    
  int surf=rwPkt->waveform.header.chanId/9;
  int chan=rwPkt->waveform.header.chanId%9;
  gotWave[surf][chan]=1;
  for(int sid=0;sid<ACTIVE_SURFS;sid++) {
    for(int cid=0;cid<9;cid++) {
      if(gotWave[sid][cid]==0) return 0; //Still missing a channel
    }
  }
  //Else time to plot
  //    subtractCurrentPeds(&curBody,&curPSBody);
  return 1;


}

int AnitaHeaderHandler::processEncSurfPacket(EncodedSurfPacketHeader_t  *esPkt)
{
  //Or lets not
  std::cout << "Not implemented processEncSurfPacket for event " << esPkt->eventNumber << "\n";

  return 1;
}


int AnitaHeaderHandler::processPedSubbedEncSurfPacket(EncodedPedSubbedSurfPacketHeader_t *epssPkt)
{

  unsigned char *input = (unsigned char*)epssPkt;
  int count=0;
  int chanIndex;
  CompressErrorCode_t retVal=
    AnitaCompress::unpackOneSurfToPedSubbedEvent(&curPSBody,
						 input,
						 epssPkt->gHdr.numBytes);
  curPSBody.eventNumber=epssPkt->eventNumber;
  if(retVal) {

  }

  //Need to get SURF
  EncodedPedSubbedSurfPacketHeader_t *surfHdPtr;
  EncodedSurfChannelHeader_t *chanHdPtr;
        
  surfHdPtr = (EncodedPedSubbedSurfPacketHeader_t*) &input[count];
  count+=sizeof(EncodedPedSubbedSurfPacketHeader_t);
  chanHdPtr = (EncodedSurfChannelHeader_t*)&input[count];
  chanIndex=chanHdPtr->rawHdr.chanId;
  int surf=chanIndex/9;
  //  std::cout << "processPedSubbedEncSurfPacket " << chanIndex << "\t" << surf << "\n";
  gotSurf[surf]=1;
  for(int sid=0;sid<ACTIVE_SURFS;sid++) {
    if(gotSurf[sid]==0) return 0; //Still missing a SURF
  }
  //Else time to plot
  return 1;

}

int AnitaHeaderHandler::processPedSubbedEncWavePacket(EncodedPedSubbedChannelPacketHeader_t *epscPkt)
{

  unsigned char *input = (unsigned char*)epscPkt;
  int count=0;
  int chanIndex;
  CompressErrorCode_t retVal=
    AnitaCompress::unpackOneWaveToPedSubbedEvent(&curPSBody,input,epscPkt->gHdr.numBytes);

  if(retVal) {

  }

  //Need to get SURF
  EncodedPedSubbedChannelPacketHeader_t *waveHdPtr;
  EncodedSurfChannelHeader_t *chanHdPtr;
        
  waveHdPtr = (EncodedPedSubbedChannelPacketHeader_t*) &input[count];
  count+=sizeof(EncodedPedSubbedChannelPacketHeader_t);
  chanHdPtr = (EncodedSurfChannelHeader_t*)&input[count];
  chanIndex=chanHdPtr->rawHdr.chanId;
  int surf=chanIndex/9;
  int chan=chanIndex%9;

  gotWave[surf][chan]=1;
  for(int sid=0;sid<ACTIVE_SURFS;sid++) {
    for(int cid=0;cid<9;cid++) {
      if(gotWave[sid][cid]==0) return 0; //Still missing a channel
    }
  }
  //Else time to plot
  return 1;

}


void AnitaHeaderHandler::zeroCounters() {
  memset(&curPSBody,0,sizeof(curPSBody));
  for(int surf=0;surf<ACTIVE_SURFS;surf++) {
    gotSurf[surf]=0;
    for(int chan=0;chan<CHANNELS_PER_SURF;chan++) {
      gotWave[surf][chan]=0;
    }
  }
  startedEvent=0;
}


void AnitaHeaderHandler::addCurPSBody()
{
  //  std::cout << "addCurPSBody:\t" << curPSBody.eventNumber << std::endl;
  fillGenericHeader(&curPSBody,PACKET_PED_SUBBED_EVENT,sizeof(PedSubbedEventBody_t));
  //  std::cout << "curPSBody.gHdr.code\t" << curPSBody.gHdr.code << std::endl;
  //  std::cout << "curPSBody.gHdr.numBytes\t" << curPSBody.gHdr.numBytes << std::endl;
  //  std::cout << "curPSBody.gHdr.version\t" << (int)curPSBody.gHdr.verId << std::endl;
  std::map<UInt_t,std::map<UInt_t, PedSubbedEventBody_t> >::iterator it= fEventMap.find(currentEventRun);
  if(it!=fEventMap.end()) {
    it->second.insert(std::pair<UInt_t,PedSubbedEventBody_t>(curPSBody.eventNumber,curPSBody));
  }
  else {
    std::map<UInt_t, PedSubbedEventBody_t> runMap;
    runMap.insert(std::pair<UInt_t,PedSubbedEventBody_t>(curPSBody.eventNumber,curPSBody));
    fEventMap.insert(std::pair<UInt_t,std::map<UInt_t, PedSubbedEventBody_t> >(currentEventRun,runMap));    
  }


  // AnitaEventHeader_t *hdPtr=NULL;
  // //std::cout << bdPtr->eventNumber << "\t" << 100*(bdPtr->eventNumber/100) << "\n";    
   
  // //  std::cerr << "Before plotEvent\t" << hdPtr << "\n";;
  // if(fMakeEventDisplaysForAware)
  //   plotEvent(hdPtr,&curPSBody,currentEventRun);
  // //      std::cerr << "After plotEvent\n";



}

void AnitaHeaderHandler::plotEvent(AnitaEventHeader_t *hdPtr,PedSubbedEventBody_t *bdPtr,int run) {
  //  std::cout << "plotEvent:\t"  << hdPtr << "\t" << bdPtr << "\t" << run <<  "\n";
  //  for(int channel=0;channel<NUM_DIGITZED_CHANNELS;channel++) {
  //    std::cout << channel << "\t" << (int)bdPtr->channel[channel].header.firstHitbus << "\n";
  //  }

  
  RawAnitaEvent *fTheEvent = new RawAnitaEvent(bdPtr);
  //  std::cout << "Made fTheEvent\n";
  RawAnitaHeader *fTheHead=NULL;
  static TCanvas *fMagicCanvas=NULL;
  static TPad *fMagicMainPad=NULL;
  static TPad *fMagicEventInfoPad=NULL;
  char eventDir[FILENAME_MAX];
  sprintf(eventDir,"%s/ANITA3/event",fAwareDir.c_str());

  if(hdPtr) {


    if(hdPtr->errorFlag&0x2) {
      //Have SURF 1 sync slip
    }
    else {
      fTheEvent->surfEventId[0]=hdPtr->turfEventId;
    }
    if(hdPtr->errorFlag&0x4) {
      //Have SURF 1 sync slip
    }
    else {
      fTheEvent->surfEventId[9]=hdPtr->turfEventId;
    }
    for(int surf=0;surf<8;surf++) {
      if(hdPtr->surfSlipFlag & (1<<surf)) {
	//Have SURF +2 sync slip
      }
      else {
	fTheEvent->surfEventId[surf]=hdPtr->turfEventId;
      }
    }
    for(int surf=8;surf<ACTIVE_SURFS;surf++) {
      fTheEvent->surfEventId[surf]=hdPtr->turfEventId;
    }
    

    UInt_t triggerTimeNs=1e9*hdPtr->turfio.trigTime/hdPtr->turfio.c3poNum;
    fTheHead = new RawAnitaHeader(hdPtr,run,hdPtr->unixTime,hdPtr->unixTime,triggerTimeNs,1);
  }
  else {
    AnitaEventHeader_t tempHeader;
    memset(&tempHeader,0,sizeof(AnitaEventHeader_t));
    fillGenericHeader(&tempHeader,PACKET_HD,sizeof(AnitaEventHeader_t));
    if(fTheHead) delete fTheHead;
    fTheHead = new RawAnitaHeader(&tempHeader,0,0,0,0,0);
    fTheHead->eventNumber=bdPtr->eventNumber;    
  }


  AnitaCanvasMaker *gEventCanvasMaker=AnitaCanvasMaker::Instance();
  char pngName[180];
  if(!fMagicCanvas) {
    fMagicCanvas = new TCanvas("canMagic","canMagic",1200,800);
    fMagicCanvas->cd();
  }
  if(!fMagicMainPad) {
    fMagicCanvas->cd();
    fMagicMainPad= new TPad("canMagicMain","canMagicMain",0,0,1,0.9);
    fMagicMainPad->Draw();
    fMagicCanvas->Update();
  }
  if(!fMagicEventInfoPad) {
    fMagicCanvas->cd();
    fMagicEventInfoPad= new TPad("canMagicEventInfo","canMagicEventInfo",0.2,0.91,0.8,0.99);
    fMagicEventInfoPad->Draw();
    fMagicCanvas->Update();
  } 
  
  UsefulAnitaEvent *usefulEventPtr = new UsefulAnitaEvent(fTheEvent,WaveCalType::kVTFast);  
  


  gEventCanvasMaker->getEventInfoCanvas(usefulEventPtr,fTheHead,fMagicEventInfoPad);
  gEventCanvasMaker->fMinVoltLimit=-100;
  gEventCanvasMaker->fMaxVoltLimit=100;
  
  gEventCanvasMaker->quickGetEventViewerCanvasForWebPlottter(usefulEventPtr,fTheHead,fMagicMainPad);
    
  
  
  
  
  char dirName[FILENAME_MAX];
  sprintf(dirName,"%s/all/run%d/ev%u",eventDir,run,EVENTS_PER_DIR*(fTheEvent->eventNumber/EVENTS_PER_DIR));
  gSystem->mkdir(dirName,kTRUE);
  sprintf(pngName,"%s/event_%u.png",dirName,fTheEvent->eventNumber);
  unlink(pngName);
  fMagicCanvas->Print(pngName);
  //  fMagicMainPad->Clear();
  
  char lastName[FILENAME_MAX];
  sprintf(lastName,"%s/lastEvent.png",eventDir);
  unlink(lastName);
  link(pngName,lastName);
  
  if(fTheHead->eventNumber == fTheEvent->eventNumber) {

    static unsigned int lastTime=0;
    if(fTheHead->realTime>lastTime) {
      lastTime=fTheHead->realTime;
      AwareRunDatabase::updateTouchFile(fEventTouchFile.c_str(),run,lastTime);
    }


    //Now make priority link
    char linkName[FILENAME_MAX];
    sprintf(dirName,"%s/pri%d/ev%u",eventDir,fTheHead->priority&0xf,10000*(fTheEvent->eventNumber/10000));
    //       if(!is_dir(dirName)) 
    gSystem->mkdir(dirName,kTRUE);   
    sprintf(linkName,"%s/event_%u.png",dirName,fTheEvent->eventNumber);
    link(pngName,linkName);
    
    sprintf(linkName,"%s/currentPri%d.png",eventDir,fTheHead->priority&0xf);
    unlink(linkName);
    link(pngName,linkName);
    
    
    char evTouchFile[FILENAME_MAX];
    sprintf(evTouchFile,"%s/lastPri%d",eventDir,fTheHead->priority&0xf);
    
    //Touch File
    struct utimbuf ut;
    ut.actime=fTheHead->realTime;
    ut.modtime=fTheHead->realTime;
    
    struct stat buf;
    int retVal2=stat(evTouchFile,&buf);
    if(retVal2==0) {
      if(buf.st_mtime<ut.modtime)
	utime(evTouchFile,&ut);
    }
    else {
      ofstream Touch(evTouchFile);
      Touch.close();
      utime(evTouchFile,&ut);
    }
    
    
    
    
    //Now make trig type link       
    const char *trigNames[4]={"rf","pps1","pps2","soft"};
    int trigTypes[4]={0x1,0x2,0x4,0x8};
    
    for(int trigInd=0;trigInd<4;trigInd++) {
      if(fTheHead->trigType&trigTypes[trigInd]) {
	sprintf(dirName,"%s/%s/ev%u",eventDir,trigNames[trigInd],10000*(fTheEvent->eventNumber/10000));
	gSystem->mkdir(dirName,kTRUE);   
	sprintf(linkName,"%s/event_%u.png",dirName,fTheEvent->eventNumber);
	link(pngName,linkName);
	
	sprintf(linkName,"%s/current%sTrig.png",eventDir,trigNames[trigInd]);
	unlink(linkName);
	link(pngName,linkName);
	
	
	sprintf(evTouchFile,"%s/last%sTrig",eventDir,trigNames[trigInd]);
	
	//Touch File
	ut.actime=fTheHead->realTime;
	ut.modtime=fTheHead->realTime;
	
	retVal2=stat(evTouchFile,&buf);
	if(retVal2==0) {
	  if(buf.st_mtime<ut.modtime)
	    utime(evTouchFile,&ut);
	}
	else {
	  ofstream Touch(evTouchFile);
	  Touch.close();
	  utime(evTouchFile,&ut);
	}
	
      }
    }
  }
  delete usefulEventPtr;

}
