////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered GpsDataStruct_t      /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ANITAGPSHANDLER_H
#define ANITAGPSHANDLER_H

#include <map>


#include "simpleStructs.h"
#include "plotUtils.h"    


class AnitaGpsHandler 
{   
public:
    AnitaGpsHandler(int run);
    ~AnitaGpsHandler();
    
    void addG12Pos(GpsG12PosStruct_t *g12Ptr);
    void addG12Sat(GpsG12SatStruct_t *g12Ptr);

    void loopG12PosMap();
    void loopG12SatMap();


private:
    int fRun;
    std::map<UInt_t, GpsG12PosStruct_t> fG12PosMap;
    std::map<UInt_t, GpsG12SatStruct_t> fG12SatMap;


};

#endif //ANITAGPSHANDLER_H


