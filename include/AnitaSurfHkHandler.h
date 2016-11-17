////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered SurfHkDataStruct_t      /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ANITASURFHKHANDLER_H
#define ANITASURFHKHANDLER_H

#include <map>
#include <string>


#include "simpleStructs.h"
#include "plotUtils.h"    


class AnitaSurfHkHandler 
{   
public:
  AnitaSurfHkHandler(std::string rawDir);
    ~AnitaSurfHkHandler();
    
    void addSurfHk(FullSurfHkStruct_t *hkPtr,int run);
    void addAveragedSurfHk(AveragedSurfHkStruct_t *hkPtr,int run);
    void loopMap();
    void loopAvgMap();


private:
    std::string fRawDir;
    std::map<UInt_t,std::map<UInt_t,FullSurfHkStruct_t> > fSurfHkMap;
    std::map<UInt_t,std::map<UInt_t,AveragedSurfHkStruct_t> > fAvgSurfHkMap;


};

#endif //ANITASURFHKHANDLER_H


