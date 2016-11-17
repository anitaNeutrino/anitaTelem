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

AnitaCmdEchoHandler::AnitaCmdEchoHandler(std::string awareDir)
  :fAwareDir(awareDir)
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
  FileStat_t staty;
  char fileName[FILENAME_MAX];
  char cmdCountFile[FILENAME_MAX];
  std::map<UInt_t,CommandEcho_t>::iterator it;
  const char *fromString[2]={"Ground","Payload"};
  
  for(int payloadFlag=0;payloadFlag<2;payloadFlag++) {
     std::ofstream JsonFile;
     
     int cmdCount=0;

     sprintf(cmdCountFile,"%s/ANITA4/db/cmdCount%s.dat",fAwareDir.c_str(),fromString[payloadFlag]);
     std::ifstream CmdCountFile(cmdCountFile);
     if(CmdCountFile) {
       CmdCountFile >> cmdCount;
       CmdCountFile.close();
     }


     if(fCmdEchoMap[payloadFlag].size()>0) {

      
	sprintf(fileName,"%s/ANITA4/cmdEcho/%s",fAwareDir.c_str(),fromString[payloadFlag]);
	gSystem->mkdir(fileName,kTRUE);
 
		
	for(it=fCmdEchoMap[payloadFlag].begin();it!=fCmdEchoMap[payloadFlag].end();it++) {
	   CommandEcho_t *echoPtr=&(it->second);	
	   CommandEcho echo(-1,echoPtr->unixTime,echoPtr);
	  
	   while(1) {
	     sprintf(fileName,"%s/ANITA4/cmdEcho/%s/cmdEcho%d.json",fAwareDir.c_str(),fromString[payloadFlag],cmdCount);
	     if(gSystem->GetPathInfo(fileName,staty)) {
	       //File doesn't exist
	       break;
	     }
	     cmdCount++;
	   }
	   JsonFile.open(fileName);  


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
	   JsonFile.close();	      
	}


	std::ofstream CmdCountFile(cmdCountFile);
	if(CmdCountFile) {
	  CmdCountFile << cmdCount << "\n";
	  CmdCountFile.close();
	}
     }
  }
}


