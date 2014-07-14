#include <iostream>
#include <fstream>
#include <unistd.h>
#include <signal.h>  
#include <sys/stat.h>
using namespace std;


//Event Reader Includes
#include "simpleStructs.h"
#include "AnitaPacketUtil.h"

//Web plotter includes
#include "configLib/configLib.h"
#include "kvpLib/keyValuePair.h"
#include "AnitaAuxiliaryHandler.h"
#include "AnitaHeaderHandler.h"
#include "AnitaSurfHkHandler.h" 
#include "AnitaTurfRateHandler.h"
#include "AnitaMonitorHandler.h" 
#include "AnitaHkHandler.h" 
#include "AnitaGpsHandler.h" 
#include "AnitaFileHandler.h" 
#include "AnitaCmdEchoHandler.h" 
#include "AnitaGenericHeaderHandler.h"
#include "AnitaSlowRateHandler.h"

#include "TStopwatch.h"
#include "TTimeStamp.h" 
#include "TSystem.h" 
 

//Functions
int readConfigFile();
void handleScience(unsigned char *buffer,unsigned short numBytes);
void printSurfInfo(EncodedSurfPacketHeader_t *surfPtr);
void printPedSubSurfInfo(EncodedPedSubbedSurfPacketHeader_t *surfPtr);
int processLOSFile(char *filename);
int processHighRateTDRSSFile(char *filename);
int processIridiumFile(char *filename);
int processSlowTdrssFile(char *filename);
void sigHandler(int sig); 
void sigSegvHandler(int sig); 
int checkLOS();
int checkHighRateTDRSS();
int checkIridium();
int checkSlowTdrss();

//Global Variables
int losStartRun=10;
int losStartFile=1;
int currentLosRun=1;
int currentLosFile=1;
int tdrssStartRun=10;
int tdrssStartFile=1;
int iridiumStartFile=1;
int slowTdrssStartFile=1;
int currentTdrssRun=1;
int currentTdrssFile=1;
int currentIridiumFile=1;
int currentSlowTdrssFile=1;
int saveEvery=100;
int backupFileEvery=3600;
int makePacketPlots=1;
int makeHeaderPlots=1;
int makeSurfHkPlots=1;
int makeTurfRatePlots=1;
int makeMonitorPlots=1;
int makeHkPlots=1;
int makeGpsPlots=1;
int makeEventPlots=1;
int makeAuxPlots=1;
int progState=1;
int checkForNewLOS=1;
int checkForNewTDRSS=1;
char losDir[FILENAME_MAX];
char highRateDir[FILENAME_MAX];
char iridiumDir[FILENAME_MAX];
char slowTdrssDir[FILENAME_MAX];
char dataDir[FILENAME_MAX];
char plotDir[FILENAME_MAX];
unsigned short *bigBuffer;
AnitaHeaderHandler *headHandler;
AnitaAuxiliaryHandler *auxHandler;
AnitaSurfHkHandler *surfhkHandler;
AnitaTurfRateHandler *turfRateHandler;
AnitaMonitorHandler *monHandler;
AnitaHkHandler *hkHandler;
AnitaGpsHandler *gpsHandler;
AnitaFileHandler *fileHandler;
AnitaCmdEchoHandler *cmdHandler;
AnitaGenericHeaderHandler *ghdHandler;
AnitaSlowRateHandler *slowHandler;


//Stuff For Error Tree
int telemType; //0 is LOS, 1 is TDRSS (or vice versa)
int telemRun;
int telemFile;
unsigned int telemFileTime;
int telemBuffer;
int telemSciBytes;
int telemChecksumReport;
int telemChecksumCalc;
int telemErrFlag;
int sciencePacketCode;
int scienceNumBytes;
int scienceCheckVal;


#define MIN_LOS_SIZE 900000
#define MIN_TDRSS_SIZE 40000

#define BIG_BUF_SIZE 10000000

int main (int argc, char ** argv)
{
    int losFileLastPlot=0;
    int losRunLastPlot=0;
    int tdrssFileLastPlot=0;
    int tdrssRunLastPlot=0;
    int retVal=0;
    int losRetVal=0;
    char backupName[FILENAME_MAX];
    char backupDir[FILENAME_MAX];
    int backupCounter=0;
    TTimeStamp lastBackup;
    bigBuffer = (unsigned short*) malloc(BIG_BUF_SIZE);
    readConfigFile();


 //    unsigned int numbers[10]={0,1,2,3,4,5,6,7,8,9};
//     printf("simpleIntCrc: %u\n",simpleIntCrc(numbers,10));
//     //    exit(0);
//     AnitaEventHeader_t theHeader;
//     memset(&theHeader,0,sizeof(AnitaEventHeader_t));
//     theHeader.eventNumber=10;
//     fillGenericHeader(&theHeader,PACKET_HD,sizeof(AnitaEventHeader_t));
//     printf("theHeader.gHdr.checksum:\t%u\n",theHeader.gHdr.checksum);
//     printf("checkPacket: %d\n",checkPacket(&theHeader));
//     exit(0);

    currentLosRun=losStartRun;
    currentLosFile=losStartFile;
    currentTdrssRun=tdrssStartRun;
    currentTdrssFile=tdrssStartFile;
    currentIridiumFile=iridiumStartFile;
    currentSlowTdrssFile=slowTdrssStartFile;

    //    exit(0);
    headHandler = new AnitaHeaderHandler(plotDir,dataDir,0);
    surfhkHandler = new AnitaSurfHkHandler(plotDir,dataDir);
    turfRateHandler = new AnitaTurfRateHandler(plotDir,dataDir);
    monHandler = new AnitaMonitorHandler(plotDir,dataDir);
    hkHandler = new AnitaHkHandler(plotDir,dataDir);
    gpsHandler = new AnitaGpsHandler(plotDir,dataDir);
    fileHandler = new AnitaFileHandler(plotDir);
    cmdHandler = new AnitaCmdEchoHandler(plotDir,dataDir);
    ghdHandler = new AnitaGenericHeaderHandler(plotDir,dataDir);
    slowHandler = new AnitaSlowRateHandler(plotDir,dataDir);
    auxHandler = new AnitaAuxiliaryHandler(plotDir,dataDir);
    

    signal(SIGTERM, sigHandler);
    signal(SIGINT, sigHandler);
    signal(SIGSEGV,sigSegvHandler);
    signal(SIGABRT,sigSegvHandler);

     TStopwatch stopy;
    stopy.Start();
    if(makeHkPlots) hkHandler->makePlots();
    stopy.Stop();
    cout << "hkHandler->makePlots: " << stopy.CpuTime() << " "
	 << stopy.RealTime() << endl;    
    stopy.Start();
    if(makePacketPlots) ghdHandler->makePlots();
    stopy.Stop();
    cout << "ghdHandler->makePlots: " << stopy.CpuTime() << " "
	 << stopy.RealTime() << endl;
    stopy.Start();
    if(makeHeaderPlots) headHandler->makePlots();
    stopy.Stop();
    cout << "headHandler->makePlots: " << stopy.CpuTime() << " "
	 << stopy.RealTime() << endl;
    stopy.Start();
    if(makeSurfHkPlots) surfhkHandler->makePlots();
    stopy.Stop();
    cout << "surfhkHandler->makePlots: " << stopy.CpuTime() << " "
	 << stopy.RealTime() << endl;
    stopy.Start();
    if(makeTurfRatePlots) turfRateHandler->makePlots();
    stopy.Stop();
    cout << "turfRateHandler->makePlots: " << stopy.CpuTime() << " "
	 << stopy.RealTime() << endl;
    stopy.Start();
    if(makeMonitorPlots) monHandler->makePlots();
    stopy.Stop();
    cout << "monHandler->makePlots: " << stopy.CpuTime() << " "
	 << stopy.RealTime() << endl;

    stopy.Start();
    if(makeGpsPlots) gpsHandler->makePlots();
    stopy.Stop();
    cout << "gpsHandler->makePlots: " << stopy.CpuTime() << " "
	 << stopy.RealTime() << endl;
    stopy.Start();
    if(makeAuxPlots) auxHandler->makePlots();
    stopy.Stop();
    cout << "auxHandler->makePlots: " << stopy.CpuTime() << " "
	 << stopy.RealTime() << endl;    

   

    cout << "Using LOS directory: " << losDir <<  endl;
    cout << "Using High Rate directory: " << highRateDir <<  endl;
    cout << "Using Slow Rate directory: " << slowTdrssDir <<  endl;
    cout << "Using Iridium directory: " << iridiumDir <<  endl;
    
    cout << currentLosRun << "\t" << currentLosFile << "\t" << currentTdrssFile << "\t"
	 << currentIridiumFile << "\t" << currentSlowTdrssFile << endl;
    cout << plotDir << "\t" << dataDir << endl;
//    exit(0);

    progState=1;
    int counter=0;
    while(progState) {
	retVal=0;
	checkIridium();
	checkSlowTdrss();

	if(checkForNewTDRSS) {
	    retVal=checkHighRateTDRSS();
	    //	    cout << "TDRSS: " << retVal << endl;
	}
	if(checkForNewLOS) {
	    losRetVal=checkLOS();
	    //	    cout << "LOS: " << losRetVal << endl;
	    if(losRetVal==10) {
	      int tempMakeEventPlots=makeEventPlots;
	      makeEventPlots=0;
		while(losRetVal==10) {
		    
		    losRetVal=checkLOS();
		    counter++;
		    if(counter%saveEvery==0) {
			headHandler->saveTree();
			surfhkHandler->saveTrees();
			turfRateHandler->saveTrees();
			monHandler->saveTrees();
			hkHandler->saveTree();
			gpsHandler->saveTrees();
			cmdHandler->saveTree();
			ghdHandler->saveTree();
			configModifyInt("webPlotter.config","input","losStartFile",currentLosFile,NULL);
			sleep(1);
			configModifyInt("webPlotter.config","input","losStartRun",currentLosRun,NULL);
			sleep(1);
			configModifyInt("webPlotter.config","input","tdrssStartFile",currentTdrssFile,NULL);
			sleep(1);
			configModifyInt("webPlotter.config","input","tdrssStartRun",currentTdrssRun,NULL);
			
		    }
		}
		makeEventPlots=tempMakeEventPlots;
	    }
//	    cout << "LOS: " << losRetVal << endl;
	    if(retVal>=0)
		retVal+=losRetVal;
	    if(retVal<0)
		retVal=losRetVal;
	    
	}
// 	cout << "RetVal: " << retVal << " " << "LOS: " << checkForNewLOS 
// 	     << " " << currentLosRun << " " << currentLosFile << "\t" 
// 	     << losRunLastPlot << " " << losFileLastPlot << endl;
	
	if((!retVal && 
	    ((checkForNewLOS && (currentLosFile!=losFileLastPlot ||
				 currentLosRun!=losRunLastPlot))
	     || (checkForNewTDRSS && (currentTdrssFile!=tdrssFileLastPlot || currentTdrssRun!=tdrssRunLastPlot)))) || 
	    (checkForNewLOS && ((currentLosFile-losFileLastPlot)>5 || (currentLosRun!=losRunLastPlot))) ||
	    (checkForNewTDRSS && ((currentTdrssFile-tdrssFileLastPlot)>5 || (currentTdrssRun!=tdrssRunLastPlot)))) {
// 	    cout << "LOS: " << checkForNewLOS << " " << currentLosRun << " "
// 		 << currentLosFile << "\t" << losRunLastPlot << " " << losFileLastPlot << endl;

// 	    cout << "TDRSS: " << checkForNewTDRSS << " " << currentTdrssRun << " "
// 		 << currentTdrssFile << "\t" << tdrssRunLastPlot << " " << tdrssFileLastPlot << endl;
	  counter++;
	  stopy.Start();
	  if(makePacketPlots) ghdHandler->makePlots();
	  stopy.Stop();
	  cout << "ghdHandler->makePlots: " << stopy.CpuTime() << " "
	       << stopy.RealTime() << endl;
	  stopy.Start();
	  if(makeHeaderPlots) headHandler->makePlots();
	  stopy.Stop();
	  cout << "headHandler->makePlots: " << stopy.CpuTime() << " "
	       << stopy.RealTime() << endl;
	  stopy.Start();
	  if(makeSurfHkPlots) surfhkHandler->makePlots();
	  stopy.Stop();
	  cout << "surfhkHandler->makePlots: " << stopy.CpuTime() << " "
	       << stopy.RealTime() << endl;
	  stopy.Start();
	  if(makeTurfRatePlots) turfRateHandler->makePlots();
	  stopy.Stop();
	  cout << "turfRateHandler->makePlots: " << stopy.CpuTime() << " "
	       << stopy.RealTime() << endl;
	  stopy.Start();
	  if(makeMonitorPlots) monHandler->makePlots();
	  stopy.Stop();
	  cout << "monHandler->makePlots: " << stopy.CpuTime() << " "
	       << stopy.RealTime() << endl;
	  stopy.Start();
	  if(makeHkPlots) hkHandler->makePlots();
	  stopy.Stop();
	  cout << "hkHandler->makePlots: " << stopy.CpuTime() << " "
	       << stopy.RealTime() << endl;
	  stopy.Start();
	  if(makeGpsPlots) gpsHandler->makePlots();
	  stopy.Stop();
	  cout << "gpsHandler->makePlots: " << stopy.CpuTime() << " "
	       << stopy.RealTime() << endl;
	  if(makeAuxPlots) auxHandler->makePlots();
	  stopy.Stop();
	  cout << "auxHandler->makePlots: " << stopy.CpuTime() << " "
	       << stopy.RealTime() << endl;
	    

 	    losRunLastPlot=currentLosRun;
 	    losFileLastPlot=currentLosFile;
 	    tdrssFileLastPlot=currentTdrssFile;
 	    tdrssRunLastPlot=currentTdrssRun;
 	}
	if(!retVal) sleep(1);
 	if(counter%saveEvery==0) {
 	    headHandler->saveTree();
 	    surfhkHandler->saveTrees();
 	    turfRateHandler->saveTrees();
 	    monHandler->saveTrees();
 	    hkHandler->saveTree();
 	    gpsHandler->saveTrees();
	    cmdHandler->saveTree();
	    ghdHandler->saveTree();
	    auxHandler->saveTrees();
	    configModifyInt("webPlotter.config","input","losStartFile",currentLosFile,NULL);
	    sleep(1);
	    configModifyInt("webPlotter.config","input","losStartRun",currentLosRun,NULL);
	    sleep(1);
	    configModifyInt("webPlotter.config","input","tdrssStartFile",currentTdrssFile,NULL);
	    sleep(1);
	    configModifyInt("webPlotter.config","input","tdrssStartRun",currentTdrssRun,NULL);
	    sleep(1);
	    configModifyInt("webPlotter.config","input","iridiumStartFile",currentIridiumFile,NULL);
	    sleep(1);
	    configModifyInt("webPlotter.config","input","slowTdrssStartFile",currentSlowTdrssFile,NULL);
	    
 	}
	TTimeStamp currentTime;
	if(currentTime.GetSec() > lastBackup.GetSec() +3600) {
	    backupCounter++;
	    if(backupCounter>2) backupCounter=1;
	    sprintf(backupDir,"%s/root/%d/",plotDir,currentTime.GetDate());
	    gSystem->mkdir(backupDir,kTRUE);

	    //Header File
	    sprintf(backupName,"%s/headFile_%d.root",backupDir,backupCounter);
	    headHandler->writeBackupFile(backupName);
	    //Hk File
	    sprintf(backupName,"%s/hkFile_%d.root",backupDir,backupCounter);
	    hkHandler->writeBackupFile(backupName);
	    //SurfHk File
	    sprintf(backupName,"%s/surfhkFile_%d.root",backupDir,backupCounter);
	    surfhkHandler->writeBackupFile(backupName);
	    //GPS File
	    sprintf(backupName,"%s/gpsFile_%d.root",backupDir,backupCounter);
	    gpsHandler->writeBackupFile(backupName);
	    //Turf Rate File
	    sprintf(backupName,"%s/turfRateFile_%d.root",backupDir,backupCounter);
	    turfRateHandler->writeBackupFile(backupName);
	    //Monitor File
	    sprintf(backupName,"%s/monitorFile_%d.root",backupDir,backupCounter);
	    monHandler->writeBackupFile(backupName);
	
	    //Auxiliary File
	    sprintf(backupName,"%s/auxFile_%d.root",backupDir,backupCounter);
	    auxHandler->writeBackupFile(backupName);

	    lastBackup=currentTime;
//	    break;
	}



    }
    free(bigBuffer);
    delete headHandler;
    delete surfhkHandler;
    delete turfRateHandler;
    delete monHandler;
    delete hkHandler;
    delete gpsHandler;
    delete fileHandler;
    delete cmdHandler;
    delete ghdHandler;
    delete slowHandler;
    delete auxHandler;
    configModifyInt("webPlotter.config","input","losStartFile",currentLosFile,NULL);
    sleep(1);
    configModifyInt("webPlotter.config","input","losStartRun",currentLosRun,NULL);
    sleep(1);
    configModifyInt("webPlotter.config","input","tdrssStartFile",currentTdrssFile,NULL);
    sleep(1);
    configModifyInt("webPlotter.config","input","tdrssStartRun",currentTdrssRun,NULL);
    sleep(1);
    configModifyInt("webPlotter.config","input","iridiumStartFile",currentIridiumFile,NULL);
    sleep(1);
    configModifyInt("webPlotter.config","input","slowTdrssStartFile",currentSlowTdrssFile,NULL);
}

int checkLOS() {
    static int checkForNewRun=0;
    static int testedThisOne=0;
    static int callCount=0;
    FileStat_t staty;
    char losFilename[FILENAME_MAX];
    char testDir[FILENAME_MAX];
    
    if(checkForNewRun) {
	checkForNewRun=0;
	callCount=0;
	for(int testRun=currentLosRun+1;testRun<currentLosRun+50;testRun++) {
	    sprintf(testDir,"%s/%05d",losDir,testRun);
	    if(!gSystem->GetPathInfo(testDir,staty)) {
	      printf("Switching to LOS Run %d\n",testRun);
		currentLosRun=testRun;
		currentLosFile=0;
		break;
	    }
	}
    }
	
    sprintf(losFilename,"%s/%05d/%06d",losDir,currentLosRun,currentLosFile);
    //    cout << losFilename << endl;
    if(processLOSFile(losFilename)==0) {	    

      //      cout << "Top\n";
      sprintf(losFilename,"%s/%05d/%06d",losDir,currentLosRun,currentLosFile+10);
      ifstream Test(losFilename);
      if(Test) {
	currentLosFile++;
	  Test.close();
	  return 10;
      }
      

	sprintf(losFilename,"%s/%05d/%06d",losDir,currentLosRun,currentLosFile+1);

	ifstream Test2(losFilename);
	if(Test2) {
	  currentLosFile++;
	  Test2.close();
	  return 1;
	}
		    
	testedThisOne=0;
	callCount=0;
    }
    else {
      //      cout << "Top\n";
      sprintf(losFilename,"%s/%05d/%06d",losDir,currentLosRun,currentLosFile+1);
      ifstream Test2(losFilename);
      if(Test2) {
	currentLosFile++;
	Test2.close();
	return 1;
      }
      
      
      //	cout << "Here: " << testedThisOne << " " << callCount << endl;
      if(!testedThisOne || callCount%30==1) {
	for(int newFile=currentLosFile+1;newFile<currentLosFile+1500;newFile++) {
	  sprintf(losFilename,"%s/%05d/%06d",losDir,currentLosRun,newFile);
	  ifstream Test(losFilename);
	  if(Test) {
	    Test.close();
	    currentLosFile=newFile;
	    return 1;
	  }
	  testedThisOne=1;
	}
	callCount++;
      }
      else {
	//	    sleep(1);
	callCount++;
	if(callCount>20) checkForNewRun=1;
      }
    }
    return 0;
    
}


int checkHighRateTDRSS() {
//    cout<< "Here" << endl;
    static int checkForNewRun=0;
    static int testedThisOne=0;
    static int callCount=0;
    FileStat_t staty;
    
    char tdrssFilename[FILENAME_MAX];
    char testDir[FILENAME_MAX];
    
    if(checkForNewRun) {
	checkForNewRun=0;
	callCount=0;
	for(int testRun=currentTdrssRun+1;testRun<currentTdrssRun+50;testRun++) {
	    sprintf(testDir,"%s/%05d",highRateDir,testRun);
	    if(!gSystem->GetPathInfo(testDir,staty)) {
		currentTdrssRun=testRun;
		currentTdrssFile=0;
		currentIridiumFile=0;
		currentSlowTdrssFile=0;
		break;
	    }
	}
    }
//    cout<< "Here2" << endl;
	
    sprintf(tdrssFilename,"%s/%05d/%06d",highRateDir,currentTdrssRun,currentTdrssFile);	    
//    cout << tdrssFilename << endl;
    if(processHighRateTDRSSFile(tdrssFilename)==0) {	    
	cout << tdrssFilename << endl;

	sprintf(tdrssFilename,"%s/%05d/%06d",highRateDir,currentTdrssRun,currentTdrssFile+2);
	currentTdrssFile++;
	ifstream Test(tdrssFilename);
	if(Test) {
	    Test.close();
	    cout << "Here: " << tdrssFilename << endl;
	    return 1;
	}
	    
	testedThisOne=0;
	callCount=0;
    }
    else {
//	cout << "Here: " << testedThisOne << " " << callCount << endl;
	if(!testedThisOne || callCount%30==1) {
	    for(int newFile=currentTdrssFile;newFile<currentTdrssFile+500;newFile++) {
		sprintf(tdrssFilename,"%s/%05d/%06d",highRateDir,currentTdrssRun,newFile);
		ifstream Test(tdrssFilename);
		if(Test) {
		    Test.close();
		    currentTdrssFile=newFile;
		    cout << "Here2: " << tdrssFilename << endl;
		    return 1;
		}
		testedThisOne=1;
	    }
	    callCount++;
	}
	else {
//	    sleep(1);
	    callCount++;
	    if(callCount>30) checkForNewRun=1;
	}
    }
    return 0;

}


int checkIridium() {    
    int retVal;
    char iridiumFilename[FILENAME_MAX];
    //    char testDir[FILENAME_MAX];
  
	
    sprintf(iridiumFilename,"%s/%05d/%06d",iridiumDir,currentTdrssRun,currentIridiumFile);	    
    retVal=processIridiumFile(iridiumFilename);
    if(retVal==1) {
	//Read some new data wait for more
    }
    else if(retVal==2) {
	//File stayed the same check for new file
	for(int newFile=currentIridiumFile+1;newFile<currentIridiumFile+10;newFile++) {
	    sprintf(iridiumFilename,"%s/%05d/%06d",iridiumDir,currentTdrssRun,newFile);
//	    cout << "Test: " << iridiumFilename << endl;
	    ifstream Test(iridiumFilename);
	    if(Test) {
		Test.close();
		currentIridiumFile=newFile;
		break;
	    }
	}
    }
    return 0;
}


int checkSlowTdrss() {
    int retVal;
    char slowTdrssFilename[FILENAME_MAX];
    //    char testDir[FILENAME_MAX];
  
	
    sprintf(slowTdrssFilename,"%s/%05d/%06d",slowTdrssDir,currentTdrssRun,currentSlowTdrssFile);	    
    retVal=processSlowTdrssFile(slowTdrssFilename);
    if(retVal==1) {
	//Read some new data wait for more
    }
    else if(retVal==2) {
	//File stayed the same check for new file
	for(int newFile=currentSlowTdrssFile+1;newFile<currentSlowTdrssFile+10;newFile++) {
	    sprintf(slowTdrssFilename,"%s/%05d/%06d",slowTdrssDir,currentTdrssRun,newFile);
	    ifstream Test(slowTdrssFilename);
	    if(Test) {
	      Test.close();
	      currentSlowTdrssFile=newFile;
	      break;
	    }
	}
    }
    return 0;
}

int processIridiumFile(char *filename) {
    int numBytes=0;
    static int lastNumBytes=0;
    static unsigned int lastUnixTime=0;
    static int lastIridiumFile=-1;
    int count=0;
    if(currentIridiumFile!=lastIridiumFile) {
	lastNumBytes=0;
    }

    FILE *iridiumFile=fopen(filename,"rb");
    if(!iridiumFile) {
	//File doesn't exist
	return 0;
    }
    numBytes=fread(bigBuffer,1,BIG_BUF_SIZE,iridiumFile);
    if(numBytes<=0) {
      fclose(iridiumFile);
      return 0;
    }
    
    if(numBytes==lastNumBytes) {
	//No new data
	fclose(iridiumFile);
	return 2;
    }
    lastIridiumFile=currentIridiumFile;
    cout << "Iridium: " << filename << endl;
    //we have some new data
    lastNumBytes=numBytes;

    unsigned char *charBuffer=(unsigned char*)&bigBuffer[0];

    while(count<numBytes) {	
	unsigned char comm1or2=charBuffer[count];
// 	if(count<100) {
// 	    cout << count << " " << (int) comm1or2 << endl;
// 	}
	if(comm1or2==0xc1 || comm1or2==0xc2) {
	    unsigned char seqNum=charBuffer[count+1];
	    unsigned char numBytes=charBuffer[count+2];
	    unsigned char *sciData=&charBuffer[count+3];
	    count+=numBytes+3;
	    
//	    printf("count %d -- comm %#x, seqNum %d, numBytes %d\n",
//		   count,comm1or2,seqNum,numBytes);
	    if(checkPacket((GenericHeader_t*)sciData)==0) {
		if(numBytes==sizeof(SlowRateFull_t)) {
		    //Probably have SlowRateFull_t
		    SlowRateFull_t *slowPtr = (SlowRateFull_t*)sciData;
		    if(slowPtr->unixTime>lastUnixTime) {
		    //Have new SlowRateFull_t
			lastUnixTime=slowPtr->unixTime;
			slowHandler->addIridium(slowPtr,seqNum);
			//			gpsHandler->makeGpsMap(slowPtr->unixTime,slowPtr->hk.latitude,
			//					       slowPtr->hk.longitude,slowPtr->hk.altitude);
		    //Do something with GPS data
		    }
		}
	    }
	}
	else {
	    count++;
	}
    }
    return 1;
}
 

int processSlowTdrssFile(char *filename) {
    int numBytes=0;
    static int lastNumBytes=0;
    static unsigned int lastUnixTime=0;
    static int lastSlowTdrssFile=-1;
    int count=0;
    if(currentSlowTdrssFile!=lastSlowTdrssFile) {
	lastNumBytes=0;
    }

    FILE *slowTdrssFile=fopen(filename,"rb");
    if(!slowTdrssFile) {
	//File doesn't exist
	return 0;
    }
    numBytes=fread(bigBuffer,1,BIG_BUF_SIZE,slowTdrssFile);
    if(numBytes<=0) {
      fclose(slowTdrssFile);
	return 0;
    }
    if(numBytes==lastNumBytes) {
	//No new data
	fclose(slowTdrssFile);
	return 2;
    }

    lastSlowTdrssFile=currentSlowTdrssFile;
    cout << "Slow Tdrss: " << filename << endl;
    //we have some new data
    lastNumBytes=numBytes;

    unsigned char *charBuffer=(unsigned char*)&bigBuffer[0];

    while(count<numBytes) {
      unsigned char comm1or2=charBuffer[count];
      //      cout << count << " " << numBytes << "\t" << Int_t(comm1or2) << endl;

	if(comm1or2==0xc1 || comm1or2==0xc2) {
	    unsigned char seqNum=charBuffer[count+1];
	    unsigned char numBytes=charBuffer[count+2];
	    unsigned char *sciData=&charBuffer[count+3];
	    count+=numBytes+3;
	    
	    //	    printf("count %d -- comm %#x, seqNum %d, numBytes %d\n",
	    //		   count,comm1or2,seqNum,numBytes);
	    //	    printf("sizeof(SlowRateFull_t) -- %d\n",sizeof(SlowRateFull_t));
	    if(numBytes==sizeof(SlowRateFull_t)) {
	      //		cout << "Yes" << endl;
		//Probably have SlowRateFull_t
		SlowRateFull_t *slowPtr = (SlowRateFull_t*)sciData;
		//		cout << slowPtr->unixTime << " " << lastUnixTime << endl;

		if(checkPacket((GenericHeader_t*)slowPtr)==0 ) {
		    		    
		    if(slowPtr->unixTime>lastUnixTime) {
		      //			cout << "yep" << endl;
			//Have new SlowRateFull_t
			lastUnixTime=slowPtr->unixTime;
			slowHandler->addSlowTdrss(slowPtr,seqNum);
//			cout << "Here" << endl;
//			gpsHandler->makeGpsMap(slowPtr->unixTime,slowPtr->hk.latitude,
//					       slowPtr->hk.longitude,slowPtr->hk.altitude);
//			cout << "And Here" << endl;
			//Do something with GPS data
		    }
		}
	    }
	    
	}
	else {
	    count++;
	}
    }
    return 1;
}




void sigHandler(int sig) {
    cerr << "Will quit and save after processing this file" << endl;
    progState=0;
}


void sigSegvHandler(int sig) {
    static int count=0;
    count++;
    if(count>3) {
	cerr << "Bollocks something up" << endl;
	exit(0);
    }
    cerr << "Got SIGSEGV Will try and save right now" << endl;
    delete headHandler;
    delete surfhkHandler;
    delete turfRateHandler;
    delete monHandler;
    delete hkHandler;
    delete gpsHandler;
    delete fileHandler;
    delete cmdHandler;
    delete ghdHandler;
    currentLosFile++;
    configModifyInt("webPlotter.config","input","losStartFile",currentLosFile,NULL);
    sleep(1);
    configModifyInt("webPlotter.config","input","losStartRun",currentLosRun,NULL);
    sleep(1);
    currentTdrssFile++;
    configModifyInt("webPlotter.config","input","tdrssStartFile",currentTdrssFile,NULL);
    sleep(1);
    configModifyInt("webPlotter.config","input","tdrssStartRun",currentTdrssRun,NULL);
    sleep(1);
    configModifyInt("webPlotter.config","input","iridiumStartFile",currentIridiumFile,NULL);
    sleep(1);
    configModifyInt("webPlotter.config","input","slowTdrssStartFile",currentSlowTdrssFile,NULL);
    exit(0);
//    progState=0;
}

int processLOSFile(char *filename) {
//    cout << (int) headHandler->number << endl;
  static int lastNumBytes=0;
  //  static unsigned int lastUnixTime=0;
  static int lastLosFile=-1;
  if(currentLosFile!=lastLosFile) {
    lastNumBytes=0;
  }
  //  cout << "processLOSFile: " << filename << "\t" << lastLosFile << "\t" << currentLosFile << endl;
  lastLosFile=currentLosFile;
  int numBytes=0,count=0;
  FILE *losFile;
  
    //data stuff
    //    unsigned short numWords;
    //    unsigned short unused;
    unsigned short foodHdr;
    unsigned short doccHdr;
    unsigned short ae00Hdr;
    int losOrSip;
    int oddOrEven;
    unsigned int bufferCount;
    unsigned int *ulPtr;
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
    ghdHandler->newLosFile(currentLosRun,currentLosFile,buf.st_mtime);
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
	    //	    numWords=bigBuffer[count+3];
	    //	    unused=bigBuffer[count+2];

	    //	    printf("numWords -- %d\n",numWords);
	    //	    printf("unused -- %d\n",unused);
	    //	    printf("foodHdr -- %x\n",foodHdr);
	    //	    printf("doccHdr -- %x\n",doccHdr);
	    //	    printf("ae00Hdr -- %x\n",ae00Hdr);

	    //	    exit(0);
	    if(foodHdr==0xf00d && doccHdr==0xd0cc && (ae00Hdr&0xfff0)==0xae00) {
		//Got a los buffer
		losOrSip=ae00Hdr&0x1;
		oddOrEven=ae00Hdr&0x2>>1;
		ulPtr = (unsigned int*) &bigBuffer[count+3];
		bufferCount=*ulPtr;
		numSciBytes=bigBuffer[count+5];
		//		printf("Buffer %u -- %d bytes\n",bufferCount,numSciBytes);
		checksum=bigBuffer[count+6+(numSciBytes/2)];		
		swEndHdr=bigBuffer[count+7+(numSciBytes/2)];
		endHdr=bigBuffer[count+8+(numSciBytes/2)];
		auxHdr2=bigBuffer[count+9+(numSciBytes/2)];

		//Now do something with buffer
		handleScience((unsigned char*)&bigBuffer[count+6],numSciBytes);
		//		printf("swEndHdr -- %x\n",swEndHdr);
		//		printf("endHdr -- %x\n",endHdr);
		//		printf("auxHdr2 -- %x\n",auxHdr2);		
		//		exit(0);
//		return 0;
		count+=10+(numSciBytes/2);
		continue;
	    }
	}
	count++;
    }
    ghdHandler->makeLastFilePlots();
    

    return 0;

}


int processHighRateTDRSSFile(char *filename) {
//    cout << (int) headHandler->number << endl;

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
    ghdHandler->newTdrssFile(currentTdrssRun,currentTdrssFile,buf.st_mtime);

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

    ghdHandler->makeLastFilePlots();        

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


	//	printf("Got %s (%#x) -- (%d bytes)\n",
	//	       packetCodeAsString(gHdr->code),
	//	       gHdr->code,gHdr->numBytes);
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
	  if(testGHdr->code>0) ghdHandler->addHeader(testGHdr);
	    //	    printf("Got %s (%#x) -- (%d bytes)\n",
	    //		   packetCodeAsString(testGHdr->code),
	    //		   testGHdr->code,testGHdr->numBytes);
	    switch(testGHdr->code&BASE_PACKET_MASK) {
	    case PACKET_HD:
	      //	      cout << "Got Header\n";
	      hdPtr= (AnitaEventHeader_t*)testGHdr;
	      if(time_t(hdPtr->unixTime)<time_t(nowTime+1000))
		headHandler->addHeader(hdPtr);
	      break;
	    case PACKET_SURF_HK:
	      //	      cout << "Got SurfHk\n";
	      surfPtr = (FullSurfHkStruct_t*) testGHdr;
	      if((time_t)surfPtr->unixTime<time_t(nowTime+1000))
		surfhkHandler->addSurfHk(surfPtr);
	      break;
	    case PACKET_AVG_SURF_HK:
	      //	      cout << "Got AveragedSurfHkStruct_t\n";
	      surfhkHandler->addAveragedSurfHk((AveragedSurfHkStruct_t*) testGHdr);
	      break;
	      
	    case PACKET_TURF_RATE:
	      //	      cout << "Got TurfRate\n";
	      turfRateHandler->addTurfRate((TurfRateStruct_t*)testGHdr);
	      break;
	    case PACKET_SUM_TURF_RATE:
	      //	      cout << "Got SummedTurfRateStruct_t\n";
	      turfRateHandler->addSumTurfRate((SummedTurfRateStruct_t*)testGHdr);
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
	      gpsHandler->addAdu5Sat((GpsAdu5SatStruct_t*) testGHdr);
	      break;
	    case PACKET_GPS_ADU5_PAT:
	      //	      cout << "Got GpsAdu5PatStruct_t\n";
	      gpsHandler->addAdu5Pat((GpsAdu5PatStruct_t*) testGHdr);
	      break;
	    case PACKET_GPS_ADU5_VTG:
	      //	      cout << "Got GpsAdu5VtgStruct_t\n";
	      gpsHandler->addAdu5Vtg((GpsAdu5VtgStruct_t*) testGHdr);
	      break;
	    case PACKET_GPS_GGA:
	      //	      cout << "Got GpsGgaStruct_t\n";
	      gpsHandler->addGpsGga((GpsGgaStruct_t*) testGHdr);
	      break;
	    case PACKET_ZIPPED_FILE:
	      //	      cout << "Got ZippedFile_t\n";
	      ///		    printf("Boo\n");
	      fileHandler->processFile((ZippedFile_t*) testGHdr);
	      break;
	    case PACKET_CMD_ECHO:
	      //	      cout << "Got CommandEcho_t\n";
	      cmdHandler->addCmdEcho((CommandEcho_t*) testGHdr);
	      break;
		    
	    case PACKET_SURF:
	      cout << "Got RawSurfPacket_t\n";
	      //	      if(makeEventPlots) headHandler->addRawSurfPacket((RawSurfPacket_t*)testGHdr);
	      break;
	    case PACKET_WV:
	      cout << "Got RawWaveformPacket_t\n";
	      if(makeEventPlots) headHandler->addRawWavePacket((RawWaveformPacket_t*)testGHdr);
	      break;
	    case PACKET_ENC_SURF:
	      cout << "Got EncodedSurfPacketHeader_t\n";
	      //	      if(makeEventPlots) 
	      headHandler->addEncSurfPacket((EncodedSurfPacketHeader_t*)testGHdr);
	      break;
	    case PACKET_ENC_SURF_PEDSUB:
	      //	      cout << "Got EncodedPedSubbedSurfPacketHeader_t\n";
	      if(makeEventPlots) 
		headHandler->addEncPedSubbedSurfPacket((EncodedPedSubbedSurfPacketHeader_t*)testGHdr);
	      break;
	    case PACKET_ENC_WV_PEDSUB:
	      cout << "Got EncodedPedSubbeWavePacketHeader_t\n";
	      if(makeEventPlots) headHandler->addEncPedSubbedWavePacket((EncodedPedSubbedChannelPacketHeader_t*)testGHdr);
	      break;
	      
	    case PACKET_HKD:	      
	      hkPtr = (HkDataStruct_t*)testGHdr;
	      //	      cout << "Got HkDataStruct_t " << hkPtr->ip320.code << "\n";
	      if(hkPtr->ip320.code==IP320_RAW)
		hkHandler->addHk(hkPtr);
	      else if(hkPtr->ip320.code==IP320_CAL)
		hkHandler->addCalHk(hkPtr);
	      else if(hkPtr->ip320.code==IP320_AVZ)
		hkHandler->addAvzHk(hkPtr);
	      break;	 

	    case PACKET_ACQD_START:
	      cout << "Got Acqd Start Packet\n";
	      auxHandler->addAcqdStart((AcqdStartStruct_t*)testGHdr);
	      break;
	    case PACKET_GPSD_START:
	      cout << "Got GPSd Start Packet\n";
	      auxHandler->addGpsdStart((GpsdStartStruct_t*)testGHdr);
	      break;
	    case PACKET_LOGWATCHD_START:
	      cout << "Got Log Watchd Start Packet\n";
	      auxHandler->addLogWatchdStart((LogWatchdStart_t*)testGHdr);
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
	    printf("Problem with packet -- checkVal==%d  (code? %#x)\n",
		   checkVal,gHdr->code);
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

void printSurfInfo(EncodedSurfPacketHeader_t *surfPtr) 
{
    EncodedSurfChannelHeader_t *chan0= (EncodedSurfChannelHeader_t*)(surfPtr + sizeof(EncodedSurfPacketHeader_t));
    printf("Event %u  Chan 0: numBytes %d crc %d\n",surfPtr->eventNumber,chan0->numBytes,chan0->crc);

}



void printPedSubSurfInfo(EncodedPedSubbedSurfPacketHeader_t *surfPtr) 
{
    SurfChannelPedSubbed_t psSurfChan;
    unsigned char *eventBuffer;
    int count=0,chan,i;
    EncodedSurfChannelHeader_t *chanHdPtr;
    printf("event %u, numBytes %d\n",surfPtr->eventNumber,surfPtr->gHdr.numBytes);
    eventBuffer = (unsigned char*) surfPtr;
    count=sizeof(EncodedPedSubbedSurfPacketHeader_t);
    for(chan=0;chan<CHANNELS_PER_SURF;chan++) {
	
	chanHdPtr = (EncodedSurfChannelHeader_t*)&eventBuffer[count];
	    
	printf("Event %u  Chan %d: encType %d, numBytes %d crc %d\n",surfPtr->eventNumber,chan,chanHdPtr->encType,chanHdPtr->numBytes,chanHdPtr->crc);
	count+=sizeof(EncodedSurfChannelHeader_t);
	CompressErrorCode_t retVal=AnitaCompress::decodePSChannel(chanHdPtr,&eventBuffer[count], &psSurfChan);
	if(retVal==COMPRESS_E_OK) {
	    if(chan<8) {
		if(psSurfChan.mean<-50 || psSurfChan.mean>50) {
		    for(i=0;i<100;i++) {
			printf("**********************************\n");
		    }
		}
	    }
	    printf("Chan mean %f, rms %f, xMin %d, xMax %d\n",
		   psSurfChan.mean,psSurfChan.rms,psSurfChan.xMin,
		   psSurfChan.xMax);
	}
	else {
	    printf("Problem decoding PS channel\n");
	}
	count+=chanHdPtr->numBytes;

    }

}


int readConfigFile() 
{
    /* Config file thingies */
    ConfigErrorCode status=CONFIG_E_OK;
    //    int tempNum=12;
    //    KvpErrorCode kvpStatus=KVP_E_OK;
    char* eString ;
    char *tempString;
    kvpReset();
    status = configLoad ("webPlotter.config","input") ;    
    if(status == CONFIG_E_OK) {       
	checkForNewLOS=kvpGetInt("checkForNewLOS",1);
	checkForNewTDRSS=kvpGetInt("checkForNewTDRSS",1);
	saveEvery=kvpGetInt("saveEvery",100);
	losStartRun=kvpGetInt("losStartRun",10);
	losStartFile=kvpGetInt("losStartFile",10);
	tdrssStartRun=kvpGetInt("tdrssStartRun",10);
	tdrssStartFile=kvpGetInt("tdrssStartFile",10);
	iridiumStartFile=kvpGetInt("iridiumStartFile",0);
	slowTdrssStartFile=kvpGetInt("slowTdrssStartFile",0);
	tempString = kvpGetString("losDir");
	if(tempString) {
	    strncpy(losDir,tempString,FILENAME_MAX);
	}
	else {
	    cerr << "Couldn't get losDir" << endl;
	    exit(0);
	}
	tempString = kvpGetString("highRateDir");
	if(tempString) {
	    strncpy(highRateDir,tempString,FILENAME_MAX);
	}
	else {
	    cerr << "Couldn't get highRateDir" << endl;
	    exit(0);
	}
	tempString = kvpGetString("iridiumDir");
	if(tempString) {
	    strncpy(iridiumDir,tempString,FILENAME_MAX);
	}
	else {
	    cerr << "Couldn't get iridiumDir" << endl;
	    exit(0);
	}
	tempString = kvpGetString("slowTdrssDir");
	if(tempString) {
	    strncpy(slowTdrssDir,tempString,FILENAME_MAX);
	}
	else {
	    cerr << "Couldn't get slowTdrssDir" << endl;
	    exit(0);
	}
    }
    else {
	eString=configErrorString (status) ;
	cerr << "Error reading webPlotter.config: " << eString << endl;
    }
    kvpReset();
    status = configLoad ("webPlotter.config","output") ;    
    if(status == CONFIG_E_OK) {       
	tempString = kvpGetString("plotDir");
	if(tempString) {
	    strncpy(plotDir,tempString,FILENAME_MAX);
	}
	else {
	    cerr << "Couldn't get plotDir" << endl;
	    exit(0);
	}
	tempString = kvpGetString("dataDir");
	if(tempString) {
	    strncpy(dataDir,tempString,FILENAME_MAX);
	}
	else {
	    cerr << "Couldn't get dataDir" << endl;
	    exit(0);
	}
    }
    else {
	eString=configErrorString (status) ;
	cerr << "Error reading webPlotter.config: " << eString << endl;
    }

    kvpReset();
    status = configLoad ("webPlotter.config","plots") ;    
    if(status == CONFIG_E_OK) {  
	makeHeaderPlots=kvpGetInt("makeHeaderPlots",1);
	makeSurfHkPlots=kvpGetInt("makeSurfHkPlots",1);
	makeTurfRatePlots=kvpGetInt("makeTurfRatePlots",1);
	makeMonitorPlots=kvpGetInt("makeMonitorPlots",1);
	makeHkPlots=kvpGetInt("makeHkPlots",1);
	makeGpsPlots=kvpGetInt("makeGpsPlots",1);
	makeEventPlots=kvpGetInt("makeEventPlots",1);
    }
    else {
	eString=configErrorString (status) ;
	cerr << "Error reading webPlotter.config: " << eString << endl;
    }
    
    return status;
   
}
