

void makeRunNumberMap() {
  char fileName[180];
  std::ofstream OutFile("runNumberMap");

  for(int run=10001;run<10300;run++) {
    UInt_t firstTime=2000000000;
    UInt_t eventNumber=200000000;

    sprintf(fileName,"/anitaStorage/antarctica14/root/run%d/hkFile%d.root",run,run);
    TFile *fpHk = TFile::Open(fileName);
    if(fpHk) {
      TTree *hkTree = (TTree*)fpHk->Get("hkTree");
      if(hkTree) {
	UInt_t time=hkTree->GetMinimum("realTime");
	if(time<firstTime)
	  firstTime=time;
      }
    }

    sprintf(fileName,"/anitaStorage/antarctica14/root/run%d/headFile%d.root",run,run);
    TFile *fpHead = TFile::Open(fileName);
    if(fpHead) {
      TTree *headTree = (TTree*)fpHead->Get("headTree");
      if(headTree) {
	UInt_t time=headTree->GetMinimum("realTime");
	if(time<firstTime)
	  firstTime=time;
	UInt_t event=headTree->GetMinimum("eventNumber");
	if(event<eventNumber)
	  eventNumber=event;
      }
    }
    if(firstTime < 2000000000) {
      OutFile << run << "\t" << firstTime << "\t" << eventNumber << "\n";
    }

  }
  OutFile.close();



}
