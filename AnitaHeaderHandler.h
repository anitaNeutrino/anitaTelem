////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered AnitaEventHeader_t       /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ANITAHEADERHANDLER_H
#define ANITAHEADERHANDLER_H

#include <map>


#include "simpleStructs.h"
#include "plotUtils.h"    


class AnitaHeaderHandler 
{   
public:
    AnitaHeaderHandler();
    ~AnitaHeaderHandler();
    
    void addHeader(AnitaEventHeader_t *hdPtr);
    void loopMap();


private:
    std::map<UInt_t, AnitaEventHeader_t> fHeadMap; 


};

#endif //ANITAHEADERHANDLER_H


