////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered GpuPhiSectorPowerSpectrumStruct_t  /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- December 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ANITAGPUHANDLER_H
#define ANITAGPUHANDLER_H

#include <map>
#include <string>

#include "simpleStructs.h"
#include "plotUtils.h"    


class AnitaGpuHandler 
{   
public:
  AnitaGpuHandler(std::string rawDir);
    ~AnitaGpuHandler();
    
    void addGpu(GpuPhiSectorPowerSpectrumStruct_t *gpuPtr,int run);
    void loopMap();


private:
    std::string fRawDir;
    std::map<UInt_t,std::map<UInt_t, GpuPhiSectorPowerSpectrumStruct_t> > fGpuMap;
};

#endif //ANITAGPUHANDLER_H


