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


class AnitaHeaderHandler 
{   
public:
  AnitaHeaderHandler(std::string rawDir,int run);
  ~AnitaHeaderHandler();
  
  void addHeader(AnitaEventHeader_t *hdPtr);


  void addRawSurfPacket(RawSurfPacket_t *rsPkt);
  void addRawWavePacket(RawWaveformPacket_t *rwPkt);
  void addEncSurfPacket(EncodedSurfPacketHeader_t *esPkt);
  void addEncPedSubbedSurfPacket(EncodedPedSubbedSurfPacketHeader_t *epssPkt);
  void addEncPedSubbedWavePacket(EncodedPedSubbedChannelPacketHeader_t *epscPkt);

  void loopMap();
  void loopEventMap();
  

private:
  void zeroCounters();
  void addCurPSBody();
  int processRawSurfPacket(RawSurfPacket_t *rsPkt);
  int processRawWavePacket(RawWaveformPacket_t *rwPkt);
  int processEncSurfPacket(EncodedSurfPacketHeader_t *esPkt);
  int processPedSubbedEncSurfPacket(EncodedPedSubbedSurfPacketHeader_t *epssPkt);
  int processPedSubbedEncWavePacket(EncodedPedSubbedChannelPacketHeader_t *epscPkt);
    

  std::string fRawDir;
  int fRun;
  std::map<UInt_t, AnitaEventHeader_t> fHeadMap;
  std::map<UInt_t, PedSubbedEventBody_t> fEventMap;

  int startedEvent;

  int gotSurf[ACTIVE_SURFS];
  int gotWave[ACTIVE_SURFS][9];

  AnitaEventBody_t curBody; //Only needed as temp variable
  PedSubbedEventBody_t curPSBody; //What we actually plot with



};

#endif //ANITAHEADERHANDLER_H


