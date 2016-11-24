  ////////////////////////////////////////////////////////////////////////
 ///// class to handle telemetered TUFF packet                  ///////
 /////                                                         ///////
  ///// Cosmin Deaconu <cozzyd@kicp.uchicago.edu>  Nov 2016   //////
    ///////////////////////////////////////////////////////////////

#ifndef ANITA_TUFF_HANDLER
#define ANITA_TUFF_HANDLER_H

#include <map>
#include <string>
#include "simpleStructs.h"
#include "plotUtils.h"


class AnitaTuffHandler 
{   
public:
    AnitaTuffHandler(std::string rawDir);
    ~AnitaTuffHandler();
    
    void addTuffStatus(TuffNotchStatus_t *notchPtr, int run);
    void addRawCommand(TuffRawCmd_t *rawPtr, int run);
    void loopNotchMap();
    void loopRawCmdMap();

private:
    std::string fRawDir;
    std::map<UInt_t,std::map<UInt_t, TuffRawCmd_t> > fRawMap;
    std::map<UInt_t,std::map<UInt_t, TuffNotchStatus_t> > fNotchMap;

};

#endif //ANITA_TUFF_HANDLER_H


