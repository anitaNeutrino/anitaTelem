////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered AnitaEventHeader_t       /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#include "AnitaHeaderHandler.h"

#include <iostream>


AnitaHeaderHandler::AnitaHeaderHandler()
{


}

AnitaHeaderHandler::~AnitaHeaderHandler()
{


}
    
void AnitaHeaderHandler::addHeader(AnitaEventHeader_t *hdPtr)
{
   fHeadMap.insert(std::pair<UInt_t,AnitaEventHeader_t>(hdPtr->eventNumber,*hdPtr));

}


void AnitaHeaderHandler::loopMap() 
{
   std::map<UInt_t,AnitaEventHeader_t>::iterator it;
   for(it=fHeadMap.begin();it!=fHeadMap.end();it++) {
      AnitaEventHeader_t *hdPtr=&(it->second);
      std::cout << hdPtr->unixTime << "\t" << hdPtr->eventNumber << "\n";
   }

}
