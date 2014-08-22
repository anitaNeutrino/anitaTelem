////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered TurfRateStruct_t    /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ANITATURFRATEHANDLER_H
#define ANITATURFRATEHANDLER_H

#include <map>
#include <string>


#include "simpleStructs.h"
#include "plotUtils.h"    


class AnitaTurfRateHandler 
{   
public:
  AnitaTurfRateHandler(std::string rawDir,int run);
    ~AnitaTurfRateHandler();
    
    void addTurfRate(TurfRateStruct_t *hkPtr);
    void addSumTurfRate(SummedTurfRateStruct_t *hkPtr);
    void loopMap();
    void loopSumMap();


private:
    std::string fRawDir;
    int fRun;
    std::map<UInt_t,TurfRateStruct_t> fTurfRateMap;
    std::map<UInt_t,SummedTurfRateStruct_t> fSumTurfRateMap;


};

#endif //ANITATURFRATEHANDLER_H


