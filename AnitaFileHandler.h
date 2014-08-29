////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to unzip and write telemetered files           /////////
//////                                                             /////////
////// rjn@hep.ucl.ac.uk --- September 2006                        /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ANITAFILEHANDLER_H
#define ANITAFILEHANDLER_H


#include <string>

#include "simpleStructs.h"

class AnitaFileHandler 
{   
public:
    AnitaFileHandler(std::string rawDir,int run);
    ~AnitaFileHandler() {;}
    
    void processFile(ZippedFile_t *zfPtr);
    
private:
    void getOutputName(char *outputFilename,char *linkName,ZippedFile_t *zfPtr,int useSegment);
    std::string fRawDir;
    int fRun;

};

#endif //ANITAFILEHANDLER_H


