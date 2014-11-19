#include <iostream>
#include <fstream>
#include <map>
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
#include "AnitaSurfHkHandler.h" 
#include "AnitaTurfRateHandler.h"
#include "AnitaAuxiliaryHandler.h"
#include "AnitaCmdEchoHandler.h" 
#include "AnitaFileHandler.h" 
#include "AnitaGenericHeaderHandler.h" 

// #include "configLib/configLib.h"
// #include "kvpLib/keyValuePair.h"
// #include "AnitaGenericHeaderHandler.h"
// #include "AnitaSlowRateHandler.h"

#include "TStopwatch.h"
#include "TTimeStamp.h" 
#include "TSystem.h" 


//Functions
void handleScience(unsigned char *buffer,unsigned short numBytes);
int processHighRateTDRSSFile(char *filename);
int processLOSFile(char *filename);
int processOpenportFile(char *filename);
int guessCode(SSHkDataStruct_t *hkPtr);
int getLastRunNumber(AnitaTelemFileType::AnitaTelemFileType_t telemType) ;
int getLastFileNumber(AnitaTelemFileType::AnitaTelemFileType_t telemType) ;
int getLastNumBytesNumber(AnitaTelemFileType::AnitaTelemFileType_t telemType);
void updateLastFileNumber();
void updateLastRunNumber();
void updateLastNumBytesNumber();
void loadRunNumberMap();
void saveRunNumberMap();
UInt_t getRunNumberFromTime(UInt_t unixTime);
UInt_t getRunNumberFromEvent(UInt_t eventNumber);

void addRunToMap(UInt_t run, UInt_t eventNumber, UInt_t unixTime);
int needToSaveRunMap=0;


unsigned short *bigBuffer;


AnitaHeaderHandler *headHandler;
AnitaMonitorHandler *monHandler;
AnitaHkHandler *hkHandler;
AnitaSurfHkHandler *surfhkHandler;
AnitaTurfRateHandler *turfRateHandler;
AnitaGpsHandler *gpsHandler;
AnitaAuxiliaryHandler *auxHandler;
AnitaCmdEchoHandler *cmdHandler;
AnitaFileHandler *fileHandler;
AnitaGenericHeaderHandler *ghdHandler;

char *awareOutputDir;

const char *telemTypeForFile[3]={"Los","Tdrss","Openport"};

int lastNumBytesNumber[3]={0};
int lastRunNumber[3]={0};
int lastFileNumber[3]={0};

std::map<UInt_t,UInt_t> fTimeRunMap;
std::map<UInt_t,UInt_t> fEventRunMap;
std::map<UInt_t,UInt_t> fRunToEventMap;






#define MIN_LOS_SIZE 900000
#define MIN_TDRSS_SIZE 40000
#define BIG_BUF_SIZE 10000000

int main (int argc, char ** argv)
{
  bigBuffer = (unsigned short*) malloc(BIG_BUF_SIZE);
  if(argc<1) {
    std::cerr << "Usage: " << argv[0] << "  <telem file> <telem file>\n";
    return -1;
  }
  std::cout << "sizeof(unsigned long): " << sizeof(unsigned long) << "\n";
  //`  return -1;
  //Create the handlers
  
  char *anitaTelemDataDir=getenv("ANITA_TELEM_DATA_DIR");
  if(!anitaTelemDataDir) {
    anitaTelemDataDir="/anitaStorage/antarctica14/telem";
  }
  
  std::string rawDir(anitaTelemDataDir);
  rawDir+="/raw";
  std::cout << rawDir << "\n";

  awareOutputDir=getenv("AWARE_OUTPUT_DIR");
  if(!awareOutputDir) {
    std::cout << "AWARE_OUTPUT_DIR not set using default\n";
    awareOutputDir=(char*)"/anitaStorage/antarctica14/telem/aware/output";
  }
 
  for(int i=AnitaTelemFileType::kAnitaTelemLos;i<=AnitaTelemFileType::kAnitaTelemOpenport;i++) {
    lastRunNumber[i]=getLastRunNumber((AnitaTelemFileType::AnitaTelemFileType_t)i);
    lastFileNumber[i]=getLastFileNumber((AnitaTelemFileType::AnitaTelemFileType_t)i);
    std::cout << telemTypeForFile[i] << "\t" << lastRunNumber[i] << "\t" << lastFileNumber[i] << "\n";
  }
  loadRunNumberMap();
  

  headHandler = new AnitaHeaderHandler(rawDir,awareOutputDir,0);
  hkHandler = new AnitaHkHandler(rawDir);
  gpsHandler = new AnitaGpsHandler(rawDir);
  monHandler = new AnitaMonitorHandler(rawDir);
  surfhkHandler = new AnitaSurfHkHandler(rawDir);
  turfRateHandler = new AnitaTurfRateHandler(rawDir);
  auxHandler = new AnitaAuxiliaryHandler(rawDir);

  ghdHandler = new AnitaGenericHeaderHandler(awareOutputDir);
  cmdHandler = new AnitaCmdEchoHandler(awareOutputDir);
  fileHandler = new AnitaFileHandler(awareOutputDir);
  
  //Lazy hack way of determing which type of file we are dealing with
  for(int i=1;i<argc;i++) {
    if(strstr(argv[i],"tdrss")!=NULL) {
      std::cout << "Got TDRSS:\t" << strstr(argv[i],"tdrss") << "\n";
      processHighRateTDRSSFile(argv[i]);

    }
    else if(strstr(argv[i],"los")!=NULL) {
      std::cout << "Got LOS:\t" << strstr(argv[i],"los") << "\n";
      processLOSFile(argv[i]);
    }
    else if(strstr(argv[i],"openport")!=NULL) {
      std::cout << "Got Openport:\t" << strstr(argv[i],"openport") << "\n";
      processOpenportFile(argv[i]);
    }
  }

  free(bigBuffer);

  ghdHandler->writeFileSummary();
  headHandler->loopMap();
  headHandler->loopEventMap();
  hkHandler->loopMap();
  hkHandler->loopSSMap();
  gpsHandler->loopG12PosMap();
  gpsHandler->loopG12SatMap();
  gpsHandler->loopAdu5PatMaps();
  gpsHandler->loopAdu5SatMaps();
  gpsHandler->loopAdu5VtgMaps();
  gpsHandler->loopGpsGgaMaps();
  monHandler->loopMap();
  monHandler->loopOtherMap();
  surfhkHandler->loopMap();
  surfhkHandler->loopAvgMap();
  turfRateHandler->loopMap();
  turfRateHandler->loopSumMap();
  auxHandler->loopAcqdStartMap();
  auxHandler->loopGpsdStartMap();
  auxHandler->loopLogWatchdStartMap();
  cmdHandler->loopMaps();

  updateLastRunNumber();
  updateLastFileNumber();
  updateLastNumBytesNumber();
  if(needToSaveRunMap) saveRunNumberMap();
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
    unsigned long *ulPtr;
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
    //    ghdHandler->newFile(AnitaTelemFileType::kAnitaTelemTdrss,currentTdrssRun,currentTdrssFile,buf.st_mtime);

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
	if(count3>10000) break;
	if(bigBuffer[count]==0xf00d) {
	    count3=0;
	    //	    printf("Got f00d\n");
	    //Maybe new tdrss buffer
	    startHdr=bigBuffer[count];
	    auxHdr=bigBuffer[count+1];
	    idHdr=bigBuffer[count+2];
	    //	    printf("startHdr -- %x (count %d %x)\n",startHdr,count,count);
	    //	    printf("auxHdr -- %x\n",auxHdr);
	    //	    printf("idHdr -- %x\n",idHdr);
	    if(startHdr==0xf00d && auxHdr==0xd0cc && (idHdr&0xfff0)==0xae00) {
		//Got a tdrss buffer
		losOrSip=idHdr&0x1;
		oddOrEven=idHdr&0x2>>1;
		ulPtr = (unsigned long*) &bigBuffer[count+3];
		bufferCount=*ulPtr;
		numSciBytes=bigBuffer[count+2+5];
		//		printf("Buffer %u -- %d bytes\n",bufferCount,numSciBytes);		
		handleScience((unsigned char*)&bigBuffer[count+2+6],numSciBytes);
		checksum=bigBuffer[count+2+6+(numSciBytes/2)];		
		swEndHdr=bigBuffer[count+2+7+(numSciBytes/2)];
		endHdr=bigBuffer[count+2+8+(numSciBytes/2)];
		auxHdr2=bigBuffer[count+2+9+(numSciBytes/2)];
		//		printf("swEndHdr -- %x  (count %d %x)\n",swEndHdr,count+2+7+(numSciBytes/2),count+2+7+(numSciBytes/2));
		//		printf("endHdr -- %x\n",endHdr);
		//		printf("auxHdr2 -- %x\n",auxHdr2);		
//		return 0;
		count+=2+10+(numSciBytes/2);
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
    static UInt_t latestHeaderEventNumber=0;
    static UInt_t lastHeaderEventNumber=0;
    static UInt_t lastHeaderRunNumber=0;
    int run=0;
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


// 	printf("Got %s (%#x) -- (%d bytes)\n",
// 	       packetCodeAsString(gHdr->code),
// 	       gHdr->code,gHdr->numBytes);
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
	SSHkDataStruct_t *sshkPtr=0;
	EncodedPedSubbedChannelPacketHeader_t *pack1=0;
	EncodedPedSubbedSurfPacketHeader_t *pack2=0;
	EncodedSurfPacketHeader_t *pack3=0;
	RawWaveformPacket_t *pack4=0;
	RawSurfPacket_t *rsp=0;
	RunStart_t *runStartPtr=0;
	LogWatchdStart_t *logWatchStart=0;
	OtherMonitorStruct_t *otherMonPtr=0;
	if((checkVal==0) && testGHdr>0) {	  
	  if(testGHdr->code>0) ghdHandler->addGenericHeader(testGHdr);
	    //	    printf("Got %s (%#x) -- (%d bytes)\n",
	    //		   packetCodeAsString(testGHdr->code),
	    //		   testGHdr->code,testGHdr->numBytes);
	    switch(testGHdr->code&BASE_PACKET_MASK) {
	    case PACKET_HD:

	      hdPtr= (AnitaEventHeader_t*)testGHdr;
	      run=getRunNumberFromTime(hdPtr->unixTime);
	      //	      cout << "Got Header\t" << run << "\t" << hdPtr->eventNumber << "\n";
	      headHandler->addHeader(hdPtr,run);
	      lastHeaderRunNumber=run;
	      lastHeaderEventNumber=hdPtr->eventNumber;
	      if(hdPtr->eventNumber>latestHeaderEventNumber)
		latestHeaderEventNumber=hdPtr->eventNumber;
	      break;
	    case PACKET_SURF_HK:
	      //	      cout << "Got SurfHk\n";
	      surfPtr = (FullSurfHkStruct_t*) testGHdr;
	      //	      if((time_t)surfPtr->unixTime<time_t(nowTime+1000))
	      surfhkHandler->addSurfHk(surfPtr,getRunNumberFromTime(surfPtr->unixTime));
	      break;
	    case PACKET_AVG_SURF_HK:
	      //	      cout << "Got AveragedSurfHkStruct_t\n";
	      surfhkHandler->addAveragedSurfHk((AveragedSurfHkStruct_t*) testGHdr,getRunNumberFromTime(((AveragedSurfHkStruct_t*) testGHdr)->unixTime));
	      break;
	      
	    case PACKET_TURF_RATE:
	      //	      cout << "Got TurfRate\n";
	      turfRateHandler->addTurfRate((TurfRateStruct_t*)testGHdr,getRunNumberFromTime(((TurfRateStruct_t*) testGHdr)->unixTime));
	      break;
	    case PACKET_SUM_TURF_RATE:
	      //	      cout << "Got SummedTurfRateStruct_t\n";
	      turfRateHandler->addSumTurfRate((SummedTurfRateStruct_t*)testGHdr,getRunNumberFromTime(((SummedTurfRateStruct_t*) testGHdr)->unixTime));
	      break;
	    case PACKET_MONITOR:
	      //	      cout << "Got MonitorStruct_t\n";
	      monHandler->addMonitor((MonitorStruct_t*)testGHdr,getRunNumberFromTime(((MonitorStruct_t*) testGHdr)->unixTime));
	      break;
	    case PACKET_OTHER_MONITOR:
	      //	      cout << "Got OtherMonitorStruct_t\n";
	      otherMonPtr=(OtherMonitorStruct_t*)testGHdr;
	      monHandler->addOtherMonitor(otherMonPtr,otherMonPtr->runNumber);
	      addRunToMap(otherMonPtr->runNumber,otherMonPtr->runStartEventNumber,otherMonPtr->runStartTime);
	      break;
	    case PACKET_GPS_G12_SAT:
	      //	      cout << "Got GpsG12SatStruct_t\n";
	      gpsHandler->addG12Sat((GpsG12SatStruct_t*) testGHdr,getRunNumberFromTime(((GpsG12SatStruct_t*) testGHdr)->unixTime));
	      break;
	    case PACKET_GPS_G12_POS:
	      //	      cout << "Got GpsG12PosStruct_t\n";
	      gpsHandler->addG12Pos((GpsG12PosStruct_t*) testGHdr,getRunNumberFromTime(((GpsG12PosStruct_t*) testGHdr)->unixTime));
	      break;
	    case PACKET_GPS_ADU5_SAT:
	      //	      cout << "Got GpsAdu5SatStruct_t\n";
	      gpsHandler->addAdu5Sat((GpsAdu5SatStruct_t*) testGHdr,getRunNumberFromTime(((GpsAdu5SatStruct_t*) testGHdr)->unixTime));
	      break;
	    case PACKET_GPS_ADU5_PAT:
	      //	      cout << "Got GpsAdu5PatStruct_t\n";

	      gpsHandler->addAdu5Pat((GpsAdu5PatStruct_t*) testGHdr,getRunNumberFromTime(((GpsAdu5PatStruct_t*) testGHdr)->unixTime));
	      break;
	    case PACKET_GPS_ADU5_VTG:
	      //	      cout << "Got GpsAdu5VtgStruct_t\n";
	      gpsHandler->addAdu5Vtg((GpsAdu5VtgStruct_t*) testGHdr,getRunNumberFromTime(((GpsAdu5VtgStruct_t*) testGHdr)->unixTime));
	      break;
	    case PACKET_GPS_GGA:
	      //	      cout << "Got GpsGgaStruct_t\n";
	      gpsHandler->addGpsGga((GpsGgaStruct_t*) testGHdr,getRunNumberFromTime(((GpsGgaStruct_t*) testGHdr)->unixTime));

	      break;
	    case PACKET_ZIPPED_FILE:
	      cout << "Got ZippedFile_t\n";
	      ///		    printf("Boo\n");
	      fileHandler->processFile((ZippedFile_t*) testGHdr,getRunNumberFromTime(((ZippedFile_t*) testGHdr)->unixTime));
	      break;
	    case PACKET_CMD_ECHO:
	      cout << "Got CommandEcho_t\n";
	      cmdHandler->addCmdEcho((CommandEcho_t*) testGHdr);
	      break;
		    
	    case PACKET_SURF:
	      //	      cout << "Got RawSurfPacket_t\n";
	      rsp=(RawSurfPacket_t*)testGHdr;	  
	      headHandler->addRawSurfPacket((RawSurfPacket_t*)testGHdr,getRunNumberFromEvent(rsp->eventNumber));
	      break;
	    case PACKET_WV:
	      //	      cout << "Got RawWaveformPacket_t\n";
	      pack4=(RawWaveformPacket_t*)testGHdr;
	      headHandler->addRawWavePacket((RawWaveformPacket_t*)testGHdr,getRunNumberFromEvent(pack4->eventNumber));
	      break;
	    case PACKET_ENC_SURF:
	      //	      cout << "Got EncodedSurfPacketHeader_t\n";
	      pack3=(EncodedSurfPacketHeader_t*)testGHdr;
	      headHandler->addEncSurfPacket(pack3,getRunNumberFromEvent(pack3->eventNumber));
	      break;
	    case PACKET_ENC_SURF_PEDSUB:
	      //	      cout << "Got EncodedPedSubbedSurfPacketHeader_t\n";
	      pack2=(EncodedPedSubbedSurfPacketHeader_t*)testGHdr;
	      run=getRunNumberFromEvent(pack2->eventNumber);
	      if(pack2->eventNumber==lastHeaderEventNumber) run=lastHeaderRunNumber;
	      headHandler->addEncPedSubbedSurfPacket(pack2,run);
	      break;
	    case PACKET_ENC_WV_PEDSUB:
	      //	      cout << "Got EncodedPedSubbeWavePacketHeader_t\n";
	      pack1=(EncodedPedSubbedChannelPacketHeader_t*)testGHdr;
	      headHandler->addEncPedSubbedWavePacket(pack1,getRunNumberFromEvent(pack1->eventNumber));
	      break;
	      
	    case PACKET_HKD:	      
	      hkPtr = (HkDataStruct_t*)testGHdr;
	      //	      cout << "Got HkDataStruct_t " << hkPtr->ip320.code << "\t" << IP320_RAW << "\n";
	      if(hkPtr->ip320.code==IP320_RAW)
		hkHandler->addHk(hkPtr,getRunNumberFromTime(hkPtr->unixTime));
	      //	      else if(hkPtr->ip320.code==IP320_CAL)
	      //		hkHandler->addCalHk(hkPtr);
	      //	      else if(hkPtr->ip320.code==IP320_AVZ)
	      //		hkHandler->addAvzHk(hkPtr);
	      break;	
	    case PACKET_HKD_SS:	      
	      sshkPtr= (SSHkDataStruct_t*)testGHdr;
	      
	      //	      cout << "Got SSHkDataStruct_t " <<guessCode(sshkPtr) << "\t" << IP320_RAW << "\n";
	      if(sshkPtr->ip320.code==0) {
		sshkPtr->ip320.code=(AnalogueCode_t)guessCode(sshkPtr);
	      }
	      if(sshkPtr->ip320.code==IP320_RAW) 
		hkHandler->addSSHk(sshkPtr,getRunNumberFromTime(sshkPtr->unixTime));
	      break;	 

	    case PACKET_ACQD_START:
	      //	      cout << "Got Acqd Start Packet\n";
	      auxHandler->addAcqdStart((AcqdStartStruct_t*)testGHdr,getRunNumberFromTime(((AcqdStartStruct_t*) testGHdr)->unixTime));
	      break;
	    case PACKET_GPSD_START:
	      //	      cout << "Got GPSd Start Packet\n";
	      auxHandler->addGpsdStart((GpsdStartStruct_t*)testGHdr,getRunNumberFromTime(((GpsdStartStruct_t*) testGHdr)->unixTime));
	      break;
	    case PACKET_LOGWATCHD_START:
	      //	      cout << "Got Log Watchd Start Packet\n";
	      logWatchStart=(LogWatchdStart_t*)testGHdr;
	      auxHandler->addLogWatchdStart(logWatchStart,logWatchStart->runNumber);
	      
	      addRunToMap(logWatchStart->runNumber,latestHeaderEventNumber,logWatchStart->unixTime);
	      break;
	    case PACKET_RUN_START:
	      cout << "Got Run Start Packet\n";
	      runStartPtr = (RunStart_t*)testGHdr;
	      addRunToMap(runStartPtr->runNumber,runStartPtr->eventNumber,runStartPtr->unixTime);
	      break;
	      
	    default: 
	      {
		fprintf(stderr,"Got packet without a handler (code: %x -- %s)\n",
			testGHdr->code,packetCodeAsString(testGHdr->code));
		break;
	      }
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

void updateLastRunNumber() {
  for(int telemType=AnitaTelemFileType::kAnitaTelemLos;telemType<=AnitaTelemFileType::kAnitaTelemOpenport;telemType++) {
    std::cout << "updateRunNumber\t" << telemTypeForFile[telemType] << "\t" << lastRunNumber[telemType] << "\n";
    
    char fileName[FILENAME_MAX];
    sprintf(fileName,"%s/ANITA3/db/last%sRun",awareOutputDir,telemTypeForFile[telemType]);
    std::ofstream RunFile(fileName);
    if(RunFile) {
      RunFile << lastRunNumber[telemType] << "\n";
      RunFile.close();
    } 
  }
}

void updateLastFileNumber() {
  for(int telemType=AnitaTelemFileType::kAnitaTelemLos;telemType<=AnitaTelemFileType::kAnitaTelemOpenport;telemType++) {
    std::cout << "updateLastFileNumber\t" << telemTypeForFile[telemType] << "\t" << lastFileNumber[telemType] << "\n";
    char fileName[FILENAME_MAX];
    sprintf(fileName,"%s/ANITA3/db/last%sFile",awareOutputDir,telemTypeForFile[telemType]);
    std::ofstream FileFile(fileName);
    if(FileFile) {
      FileFile << lastFileNumber[telemType] << "\n";
      FileFile.close();
    } 
  }
}


void updateLastNumBytesNumber() {
  for(int telemType=AnitaTelemFileType::kAnitaTelemLos;telemType<=AnitaTelemFileType::kAnitaTelemOpenport;telemType++) {
    std::cout << "updateLastNumBytesNumber\t" << telemTypeForFile[telemType] << "\t" <<  lastNumBytesNumber[telemType] << "\n";
    char fileName[FILENAME_MAX];
    sprintf(fileName,"%s/ANITA3/db/last%sNumBytes",awareOutputDir,telemTypeForFile[telemType]);
    std::ofstream NumBytesNumBytes(fileName);
    if(NumBytesNumBytes) {
      NumBytesNumBytes << lastNumBytesNumber[telemType] << "\n";
      NumBytesNumBytes.close();
    } 
  }
}

int getLastRunNumber(AnitaTelemFileType::AnitaTelemFileType_t telemType)  {
  char fileName[FILENAME_MAX];
  sprintf(fileName,"%s/ANITA3/db/last%sRun",awareOutputDir,telemTypeForFile[telemType]);
  std::ifstream RunFile(fileName);
  if(RunFile) {
    RunFile >> lastRunNumber[telemType];
    RunFile.close();
    return lastRunNumber[telemType];
  }
  return -1;
}

int getLastNumBytesNumber(AnitaTelemFileType::AnitaTelemFileType_t telemType)  {
  char fileName[FILENAME_MAX];
  sprintf(fileName,"%s/ANITA3/db/last%sNumBytes",awareOutputDir,telemTypeForFile[telemType]);
  std::ifstream RunFile(fileName);
  if(RunFile) {
    RunFile >> lastNumBytesNumber[telemType];
    RunFile.close();
    return lastNumBytesNumber[telemType];
  }
  return -1;
}

int getLastFileNumber(AnitaTelemFileType::AnitaTelemFileType_t telemType)  {
  char fileName[FILENAME_MAX];
  sprintf(fileName,"%s/ANITA3/db/last%sFile",awareOutputDir,telemTypeForFile[telemType]);
  std::cout << fileName << "\n";
  std::ifstream LastFile(fileName);
  if(LastFile) {
    LastFile >> lastFileNumber[telemType];
    LastFile.close();
    return lastFileNumber[telemType];
  }
  return -1;
}


int processLOSFile(char *filename) {
  static int lastNumBytes=0;
  //  static unsigned int lastUnixTime=0;
  lastNumBytes=0;
  int thisFileNumber=0;
  int thisRunNumber=0;

  const char *losFileString = gSystem->BaseName(filename);
  const char *losDirString = gSystem->BaseName(gSystem->DirName(filename));

  sscanf(losFileString,"%06d",&thisFileNumber);
  sscanf(losDirString,"%06d",&thisRunNumber);
  cout << "processLOSFile: " << filename << "\t" << losFileString << "\t" << losDirString << "\t" << thisFileNumber <<  "\t" << thisRunNumber << "\t" << lastFileNumber[AnitaTelemFileType::kAnitaTelemLos] << "\t" << lastRunNumber[AnitaTelemFileType::kAnitaTelemLos] << endl;

  int newRun=-1;
  if(thisRunNumber>lastRunNumber[AnitaTelemFileType::kAnitaTelemLos])
    newRun=1;
  else if(thisRunNumber==lastRunNumber[AnitaTelemFileType::kAnitaTelemLos]) {
    if(thisFileNumber==lastFileNumber[AnitaTelemFileType::kAnitaTelemLos]) {
      newRun=0;
      lastNumBytes=getLastNumBytesNumber(AnitaTelemFileType::kAnitaTelemLos);
    }
    else if(thisFileNumber>lastFileNumber[AnitaTelemFileType::kAnitaTelemLos]) {
      newRun=1;
      lastNumBytes=0;
    }
  }
  
  if(newRun<0) return 0;

  int numBytes=0,count=0;
  FILE *losFile;
  
    //data stuff
  unsigned short numWords;
  //  unsigned short unused;
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
  
  printf("numBytes %d, lastNumBytes %d\n",numBytes,lastNumBytes);
  if(numBytes==lastNumBytes) {
    //No new data
    fclose(losFile);
    return 2;
  }
  count=lastNumBytes;
  lastNumBytes=numBytes;
  lastNumBytesNumber[AnitaTelemFileType::kAnitaTelemLos]=numBytes;
  cout << "losFile: " << filename << endl;
  struct stat buf;
  //    int retVal2=
  
  
    
  stat(filename,&buf);
  ghdHandler->newFile(AnitaTelemFileType::kAnitaTelemLos,thisRunNumber,thisFileNumber,buf.st_mtime);
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


  lastRunNumber[AnitaTelemFileType::kAnitaTelemLos]=thisRunNumber;
  lastFileNumber[AnitaTelemFileType::kAnitaTelemLos]=thisFileNumber;

  return 0;

}

int processOpenportFile(char *filename) {
  static int lastNumBytes=0;
  //  static unsigned int lastUnixTime=0;
  lastNumBytes=0;
  int thisFileNumber=0;
  int thisRunNumber=0;

  const char *openportFileString = gSystem->BaseName(filename);
  const char *openportDirString = gSystem->BaseName(gSystem->DirName(filename));

  sscanf(openportFileString,"%06d",&thisFileNumber);
  sscanf(openportDirString,"%06d",&thisRunNumber);
  cout << "processOpenportFile: " << filename << "\t" << openportFileString << "\t" << openportDirString << "\t" << thisFileNumber <<  "\t" << thisRunNumber << "\t" << lastFileNumber[AnitaTelemFileType::kAnitaTelemOpenport] << "\t" << lastRunNumber[AnitaTelemFileType::kAnitaTelemOpenport] << endl;

  int newRun=-1;
  if(thisRunNumber>lastRunNumber[AnitaTelemFileType::kAnitaTelemOpenport])
    newRun=1;
  else if(thisRunNumber==lastRunNumber[AnitaTelemFileType::kAnitaTelemOpenport]) {
    if(thisFileNumber==lastFileNumber[AnitaTelemFileType::kAnitaTelemOpenport]) {
      newRun=0;
      lastNumBytes=getLastNumBytesNumber(AnitaTelemFileType::kAnitaTelemOpenport);
    }
    else if(thisFileNumber>lastFileNumber[AnitaTelemFileType::kAnitaTelemOpenport]) {
      newRun=1;
      lastNumBytes=0;
    }
  }
  
  if(newRun<0) return 0;

  int numBytes=0,count=0;
  FILE *openportFile;
  
    //data stuff
  unsigned short numWords;
  //  unsigned short unused;
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

  openportFile=fopen(filename,"rb");
  if(!openportFile) {
    //	printf("Couldn't open: %s\n",filename);
    return -1;
  }
  
  
  numBytes=fread(bigBuffer,1,BIG_BUF_SIZE,openportFile);
  if(numBytes<=0) {
    fclose(openportFile);
    return -1;
  }
  
  printf("numBytes %d, lastNumBytes %d\n",numBytes,lastNumBytes);
  if(numBytes==lastNumBytes) {
    //No new data
    fclose(openportFile);
    return 2;
  }
  count=lastNumBytes;
  lastNumBytes=numBytes;
  lastNumBytesNumber[AnitaTelemFileType::kAnitaTelemOpenport]=numBytes;
  cout << "openportFile: " << filename << endl;
  struct stat buf;
  //    int retVal2=
  
  
    
  stat(filename,&buf);

  ghdHandler->newFile(AnitaTelemFileType::kAnitaTelemOpenport,thisRunNumber,thisFileNumber,buf.st_mtime);
  printf("Read %d bytes from %s\n",numBytes,filename);
  fclose(openportFile);
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
	    //Maybe new openport buffer

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
		//Got a openport buffer
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


  lastRunNumber[AnitaTelemFileType::kAnitaTelemOpenport]=thisRunNumber;
  lastFileNumber[AnitaTelemFileType::kAnitaTelemOpenport]=thisFileNumber;

  return 0;

}


int guessCode(SSHkDataStruct_t *hkPtr) {
  float mean=0;
  for(int i=0;i<CHANS_PER_IP320;i++) {
    //    std::cout << i << "\t" <<  << "\n";
    mean+=((hkPtr->ip320.board.data[i])>>4);
  }
  mean/=CHANS_PER_IP320;
  //  std::cout << "Mean: " << mean << "\n";
  if(mean>4000) return IP320_CAL;
  if(mean<2050) return IP320_AVZ;
  return IP320_RAW;
}


void loadRunNumberMap()
{
  UInt_t runNumber;
  UInt_t unixTime;
  UInt_t eventNumber;
  char fileName[FILENAME_MAX];
  sprintf(fileName,"%s/ANITA3/db/runNumberMap",awareOutputDir);
  std::ifstream RunFile(fileName);
  if(RunFile) { 
    while(RunFile >> runNumber >> unixTime >> eventNumber) {
      //      std::cout << runNumber << "\t" << unixTime << "\t" << eventNumber << "\n";
      fTimeRunMap.insert(std::pair<UInt_t,UInt_t>(unixTime,runNumber));      
      fEventRunMap.insert(std::pair<UInt_t,UInt_t>(eventNumber,runNumber));      
      fRunToEventMap.insert(std::pair<UInt_t,UInt_t>(runNumber,eventNumber));      

    }
  }
}

void saveRunNumberMap()
{
  char fileName[FILENAME_MAX];
  sprintf(fileName,"%s/ANITA3/db/runNumberMap",awareOutputDir);
  std::ofstream RunFile(fileName);

  if(RunFile) {
    std::map<UInt_t,UInt_t>::iterator it;
    std::map<UInt_t,UInt_t>::iterator itEvent;
    UInt_t eventNumber=200000000;
    for(it=fTimeRunMap.begin();it!=fTimeRunMap.end();it++) {
      itEvent=fRunToEventMap.find(it->second);
      if(itEvent!=fRunToEventMap.end())
	eventNumber=itEvent->second;
      RunFile << it->second << "\t" << it->first << "\t" << eventNumber << "\n";
    }
    RunFile.close();
  }
}

UInt_t getRunNumberFromTime(UInt_t unixTime)
{
  std::map<UInt_t,UInt_t>::iterator it=fTimeRunMap.lower_bound(unixTime);
  if(it!=fTimeRunMap.end()) {
    //    std::cout << it->first << "\t" << it->second << "\t" << unixTime << "\n";
    return it->second;
  }
  if(fTimeRunMap.size()>0) {
    it=fTimeRunMap.end();
    it--;  
    return it->second;
  }
  std::cout << "Couldn't get run number from unixTime " << unixTime << "\n" ;
  return -1;
}

UInt_t getRunNumberFromEvent(UInt_t eventNumber)
{
  // std:cout << "getRunNumberFromEvent\t" << eventNumber << "\n";
  std::map<UInt_t,UInt_t>::iterator it=fEventRunMap.lower_bound(eventNumber);
  
  //  std::map<UInt_t,UInt_t>::iterator it3;
  //  for(it3=fEventRunMap.begin();it3!=fEventRunMap.end();it3++) {
  //    std::cout << it3->first << "\t" << it3->second << "\t" << eventNumber << "\n";
  //  }
  if(it!=fEventRunMap.end()) {

    //    std::cout << "it: " << it->first << "\t" << it->second << "\t" << eventNumber << "\n";
    while(it->first>eventNumber && it!=fEventRunMap.begin()) {
      it--;
    }
    //    std::cout << it->first << "\t" << it->second << "\t" << eventNumber << "\n"; 
    
    return it->second;
  }
  if(fEventRunMap.size()>0) {
    it=fEventRunMap.end();
    it--;
    return it->second;
  }
  std::cout << "Couldn't get run number from eventNumber " << eventNumber << "\n" ;
  return -1;
}


void addRunToMap(UInt_t run, UInt_t eventNumber, UInt_t unixTime) 
{
  std::cout << run << "\t" << eventNumber << "\t" << unixTime << "\n";

  std::map<UInt_t,UInt_t>::iterator it=fRunToEventMap.find(run);
  if(it==fRunToEventMap.end()) {
    std::cout << "Adding new run to run map\n";
    needToSaveRunMap=1;
    //Don't have this run
    fTimeRunMap.insert(std::pair<UInt_t,UInt_t>(unixTime,run));      
    fEventRunMap.insert(std::pair<UInt_t,UInt_t>(eventNumber,run));      
    fRunToEventMap.insert(std::pair<UInt_t,UInt_t>(run,eventNumber)); 
  }
  else {
    //Maybe do something clever
    std::cout << "Already have this run in the map\n";
    std::cout << it->second << "\n";
    if(it->second > eventNumber) it->second=eventNumber;    
  }
}
