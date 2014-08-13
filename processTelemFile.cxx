#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>  
#include <sys/stat.h>
using namespace std;


//Event Reader Includes
#include "simpleStructs.h"
#include "AnitaPacketUtil.h"

//Web plotter includes
#include "AnitaHeaderHandler.h"
#include "AnitaHkHandler.h"
#include "AnitaGpsHandler.h" 
#include "AnitaMonitorHandler.h" 

// #include "configLib/configLib.h"
// #include "kvpLib/keyValuePair.h"
// #include "AnitaAuxiliaryHandler.h"
// #include "AnitaSurfHkHandler.h" 
// #include "AnitaTurfRateHandler.h"
// #include "AnitaHkHandler.h" 
// #include "AnitaFileHandler.h" 
// #include "AnitaCmdEchoHandler.h" 
// #include "AnitaGenericHeaderHandler.h"
// #include "AnitaSlowRateHandler.h"

#include "TStopwatch.h"
#include "TTimeStamp.h" 
#include "TSystem.h" 


//Functions
void handleScience(unsigned char *buffer,unsigned short numBytes);
int processHighRateTDRSSFile(char *filename);
int processLOSFile(char *filename);

unsigned short *bigBuffer;


AnitaHeaderHandler *headHandler;
AnitaMonitorHandler *monHandler;
AnitaHkHandler *hkHandler;
AnitaGpsHandler *gpsHandler;

int currentRun=0;

#define MIN_LOS_SIZE 900000
#define MIN_TDRSS_SIZE 40000
#define BIG_BUF_SIZE 10000000

int main (int argc, char ** argv)
{
  bigBuffer = (unsigned short*) malloc(BIG_BUF_SIZE);
  if(argc<3) {
    std::cerr << "Usage: " << argv[0] << " <run> <telem file> <telem file>\n";
    return -1;
  }
  
  std::cout << "sizeof(unsigned long): " << sizeof(unsigned long) << "\n";
  currentRun=atoi(argv[1]);
  //`  return -1;
  //Create the handlers
  headHandler = new AnitaHeaderHandler(currentRun);
  hkHandler = new AnitaHkHandler(currentRun);
  gpsHandler = new AnitaGpsHandler(currentRun);
  monHandler = new AnitaMonitorHandler(currentRun);


  for(int i=2;i<argc;i++) 
    processLOSFile(argv[i]);
  free(bigBuffer);

  headHandler->loopMap();
  hkHandler->loopMap();
  gpsHandler->loopG12PosMap();
  gpsHandler->loopG12SatMap();
  monHandler->loopMap();
  monHandler->loopOtherMap();
}


int processHighRateTDRSSFile(char *filename) {

    int numBytes=0,count=0;
    FILE *tdrssFile;

    //data stuff
    //    unsigned short numWords;
    //    unsigned short unused;
    unsigned short startHdr;
    unsigned short auxHdr;
    unsigned short idHdr;
    int losOrSip;
    int oddOrEven;
    unsigned int bufferCount;
    unsigned int *ulPtr;
    unsigned short numSciBytes;
    unsigned short checksum;
    unsigned short endHdr;
    unsigned short swEndHdr;
    unsigned short auxHdr2;

    tdrssFile=fopen(filename,"rb");
    if(!tdrssFile ) {
//	printf("Couldn't open: %s\n",filename);
	return -1;
    }


    struct stat buf;
    //    int retVal2=
    stat(filename,&buf);
    //    ghdHandler->newTdrssFile(currentTdrssRun,currentTdrssFile,buf.st_mtime);

    static int triedThisOne=0;
    numBytes=fread(bigBuffer,1,BIG_BUF_SIZE,tdrssFile);
    if(numBytes<MIN_TDRSS_SIZE && triedThisOne<50) {
	sleep(1);
	fclose(tdrssFile);
	triedThisOne++;
	return -1;
    }
    triedThisOne=0;
    printf("Read %d bytes from %s\n",numBytes,filename);
    fclose(tdrssFile);
    int count3=0;
    while(count<numBytes) {
//	printf("%x\n",bigBuffer[count]);
//	printf("%d of %d\n",count,numBytes);
	count3++;
	if(count3>1000) break;
	if(bigBuffer[count]==0xf00d) {
	    count3=0;
	    //	    printf("Got f00d\n");
	    //Maybe new tdrss buffer
	    startHdr=bigBuffer[count];
	    auxHdr=bigBuffer[count+1];
	    idHdr=bigBuffer[count+2];
//	    printf("startHdr -- %x\n",startHdr);
//	    printf("auxHdr -- %x\n",auxHdr);
//	    printf("idHdr -- %x\n",idHdr);
	    if(startHdr==0xf00d && auxHdr==0xd0cc && (idHdr&0xfff0)==0xae00) {
		//Got a tdrss buffer
		losOrSip=idHdr&0x1;
		oddOrEven=idHdr&0x2>>1;
		ulPtr = (unsigned int*) &bigBuffer[count+3];
		bufferCount=*ulPtr;
		numSciBytes=bigBuffer[count+5];
//		printf("Buffer %u -- %d bytes\n",bufferCount,numSciBytes);		
		handleScience((unsigned char*)&bigBuffer[count+6],numSciBytes);
		checksum=bigBuffer[count+6+(numSciBytes/2)];		
		swEndHdr=bigBuffer[count+7+(numSciBytes/2)];
		endHdr=bigBuffer[count+8+(numSciBytes/2)];
		auxHdr2=bigBuffer[count+9+(numSciBytes/2)];
//		printf("swEndHdr -- %x\n",swEndHdr);
//		printf("endHdr -- %x\n",endHdr);
//		printf("auxHdr2 -- %x\n",auxHdr2);		
//		return 0;
		count+=10+(numSciBytes/2);
		continue;
	    }
	}
	count++;
    }
 


    return 0;

}



void handleScience(unsigned char *buffer,unsigned short numBytes) {

//    cout << (int) headHandler << endl;
    unsigned long count=0;
    GenericHeader_t *gHdr;
    int checkVal;
    char packetBuffer[10000];
    time_t nowTime;
    time(&nowTime);
    unsigned long lastCount=count-1;
    while(count<(unsigned long)(numBytes-1)) {
      if(count==lastCount)
	count++;
      lastCount=count;
// 	if(buffer[count]==0xfe && buffer[count+1]==0xfe && buffer[numBytes-1]==0xfe) {	    
// 	    printf("Got 0xfe Wake Up Buffer\n");
// 	    return;
// 	}
//	printf("count %d\n",count);
	checkVal=checkPacket(&buffer[count]);
	gHdr = (GenericHeader_t*) &buffer[count];		

	if(checkVal==8 && gHdr->code==PACKET_HKD && gHdr->numBytes==sizeof(SSHkDataStruct_t)) {
	  printf("Got SSHkDataStruct_t??\n");
	  gHdr->code=PACKET_HKD_SS;
	  gHdr->verId=VER_HK_SS;
	  checkVal=0;
	  count+=sizeof(HkDataStruct_t);
	  continue;
	}


	// printf("Got %s (%#x) -- (%d bytes)\n",
	//        packetCodeAsString(gHdr->code),
	//        gHdr->code,gHdr->numBytes);
	GenericHeader_t *testGHdr=gHdr;


	if(gHdr->code==PACKET_ZIPPED_PACKET) {
	  
	  int retVal=unzipZippedPacket((ZippedPacket_t*)&buffer[count],
				       packetBuffer,10000);
	  checkVal=checkPacket(packetBuffer);
	  if(retVal==0 && checkVal==0) {
	    testGHdr=(GenericHeader_t*) packetBuffer;
	    //		    printf("\tGot %s (%#x) -- (%d bytes)\n",
	    //			   packetCodeAsString(gHdr2->code),
	    //			   gHdr2->code,gHdr2->numBytes);
	  }
	  else {
	    testGHdr=0;
	    printf("\tunzipZippedPacket retVal %d -- checkPacket == %d\n",retVal,checkVal);
	  }
	}

	HkDataStruct_t *hkPtr=0;
	AnitaEventHeader_t *hdPtr=0;
	FullSurfHkStruct_t *surfPtr=0;
	if((checkVal==0) && testGHdr>0) {	  
	  //	  if(testGHdr->code>0) ghdHandler->addHeader(testGHdr);
	    //	    printf("Got %s (%#x) -- (%d bytes)\n",
	    //		   packetCodeAsString(testGHdr->code),
	    //		   testGHdr->code,testGHdr->numBytes);
	    switch(testGHdr->code&BASE_PACKET_MASK) {
	    case PACKET_HD:
	      //	      cout << "Got Header\n";
	      hdPtr= (AnitaEventHeader_t*)testGHdr;
	      headHandler->addHeader(hdPtr);
	      break;
	    case PACKET_SURF_HK:
	      //	      cout << "Got SurfHk\n";
	      surfPtr = (FullSurfHkStruct_t*) testGHdr;
	      //	      if((time_t)surfPtr->unixTime<time_t(nowTime+1000))
	      //		surfhkHandler->addSurfHk(surfPtr);
	      break;
	    case PACKET_AVG_SURF_HK:
	      //	      cout << "Got AveragedSurfHkStruct_t\n";
	      //	      surfhkHandler->addAveragedSurfHk((AveragedSurfHkStruct_t*) testGHdr);
	      break;
	      
	    case PACKET_TURF_RATE:
	      //	      cout << "Got TurfRate\n";
	      //	      turfRateHandler->addTurfRate((TurfRateStruct_t*)testGHdr);
	      break;
	    case PACKET_SUM_TURF_RATE:
	      //	      cout << "Got SummedTurfRateStruct_t\n";
	      //	      turfRateHandler->addSumTurfRate((SummedTurfRateStruct_t*)testGHdr);
	      break;
	    case PACKET_MONITOR:
	      //	      cout << "Got MonitorStruct_t\n";
	      monHandler->addMonitor((MonitorStruct_t*)testGHdr);
	      break;
	    case PACKET_OTHER_MONITOR:
	      //	      cout << "Got OtherMonitorStruct_t\n";
	      monHandler->addOtherMonitor((OtherMonitorStruct_t*)testGHdr);
	      break;
	    case PACKET_GPS_G12_SAT:
	      //	      cout << "Got GpsG12SatStruct_t\n";
	      gpsHandler->addG12Sat((GpsG12SatStruct_t*) testGHdr);
	      break;
	    case PACKET_GPS_G12_POS:
	      //	      cout << "Got GpsG12PosStruct_t\n";
	      gpsHandler->addG12Pos((GpsG12PosStruct_t*) testGHdr);
	      break;
	    case PACKET_GPS_ADU5_SAT:
	      //	      cout << "Got GpsAdu5SatStruct_t\n";
	      //	      gpsHandler->addAdu5Sat((GpsAdu5SatStruct_t*) testGHdr);
	      break;
	    case PACKET_GPS_ADU5_PAT:
	      //	      cout << "Got GpsAdu5PatStruct_t\n";
	      //	      gpsHandler->addAdu5Pat((GpsAdu5PatStruct_t*) testGHdr);
	      break;
	    case PACKET_GPS_ADU5_VTG:
	      //	      cout << "Got GpsAdu5VtgStruct_t\n";
	      //	      gpsHandler->addAdu5Vtg((GpsAdu5VtgStruct_t*) testGHdr);
	      break;
	    case PACKET_GPS_GGA:
	      //	      cout << "Got GpsGgaStruct_t\n";
	      //	      gpsHandler->addGpsGga((GpsGgaStruct_t*) testGHdr);
	      break;
	    case PACKET_ZIPPED_FILE:
	      //	      cout << "Got ZippedFile_t\n";
	      ///		    printf("Boo\n");
	      //	      fileHandler->processFile((ZippedFile_t*) testGHdr);
	      break;
	    case PACKET_CMD_ECHO:
	      //	      cout << "Got CommandEcho_t\n";
	      //	      cmdHandler->addCmdEcho((CommandEcho_t*) testGHdr);
	      break;
		    
	    case PACKET_SURF:
	      //	      cout << "Got RawSurfPacket_t\n";
	      //	      if(makeEventPlots) headHandler->addRawSurfPacket((RawSurfPacket_t*)testGHdr);
	      break;
	    case PACKET_WV:
	      //	      cout << "Got RawWaveformPacket_t\n";
	      //	      if(makeEventPlots) headHandler->addRawWavePacket((RawWaveformPacket_t*)testGHdr);
	      break;
	    case PACKET_ENC_SURF:
	      //	      cout << "Got EncodedSurfPacketHeader_t\n";
	      //	      if(makeEventPlots) 
	      //	      headHandler->addEncSurfPacket((EncodedSurfPacketHeader_t*)testGHdr);
	      break;
	    case PACKET_ENC_SURF_PEDSUB:
	      //	      	      cout << "Got EncodedPedSubbedSurfPacketHeader_t\n";
	      //	      if(makeEventPlots) 
	      //		headHandler->addEncPedSubbedSurfPacket((EncodedPedSubbedSurfPacketHeader_t*)testGHdr);
	      break;
	    case PACKET_ENC_WV_PEDSUB:
	      //	      cout << "Got EncodedPedSubbeWavePacketHeader_t\n";
	      //if(makeEventPlots) headHandler->addEncPedSubbedWavePacket((EncodedPedSubbedChannelPacketHeader_t*)testGHdr);
	      break;
	      
	    case PACKET_HKD:	      
	      hkPtr = (HkDataStruct_t*)testGHdr;
	      cout << "Got HkDataStruct_t " << hkPtr->ip320.code << "\t" << IP320_RAW << "\n";
	      if(hkPtr->ip320.code==IP320_RAW)
		hkHandler->addHk(hkPtr);
	      //	      else if(hkPtr->ip320.code==IP320_CAL)
	      //		hkHandler->addCalHk(hkPtr);
	      //	      else if(hkPtr->ip320.code==IP320_AVZ)
	      //		hkHandler->addAvzHk(hkPtr);
	      break;	
	    case PACKET_HKD_SS:	      
	      break;	 

	    case PACKET_ACQD_START:
	      //	      cout << "Got Acqd Start Packet\n";
	      //	      auxHandler->addAcqdStart((AcqdStartStruct_t*)testGHdr);
	      break;
	    case PACKET_GPSD_START:
	      //	      cout << "Got GPSd Start Packet\n";
	      //	      auxHandler->addGpsdStart((GpsdStartStruct_t*)testGHdr);
	      break;
	    case PACKET_LOGWATCHD_START:
	      //	      cout << "Got Log Watchd Start Packet\n";
	      //	      auxHandler->addLogWatchdStart((LogWatchdStart_t*)testGHdr);
	      break;
	      
	      //	    defualt:
	      fprintf(stderr,"Got packet without a handler (code: %x -- %s)\n",
		      testGHdr->code,packetCodeAsString(testGHdr->code));
	      break;
	    }

	    
	    if(gHdr->numBytes>0) 
	      count+=gHdr->numBytes;
	    else {
	      //Got broken packet will bail
	      printf("Problem with buffer -- bailing\n");
	      return;
	    }
	      
	}
	else {
	    printf("Problem with packet -- checkVal==%d  (%s code? %#x)\n",
		   checkVal,packetCodeAsString(gHdr->code),gHdr->code);
	    //	    return;
	    if(gHdr->numBytes>0) {
	      count+=gHdr->numBytes;
	    }
	    else {
	      //Got broken packet will bail
	      printf("Problem with buffer -- bailing\n");
	      return;
	    }
	}
    }
       
}


int processLOSFile(char *filename) {
  static int lastNumBytes=0;
  //  static unsigned int lastUnixTime=0;
  lastNumBytes=0;

  //  cout << "processLOSFile: " << filename << "\t" << lastLosFile << "\t" << currentLosFile << endl;

  int numBytes=0,count=0;
  FILE *losFile;
  
    //data stuff
  unsigned short numWords;
  unsigned short unused;
  unsigned short foodHdr;
  unsigned short doccHdr;
  unsigned short ae00Hdr;
  int losOrSip;
  int oddOrEven;
  unsigned long bufferCount;
  unsigned long *ulPtr;
  unsigned short numSciBytes;
  unsigned short checksum;
  unsigned short endHdr;
  unsigned short swEndHdr;
  unsigned short auxHdr2;

    losFile=fopen(filename,"rb");
    if(!losFile) {
//	printf("Couldn't open: %s\n",filename);
	return -1;
    }

    
    numBytes=fread(bigBuffer,1,BIG_BUF_SIZE,losFile);
    if(numBytes<=0) {
      fclose(losFile);
      return -1;
    }

    //    printf("numBytes %d, lastNumBytes %d\n",numBytes,lastNumBytes);
    if(numBytes==lastNumBytes) {
      //No new data
      fclose(losFile);
      return 2;
    }
    count=lastNumBytes;
    lastNumBytes=numBytes;
    cout << "losFile: " << filename << endl;
    struct stat buf;
    //    int retVal2=


    
    stat(filename,&buf);
    //    ghdHandler->newLosFile(currentLosRun,currentLosFile,buf.st_mtime);
    printf("Read %d bytes from %s\n",numBytes,filename);
    fclose(losFile);
    int count3=0;

    //    for(int i=0;i<100;i++)
      //      printf("%d\t%#x\n",i,bigBuffer[i]);
    while(count<numBytes) {
      //      printf("%d -- %x\n",count,bigBuffer[count]);
      //      printf("%d of %d\n",count,numBytes);
	count3++;
	//	if(count3>100) break;
	if(bigBuffer[count]==0xf00d) {
	    count3=0;
	    //	    printf("Got f00d %d\n",count);
	    //Maybe new los buffer

	    foodHdr=bigBuffer[count];
	    doccHdr=bigBuffer[count+1];
	    ae00Hdr=bigBuffer[count+2];
	    numWords=bigBuffer[count+3];

	    // printf("numWords -- %d -- %x\n",numWords,numWords);
	    // printf("foodHdr -- %x\n",foodHdr);
	    // printf("doccHdr -- %x\n",doccHdr);
	    // printf("ae00Hdr -- %x\n",ae00Hdr);

	    //	    exit(0);
	    if(foodHdr==0xf00d && doccHdr==0xd0cc && (ae00Hdr&0xfff0)==0xae00) {
		//Got a los buffer
		losOrSip=ae00Hdr&0x1;
		oddOrEven=ae00Hdr&0x2>>1;
		ulPtr = (unsigned long*) &bigBuffer[count+3];
		bufferCount=*ulPtr;
		numSciBytes=bigBuffer[count+2+5];
		//		printf("Buffer %u -- %d bytes\n",bufferCount,numSciBytes);
		checksum=bigBuffer[count+2+6+(numSciBytes/2)];		
		swEndHdr=bigBuffer[count+2+7+(numSciBytes/2)];
		endHdr=bigBuffer[count+2+8+(numSciBytes/2)];
		auxHdr2=bigBuffer[count+2+9+(numSciBytes/2)];

		//Now do something with buffer
		handleScience((unsigned char*)&bigBuffer[count+2+6],numSciBytes);
		// printf("swEndHdr -- %x\n",swEndHdr);
		// printf("endHdr -- %x\n",endHdr);
		// printf("auxHdr2 -- %x\n",auxHdr2);	
		
		//		exit(0);
//		return 0;
		count+=12+(numSciBytes/2);
		continue;
	    }
	}
	count++;
    }

    return 0;

}
