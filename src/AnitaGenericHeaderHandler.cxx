////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle telemetered AnitaEventHeader_t       /////////
//////                                                             /////////
////// r.nichol@ucl.ac.uk --- July 2014                            /////////
////////////////////////////////////////////////////////////////////////////

#include "AnitaGenericHeaderHandler.h"

#include <iostream>
#include <fstream>
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "AnitaPacketUtil.h"
#include "simpleStructs.h"

//AWARE Include
#include "AwareRunSummaryFileMaker.h"
#include "AwareRunDatabase.h"


#define HACK_FOR_ROOT




AnitaGenericHeaderHandler::AnitaGenericHeaderHandler(std::string awareDir)
  :fAwareDir(awareDir),fFileType(AnitaTelemFileType::kNotATelemType),fCurrentFile(-1),fCurrentRun(-1)
{
 

}

AnitaGenericHeaderHandler::~AnitaGenericHeaderHandler()
{


}
    
void AnitaGenericHeaderHandler::addGenericHeader(GenericHeader_t *hdPtr)
{
  fGhdMap.insert(std::pair<UInt_t,GenericHeader_t>(hdPtr->packetNumber,*hdPtr));  
}


void AnitaGenericHeaderHandler::writeFileSummary() 
{
  if(fFileType==AnitaTelemFileType::kNotATelemType) return;

  const char *telemTypeForFile[3]={"Los","Tdrss","Openport"};
  char outputDir[FILENAME_MAX];
  sprintf(outputDir,"%s/ANITA4/ghd",fAwareDir.c_str());
  gSystem->mkdir(outputDir,kTRUE);
  
  unsigned long totalBytes=0;
  char totalBytesFile[FILENAME_MAX];
  sprintf(totalBytesFile,"%s/ANITA4/ghd/total%s",fAwareDir.c_str(),telemTypeForFile[fFileType]);
  
  std::ifstream ByteFile(totalBytesFile);
  if(ByteFile) {
    ByteFile >> totalBytes;
    ByteFile.close();
  }

  unsigned long lastModTime=0;
  unsigned long lastModTimeRun=0;
  unsigned long lastModTimeFile=0;
  char lastModTimeFilename[FILENAME_MAX];
  sprintf(lastModTimeFilename,"%s/ANITA4/ghd/lastModTime%s",fAwareDir.c_str(),telemTypeForFile[fFileType]);
  
  std::ifstream ModFile(lastModTimeFilename);
  if(ModFile) {
    ModFile >> lastModTime >> lastModTimeRun >> lastModTimeFile;
    ModFile.close();
  }


  
  char fileTypeName[20];
  sprintf(fileTypeName,"%s",telemTypeForFile[fFileType]);

  char touchName[FILENAME_MAX];
  sprintf(touchName,"%s/ANITA4/last%s",fAwareDir.c_str(),fileTypeName);
  AwareRunDatabase::updateTouchFile(touchName,fCurrentFile,fCurrentFileTime);

  unsigned int fakeRun=(fCurrentRun*1000000) + fCurrentFile;

  AwareRunSummaryFileMaker summaryFile(fakeRun,fileTypeName,1);

  char histTitle[180];

  //    int packet;
  sprintf(histTitle,"Packet Distribution (%s, Run %d, File %d)",
	  fileTypeName,fCurrentRun,fCurrentFile);
  TH1F histPacket("histPacket",histTitle,44,-0.5,43.5);
  sprintf(histTitle,"Bytes Distribution (%s, Run %d, File %d)",
	  fileTypeName,fCurrentRun,fCurrentFile);
  TH1F histPacketBytes("histPacketBytes",histTitle,44,-0.5,43.5);

  UInt_t numFileBytes=0;
  UInt_t numFileEventBytes=0;

  std::map<UInt_t,GenericHeader_t>::iterator it;
  for(it=fGhdMap.begin();it!=fGhdMap.end();it++) {        
    int logicCode=getLogicalPacketCode(it->second.code);
    histPacket.Fill(logicCode);
    histPacketBytes.Fill(logicCode,it->second.numBytes);
    
    numFileBytes+=it->second.numBytes;
    if(logicCode>=7 && logicCode<=14)
      numFileEventBytes+=it->second.numBytes;
  }
  fGhdMap.clear();

  char elementName[180];
  char elementLabel[180];

  sprintf(elementName,"numFileBytes");
  sprintf(elementLabel,"File Bytes");
  summaryFile.addVariablePoint(elementName,elementLabel,fCurrentFileTime,numFileBytes);
  sprintf(elementName,"numFileEventBytes");
  sprintf(elementLabel,"Event Bytes");
  summaryFile.addVariablePoint(elementName,elementLabel,fCurrentFileTime,numFileEventBytes);

  totalBytes+=numFileBytes;
  sprintf(elementName,"totalBytes");
  sprintf(elementLabel,"Total Bytes");
  summaryFile.addVariablePoint(elementName,elementLabel,fCurrentFileTime,totalBytes);


  float rate=0;
  if(lastModTime>0 && (lastModTimeRun<fCurrentRun || lastModTimeFile<fCurrentFile)) {
    //Can calculate rate
    rate=(numFileBytes*8./1024)/(fCurrentFileTime-lastModTime);
  }
  sprintf(elementName,"fileRate");
  sprintf(elementLabel,"Rate (kbps)");
  summaryFile.addVariablePoint(elementName,elementLabel,fCurrentFileTime,rate);

  
  for(int bin=1;bin<=histPacket.GetNbinsX();bin++) {
    int logicCode=(int)histPacket.GetBinCenter(bin);
    sprintf(elementName,"packetNum%d",logicCode);
    sprintf(elementLabel,"Num. %s",packetCodeAsString(getRealPacketCode(logicCode)));
    summaryFile.addVariablePoint(elementName,elementLabel,fCurrentFileTime,histPacket.GetBinContent(bin));
  }
  for(int bin=1;bin<=histPacketBytes.GetNbinsX();bin++) {
    int logicCode=(int)histPacketBytes.GetBinCenter(bin);
    sprintf(elementName,"packetBytes%d",logicCode);
    sprintf(elementLabel,"Bytes %s",packetCodeAsString(getRealPacketCode(logicCode)));
    summaryFile.addVariablePoint(elementName,elementLabel,fCurrentFileTime,histPacketBytes.GetBinContent(bin));
  }


  char dirName[FILENAME_MAX];
  sprintf(dirName,"%s/%s/%05d/%06d/",outputDir,fileTypeName,fCurrentRun,fCurrentFile);
  gSystem->mkdir(dirName,kTRUE);

  std::cout << "Making: " << dirName << "\n";
  

  char fullDir[FILENAME_MAX];
  sprintf(fullDir,"%s/full",dirName);
  gSystem->mkdir(fullDir,kTRUE);
  summaryFile.writeFullJSONFiles(fullDir,"ghd");

  char outName[FILENAME_MAX];

  sprintf(outName,"%s/ghdSummary.json.gz",dirName);
  summaryFile.writeSummaryJSONFile(outName);


  sprintf(outName,"%s/ghdTime.json.gz",dirName);
  summaryFile.writeTimeJSONFile(outName);

  std::ofstream ByteFileOut(totalBytesFile);
  if(ByteFileOut) {
    ByteFileOut << totalBytes << "\n";
    ByteFileOut.close();
  }

  std::ofstream LastModFileOut(lastModTimeFilename);
  if(LastModFileOut) {
    LastModFileOut << fCurrentFileTime << "\t" << fCurrentRun << "\t" << fCurrentFile << "\n";
    LastModFileOut.close();
  }
    
  



}

void AnitaGenericHeaderHandler::newFile(AnitaTelemFileType::AnitaTelemFileType_t fileType,int runNum, int fileNum, unsigned int unixTime)
{
  if(fFileType!=AnitaTelemFileType::kNotATelemType) {
    writeFileSummary();
  }
  fFileType=fileType;
  fCurrentRun=runNum;
  fCurrentFile=fileNum;
  fCurrentFileTime=unixTime;  
}

int AnitaGenericHeaderHandler::getLogicalPacketCode(PacketCode_t code)
{

    switch(code&BASE_PACKET_MASK) {	
	case PACKET_BD: return 0; // AnitaEventBody_t -- No
	case PACKET_HD: return 1; //AnitaEventHeader_t --Yes
	case PACKET_WV: return 2; //RawWaveformPacket_t --Yes
	case PACKET_SURF: return 3; //RawSurfPacket_t -- Yes
	case PACKET_HD_SLAC: return 4;
	case PACKET_SURF_HK: return 5; //FullSurfHkStruct_t --Yes
	case PACKET_TURF_RATE: return 6; //TurfRateStruct_t -- Yes
	case PACKET_PEDSUB_WV: return 7; //PedSubbedWaveformPacket_t -- Yes
	case PACKET_ENC_SURF: return 8; //EncodedSurfPacketHeader_t -- Yes
	case PACKET_ENC_SURF_PEDSUB: return 9; //EncodedPedSubbedSurfPacketHeader_t -- Yes
	case PACKET_ENC_EVENT_WRAPPER: return 10; 
	case PACKET_PED_SUBBED_EVENT: return 11; //PedSubbedEventBody_t -- No too big
	case PACKET_ENC_WV_PEDSUB: return 12; // EncodedPedSubbedChannelPacketHeader_t -- Yes
	case PACKET_ENC_PEDSUB_EVENT_WRAPPER: return 13;
	case PACKET_PEDSUB_SURF: return 14; //PedSubbedSurfPacket_t -- Yes 
	case PACKET_LAB_PED: return 15; //
	case PACKET_FULL_PED: return 16; //Too big to telemeter
	case PACKET_GPS_ADU5_PAT: return 17;
	case PACKET_GPS_ADU5_SAT: return 18;
	case PACKET_GPS_ADU5_VTG: return 19;
	case PACKET_GPS_G12_POS: return 20;
	case PACKET_GPS_G12_SAT: return 21;
	case PACKET_HKD: return 22;
	case PACKET_CMD_ECHO: return 23;
	case PACKET_MONITOR: return 24;
	case PACKET_WAKEUP_LOS: return 25;
	case PACKET_WAKEUP_HIGHRATE: return 26;
	case PACKET_WAKEUP_COMM1: return 27;
	case PACKET_WAKEUP_COMM2: return 28;
	case PACKET_SLOW1: return 29;
	case PACKET_SLOW2: return 30;
	case PACKET_SLOW_FULL: return 31;
	case PACKET_ZIPPED_PACKET: return 32; // Is just a zipped version of another packet
	case PACKET_ZIPPED_FILE: return 33; // Is a zipped file
    case PACKET_GPS_GGA: return 34;
    case PACKET_AVG_SURF_HK: return 35;
    case PACKET_SUM_TURF_RATE: return 36;
    case PACKET_RUN_START: return 37;
    case PACKET_OTHER_MONITOR: return 38;
    case PACKET_GPSD_START: return 39;
    case PACKET_LOGWATCHD_START: return 40;
    case PACKET_ACQD_START: return 41;
	default:
	    return 42;
    }
    return 42;
}

PacketCode_t AnitaGenericHeaderHandler::getRealPacketCode(int logicCode)
{
    switch(logicCode) {
	case 0: return PACKET_BD;
	case 1: return PACKET_HD; 
	case 2: return PACKET_WV; 
	case 3: return PACKET_SURF; 
	case 4: return PACKET_HD_SLAC; 
	case 5: return PACKET_SURF_HK; 
	case 6: return PACKET_TURF_RATE; 
	case 7: return PACKET_PEDSUB_WV; 
	case 8: return PACKET_ENC_SURF; 
	case 9: return PACKET_ENC_SURF_PEDSUB; 
	case 10: return PACKET_ENC_EVENT_WRAPPER; 
	case 11: return PACKET_PED_SUBBED_EVENT; 
	case 12: return PACKET_ENC_WV_PEDSUB; 
	case 13: return PACKET_ENC_PEDSUB_EVENT_WRAPPER; 
	case 14: return PACKET_PEDSUB_SURF; 
	case 15: return PACKET_LAB_PED; 
	case 16: return PACKET_FULL_PED; 
	case 17: return PACKET_GPS_ADU5_PAT; 
	case 18: return PACKET_GPS_ADU5_SAT; 
	case 19: return PACKET_GPS_ADU5_VTG; 
	case 20: return PACKET_GPS_G12_POS; 
	case 21: return PACKET_GPS_G12_SAT; 
	case 22: return PACKET_HKD; 
	case 23: return PACKET_CMD_ECHO; 
	case 24: return PACKET_MONITOR; 
	case 25: return PACKET_WAKEUP_LOS; 
	case 26: return PACKET_WAKEUP_HIGHRATE; 
	case 27: return PACKET_WAKEUP_COMM1; 
	case 28: return PACKET_WAKEUP_COMM2; 
	case 29: return PACKET_SLOW1; 
	case 30: return PACKET_SLOW2; 
	case 31: return PACKET_SLOW_FULL; 
	case 32: return PACKET_ZIPPED_PACKET; 
	case 33: return PACKET_ZIPPED_FILE; 
	default:
	    return PACKET_BD;
    }
    return PACKET_BD;
}

