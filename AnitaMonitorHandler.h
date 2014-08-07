////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered MonitorDataStruct_t      /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ANITAMONITORHANDLER_H
#define ANITAMONITORHANDLER_H

#include <map>


#include "simpleStructs.h"
#include "plotUtils.h"    


class AnitaMonitorHandler 
{   
public:
    AnitaMonitorHandler(int run);
    ~AnitaMonitorHandler();
    
    void addMonitor(MonitorStruct_t *monitorPtr);
    void addOtherMonitor(OtherMonitorStruct_t *monitorPtr);
    void loopMap();
    void loopOtherMap();


private:
    int fRun;
    std::map<UInt_t, MonitorStruct_t> fMonitorMap;
    std::map<UInt_t, OtherMonitorStruct_t> fOtherMonitorMap;


};

#endif //ANITAMONITORHANDLER_H


