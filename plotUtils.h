////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple utiltities for making the web plots                  /////////
//////                                                             /////////
////// rjn@hep.ucl.ac.uk --- September 2006                        /////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


#ifndef PLOTUTILS_H
#define PLOTUTILS_H

#define NUM_TIME_RANGES 5 

typedef enum {
    kFullTime =1,
    kOneHour,
    kSixHours,
    kTwelveHours,
    kOneDay,
    kNoTime
} AnitaPlotTime_t;

char * getTimeString(AnitaPlotTime_t plotTime);
char * getTimeTitleString(AnitaPlotTime_t plotTime);
void setDefaultStyle();
int getNiceColour(int index);
int getNiceMarker(int index);
unsigned long getPlotInterval(AnitaPlotTime_t plotTime);



//AnitaPlotTime_t operator++ (const AnitaPlotTime_t& x, int);

#endif //PLOTUTILS_H
