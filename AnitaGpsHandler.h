////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered GpsDataStruct_t      /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ANITAGPSHANDLER_H
#define ANITAGPSHANDLER_H

#include <map>
#include <string>


#include "simpleStructs.h"
#include "plotUtils.h"    


class AnitaGpsHandler 
{   
public:
  AnitaGpsHandler(std::string rawDir);
    ~AnitaGpsHandler();
    
    void addG12Pos(GpsG12PosStruct_t *g12Ptr, int run);
    void addG12Sat(GpsG12SatStruct_t *g12Ptr, int run);
    void addGpsGga(GpsGgaStruct_t *ggaPtr, int run);
    void addAdu5Pat(GpsAdu5PatStruct_t *gpsPtr, int run);
    void addAdu5Sat(GpsAdu5SatStruct_t *gpsPtr, int run);
    void addAdu5Vtg(GpsAdu5VtgStruct_t *gpsPtr, int run);

    void loopG12PosMap();
    void loopG12SatMap();
    void loopGpsGgaMaps();
    void loopAdu5PatMaps();
    void loopAdu5SatMaps();
    void loopAdu5VtgMaps();


private:
    int whichGps(PacketCode_t code); ///< 0 is ADU5A, 1 is ADU5B, 2 is G12
    const char *getGpsName(int gpsId);
    std::string fRawDir;

    std::map<UInt_t,std::map<UInt_t, GpsG12PosStruct_t> > fG12PosMap;
    std::map<UInt_t,std::map<UInt_t, GpsG12SatStruct_t> > fG12SatMap;
    std::map<UInt_t,std::map<UInt_t, GpsGgaStruct_t> > fGpsGgaMap[3];  ///< [ADU5A,ADU5B,G12]
    std::map<UInt_t,std::map<UInt_t, GpsAdu5PatStruct_t> > fAdu5PatMap[2];  ///< [ADU5A,ADU5B]
    std::map<UInt_t,std::map<UInt_t, GpsAdu5SatStruct_t> > fAdu5SatMap[2];  ///< [ADU5A,ADU5B]
    std::map<UInt_t,std::map<UInt_t, GpsAdu5VtgStruct_t> > fAdu5VtgMap[2];  ///< [ADU5A,ADU5B]


};

#endif //ANITAGPSHANDLER_H


