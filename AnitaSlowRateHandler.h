////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered SlowRateFull_t          /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- December 2014                        /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ANITASLOWRATEHANDLER_H
#define ANITASLOWRATEHANDLER_H

#include <map>
#include <string>

#include "AnitaGenericHeaderHandler.h"
#include "simpleStructs.h"
#include "plotUtils.h"    


class AnitaSlowRateHandler 
{   
public:
  AnitaSlowRateHandler(std::string rawDir);
  ~AnitaSlowRateHandler();
  
  void addSlowRate(SlowRateFull_t *slowPtr,int run,AnitaTelemFileType::AnitaTelemFileType_t fileType, int seqNum);
  void loopMap();
  void loopOtherMap();


private:
    std::string fRawDir;
    std::map<UInt_t,std::map<UInt_t, SlowRateFull_t> > fSlowRateMap;

};

#endif //ANITASLOWRATEHANDLER_H


