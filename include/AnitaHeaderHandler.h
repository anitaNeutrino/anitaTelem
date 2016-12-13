////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered AnitaEventHeader_t       /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ANITAHEADERHANDLER_H
#define ANITAHEADERHANDLER_H

#include <map>
#include <string>

#include "simpleStructs.h"
#include "plotUtils.h"    
#include "AnitaGenericHeaderHandler.h"

class AnitaHeaderHandler 
{   
public:
  AnitaHeaderHandler(std::string rawDir,std::string awareDir,int makeEventPngsForAware=0);
  ~AnitaHeaderHandler();
  
  void addHeader(AnitaEventHeader_t *hdPtr, UInt_t run);


  void addRawSurfPacket(RawSurfPacket_t *rsPkt,UInt_t run);
  void addRawWavePacket(RawWaveformPacket_t *rwPkt, UInt_t run);
  void addEncSurfPacket(EncodedSurfPacketHeader_t *esPkt, UInt_t run);
  void addEncPedSubbedSurfPacket(EncodedPedSubbedSurfPacketHeader_t *epssPkt, UInt_t run);
  void addEncPedSubbedWavePacket(EncodedPedSubbedChannelPacketHeader_t *epscPkt, UInt_t run);

  void loopMap();
  void loopEventMap();
  
  void newFile(AnitaTelemFileType::AnitaTelemFileType_t fileType);

private:
  void zeroCounters();
  void addCurPSBody();
  int processRawSurfPacket(RawSurfPacket_t *rsPkt);
  int processRawWavePacket(RawWaveformPacket_t *rwPkt);
  int processEncSurfPacket(EncodedSurfPacketHeader_t *esPkt);
  int processPedSubbedEncSurfPacket(EncodedPedSubbedSurfPacketHeader_t *epssPkt, UInt_t run);
  int processPedSubbedEncWavePacket(EncodedPedSubbedChannelPacketHeader_t *epscPkt);
  void plotEvent(AnitaEventHeader_t *hdPtr,PedSubbedEventBody_t *bdPtr,int run) ;
  void addToRunSurfPacketFile(UInt_t run, UInt_t eventNumber, UInt_t surf, UInt_t packetNumber);

  std::string fRawDir;
  std::string fAwareDir;
  std::string fEventTouchFile;
  std::string fHeaderTouchFile;
  std::map<UInt_t,std::map<UInt_t, AnitaEventHeader_t> > fHeadMap;
  std::map<UInt_t,std::map<UInt_t, PedSubbedEventBody_t> > fEventMap;

  int fMakeEventDisplaysForAware;

  int fReadLastPartialEvent;

  int startedEvent;
  int currentEventRun;

  int gotSurf[ACTIVE_SURFS];
  int gotWave[ACTIVE_SURFS][9];

  AnitaTelemFileType::AnitaTelemFileType_t fLastFileType;

  AnitaEventBody_t curBody; //Only needed as temp variable
  PedSubbedEventBody_t curPSBody; //What we actually plot with
  AnitaEventHeader_t curEventHeader; //


};

#endif //ANITAHEADERHANDLER_H


