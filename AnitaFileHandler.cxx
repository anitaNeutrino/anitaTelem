////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to unzip and write telemetered files           /////////
//////                                                             /////////
////// rjn@hep.ucl.ac.uk --- October 2006                          /////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
#include <utime.h>   
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "AnitaFileHandler.h"
#include "AnitaPacketUtil.h"

#include "TSystem.h"

#define MAX_FILE_SIZE 10000
char unzippedBuffer[MAX_FILE_SIZE];

using namespace std;


AnitaFileHandler::AnitaFileHandler(std::string rawDir,int run)
:fRawDir(rawDir),fRun(run)
{
  char fileName[FILENAME_MAX];
  sprintf(fileName,"%s/run%d/config/archive",fRawDir.c_str(),fRun);
  sprintf(fileName,"%s/run%d/log/archive",fRawDir.c_str(),fRun);
  sprintf(fileName,"%s/run%d/aux/archive",fRawDir.c_str(),fRun);
  gSystem->mkdir(fileName,kTRUE);

}
    
void AnitaFileHandler::processFile(ZippedFile_t *zfPtr)
{
  //  cout << "Got ZippedFile_t " << zfPtr->filename << "\t" 
  //       << zfPtr->segmentNumber << endl;
  
  char segmentName[FILENAME_MAX];
  char outputFilename[FILENAME_MAX];
  char linkName[FILENAME_MAX];
  char *zippedBuf=(char*) zfPtr;
  int numInputBytes=zfPtr->gHdr.numBytes-sizeof(ZippedFile_t);
  unsigned int numOutputBytes=MAX_FILE_SIZE;
  int retVal=unzipBuffer(&zippedBuf[sizeof(ZippedFile_t)],unzippedBuffer,numInputBytes,
			 &numOutputBytes);
  if(retVal<0) {
    fprintf(stderr,"Error processing zipped file\n");
    return;
  }
  if(numOutputBytes!=zfPtr->numUncompressedBytes) {
    fprintf(stderr,"Expected %d bytes but only got %d bytes\n",
	    zfPtr->numUncompressedBytes,numOutputBytes);
  }
  
  getOutputName(outputFilename,linkName,zfPtr,1);
  
  FILE *fpOut = fopen(outputFilename,"w");
  retVal=fwrite(unzippedBuffer,numOutputBytes,1,fpOut);
  fclose(fpOut);
  //    retVal=normalSingleWrite((unsigned char*)unzippedBuffer,outputFilename,numOutputBytes);
  
  char tempLine[1024];
  
  if(zfPtr->segmentNumber==0) {
    //Okay this is a bit silly but, it should probably work
    getOutputName(outputFilename,linkName,zfPtr,0);
    ofstream OutFile(outputFilename);
    for(int segment=0;segment<100;segment++) {
      zfPtr->segmentNumber=segment;
	getOutputName(segmentName,linkName,zfPtr,1);
	ifstream InFile(segmentName);
	if(!InFile) break;
	//	cout << "Processing: " << segmentName << endl;
	while(InFile.getline(tempLine,1024))
	  OutFile << tempLine << "\n";
	unlink(segmentName);
      }	       
      OutFile.close();
      
      unlink(linkName);
      link(outputFilename,linkName);
      
      
      //Change modification times
      struct utimbuf ut;
      ut.actime=zfPtr->unixTime;
      ut.modtime=zfPtr->unixTime;
      retVal=utime(outputFilename,&ut);
      retVal=utime(linkName,&ut);
    }
}

void AnitaFileHandler::getOutputName(char *outputFilename,char *linkName,ZippedFile_t *zfPtr,int useSegment)
{
  
  //Now output the file
  if(useSegment) {
    if(strstr(zfPtr->filename,"config")) {
      //Have a config file
      sprintf(outputFilename,"%s/run%d/config/archive/%s.%u_%u",fRawDir.c_str(),fRun,zfPtr->filename,zfPtr->unixTime,zfPtr->segmentNumber);
      sprintf(linkName,"%s/run%d/config/%s",fRawDir.c_str(),fRun,zfPtr->filename);
    }
    else if(strstr(zfPtr->filename,"messages")) {
	  //Have /var/log/messages
      sprintf(outputFilename,"%s/run%d/log/archive/messages.%u_%u",fRawDir.c_str(),fRun,
		  zfPtr->unixTime,zfPtr->segmentNumber);
      sprintf(linkName,"%s/run%d/log/messages",fRawDir.c_str(),fRun);
	}
    else if(strstr(zfPtr->filename,"anita")) {
      //Have /var/log/messages
      sprintf(outputFilename,"%s/run%d/log/archive/anita.log.%u_%u",fRawDir.c_str(),fRun,
		  zfPtr->unixTime,zfPtr->segmentNumber);
      sprintf(linkName,"%s/run%d/log/anita.log",fRawDir.c_str(),fRun);
    }
    else {
      //Other file
      sprintf(outputFilename,"%s/run%d/aux/archive/%s.%u_%u",
	      fRawDir.c_str(),fRun,zfPtr->filename,
	      zfPtr->unixTime,zfPtr->segmentNumber);
      sprintf(linkName,"%s/run%d/aux/%s",fRawDir.c_str(),fRun,zfPtr->filename);
    }
  }
  else {
    if(strstr(zfPtr->filename,"config")) {
      //Have a config file
	  sprintf(outputFilename,"%s/run%d/config/archive/%s.%u",fRawDir.c_str(),fRun,zfPtr->filename,zfPtr->unixTime);
	  sprintf(linkName,"%s/run%d/config/%s",fRawDir.c_str(),fRun,zfPtr->filename);
	}
	else if(strstr(zfPtr->filename,"messages")) {
	  //Have /var/log/messages
	  sprintf(outputFilename,"%s/run%d/log/archive/messages.%u",fRawDir.c_str(),fRun,
		  zfPtr->unixTime);
	  sprintf(linkName,"%s/run%d/log/messages",fRawDir.c_str(),fRun);
	}
	else if(strstr(zfPtr->filename,"anita")) {
	  //Have /var/log/messages
	  sprintf(outputFilename,"%s/run%d/log/archive/anita.log.%u",fRawDir.c_str(),fRun,
		  zfPtr->unixTime);
	  sprintf(linkName,"%s/run%d/log/anita.log",fRawDir.c_str(),fRun);
	}
	else {
	  //Other file
	  sprintf(outputFilename,"%s/run%d/aux/archive/%s.%u",
		  fRawDir.c_str(),fRun,zfPtr->filename,
		  zfPtr->unixTime);
	  sprintf(linkName,"%s/run%d/aux/%s",fRawDir.c_str(),fRun,zfPtr->filename);
	}
  }
}


