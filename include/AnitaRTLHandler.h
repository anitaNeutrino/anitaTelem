  ////////////////////////////////////////////////////////////////////////
 ///// class to handle telemetered RTL packet                  ///////
 /////                                                         ///////
  ///// Cosmin Deaconu <cozzyd@kicp.uchicago.edu>  Nov 2016   //////
    ///////////////////////////////////////////////////////////////

#ifndef ANITA_RTL_HANDLER_H
#define ANITA_RTL_HANDLER_H

#include <map>
#include <string>
#include "simpleStructs.h"
#include "plotUtils.h"


class AnitaRTLHandler 
{   
public:
    AnitaRTLHandler(std::string rawDir);
    ~AnitaRTLHandler();
    
    void addRTL(RtlSdrPowerSpectraStruct_t *notchPtr, int run);
    void loopMap();

private:
    std::string fRawDir;
    std::map<UInt_t,std::map<UInt_t, RtlSdrPowerSpectraStruct_t> > fRTLMap;

};

#endif //ANITA_RTL_HANDLER_H


