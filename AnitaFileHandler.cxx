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

#define MAX_FILE_SIZE 100000
char unzippedBuffer[MAX_FILE_SIZE];

using namespace std;


AnitaFileHandler::AnitaFileHandler(std::string rawDir,std::string awareDir)
  :fAwareDir(awareDir),fRawDir(rawDir)
{

}
    
void AnitaFileHandler::processFile(ZippedFile_t *zfPtr,int run)
{
  cout << "Got ZippedFile_t " << zfPtr->filename << "\t" 
       << zfPtr->segmentNumber << endl;
  

  //  ofstream DebugFile2("/tmp/log_log_log2",std::ofstream::app);
  //  cout << "Got ZippedFile_t " << zfPtr->filename << "\t" << zfPtr->segmentNumber << endl;
  //  DebugFile2.close();


  char segmentName[FILENAME_MAX];
  char outputFilename[FILENAME_MAX];
  char awareFilename[FILENAME_MAX];
  //  sprintf(outputFilename,"%s/ANITA3/log/run%d/archive",fAwareDir.c_str(),run);
  //  gSystem->mkdir(outputFilename,kTRUE);
  sprintf(outputFilename,"%s/ANITA3/aux/run%d/archive",fAwareDir.c_str(),run);
  gSystem->mkdir(outputFilename,kTRUE);
  sprintf(outputFilename,"%s/run%d/config",fRawDir.c_str(),run);
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
  
  //  ofstream DebugFile("/tmp/log_log_log",std::ofstream::app);
  //  DebugFile << "Got " << outputFilename << "\t" << linkName << "\t" << zfPtr->segmentNumber << "\t" << numOutputBytes << "\n";


  FILE *fpOut = fopen(outputFilename,"w");
  retVal=fwrite(unzippedBuffer,numOutputBytes,1,fpOut);
  //  DebugFile << "retVal: " << retVal << "\n";
  fclose(fpOut);
  //    retVal=normalSingleWrite((unsigned char*)unzippedBuffer,outputFilename,numOutputBytes);

  
  char tempLine[1024];
  
  if(zfPtr->segmentNumber==0)  {
    //Okay this is a bit silly but, it should probably work
    getOutputName(outputFilename,linkName,zfPtr,0,run);
    //    DebugFile << "outputFilename: " << outputFilename << "\n";
    ofstream OutFile(outputFilename);
    for(int segment=0;segment<100;segment++) {
      zfPtr->segmentNumber=segment;
      getOutputName(segmentName,linkName,zfPtr,1,run);
      ifstream InFile(segmentName);
      if(!InFile) break;
      //      DebugFile << "Processing: " << segmentName << endl;

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
      
    if(getAwareName(awareFilename,zfPtr)) {
      struct stat buf;  
      int retVal2=stat(awareFilename,&buf);  
      if(retVal2==0) {    
	if(buf.st_mtime<ut.modtime) {
	  unlink(awareFilename);
	  link(outputFilename,awareFilename);
	  utime(awareFilename,&ut);
	}
      }
      else {
	link(outputFilename,awareFilename);
	utime(awareFilename,&ut);
      }
    }
  }
  //    DebugFile.close();
}

int AnitaFileHandler::getAwareName(char *awareName,ZippedFile_t *zfPtr) {
  if(strstr(zfPtr->filename,"config")) {
    //Have a config file
    sprintf(awareName,"%s/ANITA3/config/%s",fAwareDir.c_str(),zfPtr->filename);
    return 1;
  }
  return 0;   
}


void AnitaFileHandler::getOutputName(char *outputFilename,char *linkName,ZippedFile_t *zfPtr,int useSegment,int fRun)
{
  
  //Now output the file
  if(useSegment) {
    if(strstr(zfPtr->filename,"config")) {
      //Have a config file
      sprintf(outputFilename,"%s/run%d/config/%s.%u_%u",fRawDir.c_str(),fRun,zfPtr->filename,zfPtr->unixTime,zfPtr->segmentNumber);
      sprintf(linkName,"%s/run%d/config/%s",fRawDir.c_str(),fRun,zfPtr->filename);
    }
    else {
      //Other file
      sprintf(outputFilename,"%s/ANITA3/aux/run%d/archive/%s.%u_%u",
	      fAwareDir.c_str(),fRun,zfPtr->filename,
	      zfPtr->unixTime,zfPtr->segmentNumber);
      sprintf(linkName,"%s/ANITA3/aux/%s",fAwareDir.c_str(),zfPtr->filename);
    }
  }
  else {
    if(strstr(zfPtr->filename,"config")) {
      //Have a config file
	  sprintf(outputFilename,"%s/run%d/config/%s.%u",fRawDir.c_str(),fRun,zfPtr->filename,zfPtr->unixTime);
	  sprintf(linkName,"%s/run%d/config/%s",fRawDir.c_str(),fRun,zfPtr->filename);
	}
	else {
	  //Other file
	  sprintf(outputFilename,"%s/ANITA3/aux/run%d/archive/%s.%u",
		  fAwareDir.c_str(),fRun,zfPtr->filename,
		  zfPtr->unixTime);
	  sprintf(linkName,"%s/ANITA3/aux/%s",fAwareDir.c_str(),zfPtr->filename);
	}
  }
}


