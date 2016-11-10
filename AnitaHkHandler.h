////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered HkDataStruct_t      /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ANITAHKHANDLER_H
#define ANITAHKHANDLER_H

#include <map>
#include <string>


#include "simpleStructs.h"
#include "plotUtils.h"    


class AnitaHkHandler 
{   
public:
    AnitaHkHandler(std::string rawDir);
    ~AnitaHkHandler();
    
    void addHk(HkDataStruct_t *hkPtr, int run);
    void addSSHk(SSHkDataStruct_t *hkPtr, int run);
    void loopMap();
    void loopSSMap();

    //For Peter's status page
    void updateStatusPage(HkDataStruct_t *hkPtr);
    

private:
    std::string fRawDir;
    std::map<UInt_t,std::map<UInt_t, HkDataStruct_t> > fHkMap;
    std::map<UInt_t,std::map<UInt_t, SSHkDataStruct_t> > fSSHkMap;


};

#endif //ANITAHKHANDLER_H


