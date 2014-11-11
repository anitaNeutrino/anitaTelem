////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle making plots from AuxiliaryStruct_t /////////
//////                                                             /////////
////// rjn@hep.ucl.ac.uk --- September 2006                        /////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#ifndef ANITAAUXILIARYHANDLER_H
#define ANITAAUXILIARYHANDLER_H

#include "TFile.h"
#include "TTree.h"

#include "simpleStructs.h"
#include "plotUtils.h"    


class RunStart;
class AcqdStart;
class LogWatchdStart;
class GpsdStart;

class AnitaAuxiliaryHandler 
{   
public:
    AnitaAuxiliaryHandler(std::string rawDir);
    ~AnitaAuxiliaryHandler();

    void addRunStart(RunStart_t *runStartPtr,int run);
    void addAcqdStart(AcqdStartStruct_t *acqdStartPtr,int run);
    void addGpsdStart(GpsdStartStruct_t *gpsdStartPtr,int run);
    void addLogWatchdStart(LogWatchdStart_t *logWatchdStartPtr,int run);

    void loopAcqdStartMap();
    void loopGpsdStartMap();
    void loopLogWatchdStartMap();

    
private:
    std::string fRawDir;
    int fRun;
    std::map<UInt_t,std::map<UInt_t, RunStart_t> > fRunStartMap;
    std::map<UInt_t,std::map<UInt_t, AcqdStartStruct_t> > fAcqdStartMap;
    std::map<UInt_t,std::map<UInt_t, GpsdStartStruct_t> > fGpsdStartMap;
    std::map<UInt_t,std::map<UInt_t, LogWatchdStart_t> > fLogWatchdStartMap;



};

#endif //ANITAAUXILIARYHANDLER_H


