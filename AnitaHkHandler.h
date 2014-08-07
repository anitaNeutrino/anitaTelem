////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered HkDataStruct_t      /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ANITAHKHANDLER_H
#define ANITAHKHANDLER_H

#include <map>


#include "simpleStructs.h"
#include "plotUtils.h"    


class AnitaHkHandler 
{   
public:
    AnitaHkHandler(int run);
    ~AnitaHkHandler();
    
    void addHk(HkDataStruct_t *hkPtr);
    void loopMap();


private:
    int fRun;
    std::map<UInt_t, HkDataStruct_t> fHkMap;


};

#endif //ANITAHKHANDLER_H


