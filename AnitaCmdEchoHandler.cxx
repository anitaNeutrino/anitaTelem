////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered AnitaEventHk_t       /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#include "AnitaCmdEchoHandler.h"

#include <iostream>
#include <fstream>
#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"
#include "simpleStructs.h"
#include "CommandEcho.h"




#define HACK_FOR_ROOT

#define HK_PER_FILE 1000

AnitaCmdEchoHandler::AnitaCmdEchoHandler(std::string rawDir,int run)
  :fRawDir(rawDir),fRun(run)
{


}

AnitaCmdEchoHandler::~AnitaCmdEchoHandler()
{


}
    
void AnitaCmdEchoHandler::addCmdEcho(CommandEcho_t *echoPtr)
{
   int fromPayload=0;
   if(echoPtr->gHdr.code & CMD_FROM_PAYLOAD) {
      fromPayload=1;
   }
   else {
      fromPayload=0;
   }   
   fCmdEchoMap[fromPayload].insert(std::pair<UInt_t,CommandEcho_t>(echoPtr->unixTime,*echoPtr));
   
}


void AnitaCmdEchoHandler::loopMaps() 
{
  char fileName[FILENAME_MAX];
  std::map<UInt_t,CommandEcho_t>::iterator it;
  const char *fromString[2]={"Ground","Payload"};
  
  for(int payloadFlag=0;payloadFlag<2;payloadFlag++) {
     std::ofstream JsonFile;

     if(fCmdEchoMap[payloadFlag].size()>0) {
	sprintf(fileName,"%s/run%d/cmdecho",fRawDir.c_str(),fRun);
	gSystem->mkdir(fileName,kTRUE);
	sprintf(fileName,"%s/run%d/cmdecho/cmd%s.json",fRawDir.c_str(),fRun,fromString[payloadFlag]);
	JsonFile.open(fileName,std::ofstream::app);       
	JsonFile << "{\n";
	JsonFile << "\"cmds\":{\n";
	JsonFile << "\"from\":\"" << fromString[payloadFlag] << "\",\n";
	JsonFile << "\"numEchos\":" << fCmdEchoMap[payloadFlag].size() << ",\n";
	JsonFile << "\"cmdList\":[\n";
		
	int firstTime=1;
	for(it=fCmdEchoMap[payloadFlag].begin();it!=fCmdEchoMap[payloadFlag].end();it++) {
	   CommandEcho_t *echoPtr=&(it->second);	
	   CommandEcho echo(-1,echoPtr->unixTime,echoPtr);
	   if(!firstTime) JsonFile << ",";
	   JsonFile << "{\n";
	   JsonFile << "\"unixTime\":" << echo.realTime << ",\n";
	   JsonFile << "\"flag\":" << (int)echo.goodFlag << ",\n";
	   JsonFile << "\"description\": \"" << echo.getCommandAsString() << "\",\n";
	   JsonFile << "\"cmd\":["; 
	   for(int cmdByte=0;cmdByte<echo.numCmdBytes;cmdByte++) {
	      if(cmdByte!=0) JsonFile << ",";
	      JsonFile << (int)echo.cmd[cmdByte];
	   }
	   JsonFile << "]\n";
	   JsonFile << "}\n";
	   firstTime=0;
	}
	JsonFile << "]\n";
	JsonFile << "}\n";
	JsonFile << "}\n";
	JsonFile.close();	      
     }
  }  
}


