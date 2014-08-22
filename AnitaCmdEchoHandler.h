////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle making plots from CommandEcho_t;     /////////
//////                                                             /////////
////// rjn@hep.ucl.ac.uk --- September 2006                        /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ANITACMDECHOHANDLER_H
#define ANITACMDECHOHANDLER_H


#include <map>
#include <string>


#include "simpleStructs.h"
#include "plotUtils.h"    


class AnitaCmdEchoHandler 
{   
public:
    AnitaCmdEchoHandler(std::string rawDir,int run);
    ~AnitaCmdEchoHandler();
    
    void addCmdEcho(CommandEcho_t *echoPtr);
    void loopMaps();
    
private:

    std::string fRawDir;
    int fRun;
    std::multimap<UInt_t, CommandEcho_t> fCmdEchoMap[2];
};

#endif //ANITACMDECHOHANDLER_H


