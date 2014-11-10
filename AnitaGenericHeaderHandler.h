////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered AnitaEventHeader_t       /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ANITAGENERICHEADERHANDLER_H
#define ANITAGENERICHEADERHANDLER_H

#include <map>
#include <string>

#include "simpleStructs.h"
#include "plotUtils.h"    


class AnitaGenericHeaderHandler 
{   
public:
  AnitaGenericHeaderHandler(std::string rawDir,int run);
  ~AnitaGenericHeaderHandler();
  
  void addGenericHeader(GenericHeader_t *hdPtr);
  void startNewRun(int losOrTdrss, int dirNumber, int runNumber);


  void loopMap();
  

private:

  std::string fRawDir;
  int fRun;
  std::map<UInt_t, GenericHeader_t> fGhdMap;



};

#endif //ANITAGENERICHEADERHANDLER_H


