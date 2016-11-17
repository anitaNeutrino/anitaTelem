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


namespace AnitaTelemFileType {
  typedef enum {
    kAnitaTelemLos=0,
    kAnitaTelemTdrss,
    kAnitaTelemOpenport,
    kAnitaTelemSlowTdrss,
    kAnitaTelemIridium,
    kNotATelemType
  } AnitaTelemFileType_t;
}

class AnitaGenericHeaderHandler 
{   
public:
  AnitaGenericHeaderHandler(std::string awareDir);
  ~AnitaGenericHeaderHandler();
  
  void addGenericHeader(GenericHeader_t *hdPtr);

  void newFile(AnitaTelemFileType::AnitaTelemFileType_t fileType,int runNum, int fileNum, unsigned int unixTime);
  void writeFileSummary();
  

private:

  int getLogicalPacketCode(PacketCode_t code);
  PacketCode_t getRealPacketCode(int logicCode);
  std::string fAwareDir;
  AnitaTelemFileType::AnitaTelemFileType_t fFileType;
  int fCurrentFile;
  int fCurrentRun;
  unsigned int fCurrentFileTime;
  std::map<UInt_t,GenericHeader_t> fGhdMap;



};

#endif //ANITAGENERICHEADERHANDLER_H


