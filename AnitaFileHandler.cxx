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


AnitaFileHandler::AnitaFileHandler(std::string awareDir)
:fAwareDir(awareDir)
{

}
    
void AnitaFileHandler::processFile(ZippedFile_t *zfPtr,int run)
{
  //  cout << "Got ZippedFile_t " << zfPtr->filename << "\t" 
  //       << zfPtr->segmentNumber << endl;
  
  char segmentName[FILENAME_MAX];
  char outputFilename[FILENAME_MAX];
  sprintf(outputFilename,"%s/ANITA3/log/run%d",fAwareDir.c_str(),run);
  gSystem->mkdir(outputFilename,kTRUE);
  sprintf(outputFilename,"%s/ANITA3/aux/run%d",fAwareDir.c_str(),run);
  gSystem->mkdir(outputFilename,kTRUE);
  sprintf(outputFilename,"%s/ANITA3/config/run%d",fAwareDir.c_str(),run);
  gSystem->mkdir(outputFilename,kTRUE);

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
  
  getOutputName(outputFilename,linkName,zfPtr,1,run);
  
  FILE *fpOut = fopen(outputFilename,"w");
  retVal=fwrite(unzippedBuffer,numOutputBytes,1,fpOut);
  fclose(fpOut);
  //    retVal=normalSingleWrite((unsigned char*)unzippedBuffer,outputFilename,numOutputBytes);
  
  char tempLine[1024];
  
  if(zfPtr->segmentNumber==0) {
    //Okay this is a bit silly but, it should probably work
    getOutputName(outputFilename,linkName,zfPtr,0,run);
    ofstream OutFile(outputFilename);
    for(int segment=0;segment<100;segment++) {
      zfPtr->segmentNumber=segment;
      getOutputName(segmentName,linkName,zfPtr,1,run);
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

void AnitaFileHandler::getOutputName(char *outputFilename,char *linkName,ZippedFile_t *zfPtr,int useSegment,int fRun)
{
  
  //Now output the file
  if(useSegment) {
    if(strstr(zfPtr->filename,"config")) {
      //Have a config file
      sprintf(outputFilename,"%s/ANITA3/config/run%d/archive/%s.%u_%u",fAwareDir.c_str(),fRun,zfPtr->filename,zfPtr->unixTime,zfPtr->segmentNumber);
      sprintf(linkName,"%s/ANITA3/run%d/config/%s",fAwareDir.c_str(),fRun,zfPtr->filename);
    }
    else if(strstr(zfPtr->filename,"messages")) {
	  //Have /var/log/messages
      sprintf(outputFilename,"%s/ANITA3/log/run%d/archive/messages.%u_%u",fAwareDir.c_str(),fRun,
		  zfPtr->unixTime,zfPtr->segmentNumber);
      sprintf(linkName,"%s/ANITA3/log/run%d/messages",fAwareDir.c_str(),fRun);
	}
    else if(strstr(zfPtr->filename,"anita")) {
      //Have /var/log/messages
      sprintf(outputFilename,"%s/ANITA3//log/run%darchive/anita.log.%u_%u",fAwareDir.c_str(),fRun,
		  zfPtr->unixTime,zfPtr->segmentNumber);
      sprintf(linkName,"%s/ANITA3/log/run%d/anita.log",fAwareDir.c_str(),fRun);
    }
    else {
      //Other file
      sprintf(outputFilename,"%s/ANITA3/aux/run%darchive/%s.%u_%u",
	      fAwareDir.c_str(),fRun,zfPtr->filename,
	      zfPtr->unixTime,zfPtr->segmentNumber);
      sprintf(linkName,"%s/ANITA3/aux/run%d/%s",fAwareDir.c_str(),fRun,zfPtr->filename);
    }
  }
  else {
    if(strstr(zfPtr->filename,"config")) {
      //Have a config file
	  sprintf(outputFilename,"%s/ANITA3/config/run%d/archive/%s.%u",fAwareDir.c_str(),fRun,zfPtr->filename,zfPtr->unixTime);
	  sprintf(linkName,"%s/ANITA3/config/run%d/%s",fAwareDir.c_str(),fRun,zfPtr->filename);
	}
	else if(strstr(zfPtr->filename,"messages")) {
	  //Have /var/log/messages
	  sprintf(outputFilename,"%s/ANITA3/log/run%d/archive/messages.%u",fAwareDir.c_str(),fRun,
		  zfPtr->unixTime);
	  sprintf(linkName,"%s/ANITA3/log/run%d/messages",fAwareDir.c_str(),fRun);
	}
	else if(strstr(zfPtr->filename,"anita")) {
	  //Have /var/log/messages
	  sprintf(outputFilename,"%s/ANITA3/log/run%d/archive/anita.log.%u",fAwareDir.c_str(),fRun,
		  zfPtr->unixTime);
	  sprintf(linkName,"%s/ANITA3/log/run%d/anita.log",fAwareDir.c_str(),fRun);
	}
	else {
	  //Other file
	  sprintf(outputFilename,"%s/ANITA3/aux/run%d/archive/%s.%u",
		  fAwareDir.c_str(),fRun,zfPtr->filename,
		  zfPtr->unixTime);
	  sprintf(linkName,"%s/ANITA3/aux/run%d/%s",fAwareDir.c_str(),fRun,zfPtr->filename);
	}
  }
}


