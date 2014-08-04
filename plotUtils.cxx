////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple utiltities for making the web plots                  /////////
//////                                                             /////////
////// rjn@hep.ucl.ac.uk --- September 2006                        /////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#include "plotUtils.h"
#include "TStyle.h"
#include "TROOT.h"

char * getTimeString(AnitaPlotTime_t plotTime)
{
    char *myString;
    switch(plotTime) {
	case kFullTime:
	    myString="All";
	    break;
	case kOneHour:
	    myString="OneHour";
	    break;
	case kSixHours:
	    myString="SixHours";
	    break;
	case kTwelveHours:
	    myString="TwelveHours";
	    break;
	case kOneDay:
	    myString="OneDay";
	    break;
	default:
	    myString="Unknown";
	    break;
    }
    return myString;
}


char * getTimeTitleString(AnitaPlotTime_t plotTime)
{
    char *myString;
    switch(plotTime) {
	case kFullTime:
	    myString="All Time";
	    break;
	case kOneHour:
	    myString="Last Hour";
	    break;
	case kSixHours:
	    myString="Last Six Hours";
	    break;
	case kTwelveHours:
	    myString="Last Twelve Hours";
	    break;
	case kOneDay:
	    myString="Last Day";
	    break;
	default:
	    myString="Unknown";
	    break;
    }
    return myString;
}


//AnitaPlotTime_t operator++ (const AnitaPlotTime_t& x, int)
//{
//    int ix=(int)x;
//    ++x;
//    return (AnitaPlotTime_t)x;
//
//}




void setDefaultStyle() {
//    gROOT->SetStyle("Plain");
//    gStyle->SetCanvasBorderMode(0);
//    gStyle->SetFrameBorderMode(0);
//    gStyle->SetPadBorderMode(0);
//    gStyle->SetFrameFillStyle(4000);
//    gStyle->SetFillStyle(4000);
//    gStyle->SetDrawBorder(0);
//    gStyle->SetCanvasBorderSize(0);
//    gStyle->SetFrameBorderSize(0);
//    gStyle->SetPadBorderSize(0);
    gStyle->SetTitleBorderSize(0);

    gStyle->SetStatColor(0);
    gStyle->SetCanvasColor(0);
    gStyle->SetPadColor(0);

//    // Set the size of the default canvas
    gStyle->SetCanvasDefH(400);
    gStyle->SetCanvasDefW(500);
//    gStyle->SetCanvasDefX(10);
//    gStyle->SetCanvasDefY(10);

   // Set Line Widths
//    gStyle->SetFrameLineWidth(1);
    gStyle->SetFuncWidth(1);
//    gStyle->SetHistLineWidth(1);
    gStyle->SetFuncColor(kRed);
    gStyle->SetFuncStyle(2);

//    //Set Marker things for graphs
    gStyle->SetMarkerStyle(29);
    gStyle->SetMarkerColor(9);
    gStyle->SetMarkerSize(0.75);

//    // Set margins -- I like to shift the plot a little up and to the
//    // right to make more room for axis labels
    gStyle->SetPadTopMargin(0.1);
    gStyle->SetPadBottomMargin(0.15);
    gStyle->SetPadLeftMargin(0.12);
    gStyle->SetPadRightMargin(0.05);


//    // Set tick marks and turn off grids
//    gStyle->SetNdivisions(505,"xyz");
   
//    // Set Data/Stat/... and other options
//    gStyle->SetOptDate(0);
//    //gStyle->SetDateX(0.1);
//    //gStyle->SetDateY(0.1);
//    gStyle->SetOptFile(0);
    gStyle->SetOptStat(1110);
    gStyle->SetStatFormat("6.3f");
    gStyle->SetFitFormat("6.3f");
    gStyle->SetOptFit(1);
//    gStyle->SetStatH(0.14);
    gStyle->SetStatStyle(0000);
    gStyle->SetStatW(0.2);
    gStyle->SetStatX(0.95);
    gStyle->SetStatY(0.9);  
    gStyle->SetStatBorderSize(1);
    gStyle->SetStatColor(0);
    gStyle->SetOptTitle(1);
   
//    // Adjust size and placement of axis labels
//    gStyle->SetLabelSize(0.04,"xyz");
//    gStyle->SetLabelOffset(0.01,"x");
//    gStyle->SetLabelOffset(0.01,"y");
//    gStyle->SetLabelOffset(0.005,"z");
//    gStyle->SetTitleSize(0.05,"xyz");
//    gStyle->SetTitleOffset(0.5,"z");
    gStyle->SetTitleOffset(1.3,"y");
   
   
//    gStyle->SetPalette(1);
//    gStyle->SetCanvasColor(0);
    gStyle->SetFrameFillColor(0);
//    //   gStyle->SetFillColor(0);

   
//   //   gStyle->SetPalette(51,0);
//    gStyle->SetTitleH(0.06);
// //   gStyle->SetTitleW(0.3);
    gStyle->SetTitleStyle(0);
// //   gStyle->SetTitleFontSize(0.1);
//    gStyle->SetTextAlign(12);
// //     gStyle->SetTitleBorderSize(0);
// //     gStyle->SetTitleStyle(0); 
// //     gStyle->SetStatX(0.9);
// //     gStyle->SetStatY(0.9);
// //     gStyle->SetStatH(0.2);
// //     gStyle->SetStatW(0.2);
// //     gStyle->SetStatFormat("6.3g");  
// //     gStyle->SetStatBorderSize(1);
// //     gStyle->SetOptStat(1110);
// //     gStyle->SetOptFit(1);
//    //gStyle->SetLabelSize(0.6);
//    //gStyle->SetLabelOffset(0.7);
    gROOT->ForceStyle();
   
}


int getNiceColour(int index)
{
    if(index>10) return index;
    Int_t niceColours[11]={50,38,30,9,8,44,24,12,40,20,41};
    return niceColours[index];
}

int getNiceMarker(int index)
{
    int value=index%6;
    Int_t niceMarkers[6]={22,26,23,28,21,3};
    return niceMarkers[value];
}



unsigned long plotIntervals[NUM_TIME_RANGES]={60*60*6,60,60*10,60*20,60*30};
unsigned long getPlotInterval(AnitaPlotTime_t plotTime) {
    int ind=int(plotTime)-1;
    if(ind<0) ind=0;
    if(ind>4) ind=0;
    return plotIntervals[ind];

}
