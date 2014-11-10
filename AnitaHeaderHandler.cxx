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
#include "AnitaPacketUtil.h"
#include "simpleStructs.h"


#define HACK_FOR_ROOT

#define C3PO_AVG 20

#define EVENTS_PER_FILE 100
#define EVENT_FILES_PER_DIR 100


AnitaHeaderHandler::AnitaHeaderHandler(std::string rawDir)
  :fRawDir(rawDir),startedEvent(0)
{
  zeroCounters();

}

AnitaHeaderHandler::~AnitaHeaderHandler()
{


}
    
void AnitaHeaderHandler::addHeader(AnitaEventHeader_t *hdPtr, UInt_t run)
{
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
    for(it=(runIt->second).begin();it!=(runIt->second).end();it++) {

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
	
	sprintf(fileName,"%s/run%d/event/ev%d/ev%d",fRawDir.c_str(),run,dirNumber,subDirNumber);
	gSystem->mkdir(fileName,kTRUE);
	sprintf(fileName,"%s/run%d/event/ev%d/ev%d/hd_%d.dat.gz",fRawDir.c_str(),run,dirNumber,subDirNumber,fileNumber);
	outFile=fopen(fileName,"ab");
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
}


void AnitaHeaderHandler::loopEventMap() 
{
  char fileName[FILENAME_MAX];
  int lastFileNumber=-1;

  std::map<UInt_t,std::map<UInt_t, PedSubbedEventBody_t> >::iterator runIt;
  for(runIt=fEventMap.begin();runIt!=fEventMap.end();runIt++) {    
    
    std::map<UInt_t,PedSubbedEventBody_t>::iterator it;
    FILE *outFile=NULL;
    UInt_t run=runIt->first;
    for(it=(runIt->second).begin();it!=(runIt->second).end();it++) {
      PedSubbedEventBody_t *bdPtr=&(it->second);
      //    std::cout << bdPtr->unixTime << "\t" << bdPtr->eventNumber << "\t" << 100*(bdPtr->eventNumber/100) << "\n";    
      int fileNumber=100*(bdPtr->eventNumber/100);
      //    processHeader(bdPtr);
      
      //Make base dir
      int dirNumber=(EVENTS_PER_FILE*EVENT_FILES_PER_DIR*EVENT_FILES_PER_DIR)*(bdPtr->eventNumber/(EVENTS_PER_FILE*EVENT_FILES_PER_DIR*EVENT_FILES_PER_DIR));    
      //Make sub dir
      int subDirNumber=(EVENTS_PER_FILE*EVENT_FILES_PER_DIR)*(bdPtr->eventNumber/(EVENTS_PER_FILE*EVENT_FILES_PER_DIR));
      
      if(fileNumber!=lastFileNumber) {
	//Create a file
	if(outFile) fclose(outFile);
	outFile=NULL;
	
	sprintf(fileName,"%s/run%d/event/ev%d/ev%d",fRawDir.c_str(),run,dirNumber,subDirNumber);
	gSystem->mkdir(fileName,kTRUE);
	sprintf(fileName,"%s/run%d/event/ev%d/ev%d/psev_%d.dat.gz",fRawDir.c_str(),run,dirNumber,subDirNumber,fileNumber);
	outFile=fopen(fileName,"ab");
	if(!outFile ) {
	  printf("Couldn't open: %s\n",fileName);
	  return;
	}
      }
      lastFileNumber=fileNumber;
      fwrite(bdPtr,sizeof(PedSubbedEventBody_t),1,outFile);
    }
    
    if(outFile) fclose(outFile);
    outFile=NULL;
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
  //  cout << "addEncPedSubbedSurfPacket\t" << epssPkt->eventNumber
  //   << "\t" << curPSBody.eventNumber << endl;
  if(epssPkt->eventNumber == curPSBody.eventNumber) {
    startedEvent=1;
    currentEventRun=run;
    int finished=processPedSubbedEncSurfPacket(epssPkt);
    //      cout << startedEvent << "\t" << finished << endl;
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
  //  cout << "plotEvent:\t" << curPSBody.eventNumber << endl;
  fillGenericHeader(&curPSBody,PACKET_PED_SUBBED_EVENT,sizeof(PedSubbedEventBody_t));
  std::map<UInt_t,std::map<UInt_t, PedSubbedEventBody_t> >::iterator it= fEventMap.find(currentEventRun);
  if(it!=fEventMap.end()) {
    it->second.insert(std::pair<UInt_t,PedSubbedEventBody_t>(curPSBody.eventNumber,curPSBody));
  }
  else {
    std::map<UInt_t, PedSubbedEventBody_t> runMap;
    runMap.insert(std::pair<UInt_t,PedSubbedEventBody_t>(curPSBody.eventNumber,curPSBody));
    fEventMap.insert(std::pair<UInt_t,std::map<UInt_t, PedSubbedEventBody_t> >(currentEventRun,runMap));    
  }

}
