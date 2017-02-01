#ifndef TPIXY_H
#define TPIXY_H

// Standard Libraries.
#include <stdlib.h>
#include <iomanip>
#include <algorithm>
#include <time.h>
#include <string>
#include "Riostream.h"
using namespace std;

// Root Stuff.
#include <TROOT.h>
#include <TRint.h>
#include <TTree.h>
#include <TPolyMarker.h>
#include <TApplication.h>
#include <TAttText.h>
#include <TText.h>
#include <TPaveText.h>
#include <TGTextEdit.h>
#include <TGTextView.h>
#include <TF1.h>
#include <TGListBox.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGImageMap.h>
#include <TVirtualX.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGDoubleSlider.h>
#include <TGFileDialog.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TRandom.h>
#include <TSystem.h>
#include <TEnv.h>
#include <TGTextView.h>
#include <TFile.h>
#include <TString.h>
#include <TGString.h>
#include <TLine.h>
#include <TMarker.h>
#include <TGaxis.h>
#include <TObjArray.h>
#include <TGTableLayout.h>
#include <TObjectTable.h>
#include <TEllipse.h>
#include <TGFileDialog.h>
#include <TColor.h>
#include <TMath.h>
#include <TPaveStats.h>
// Constants, parameters and other stuff.

#define PIX_R_DIM 352            // Number of readout rows
#define PIX_C_DIM 300            // Number of readout columns
#define PITCH  0.050             // Readout pitch in mm.
#define MAXNUMCLUSTS 200         // Maximum allowed number of clusters in one event.
#define MAXCLUSIZE 400           // Maximum allowed cluster size in one event
#define MINCLUSIZE 6             // Minimum allowed cluster size in one event
#define kSmearingFact 0.00       // Smearing factor for non-interpolated hits.
#define MAX_ADC 2500             // Maximum value for ADC counts. 


//#define kWeightIndex 4.2         // Relevant for Angular reconstruction - iteration 1.
//#define kWeightLengthScale 0.13  //0.05 Ottimo x NeDME 50/50// 0.020 Francesco  Relevant for Angular reconstruction - iteration II
//#define kSmallCircleRadius 0.5   // Gloria 0.5   // 1.5 Francesco  // Relevant for conversion point reconstruction. In units of MomX. 
//#define kWideCircleRadius 2.5    // Relevant for conversion point reconstruction. In units of MomX. 

#define fPixScanRange 1
#define fClusterThresh 10
#define fClusterThreshHi 10000
#define fCluSizeThreshLo 1
#define fCluSizeThreshHi 100
#define fShapeThreshLo 1
#define fShapeThreshHi 1000 

#define kAscii 0
#define kBinary 1
#define kPI 3.14159

#define NODEBUG

const Int_t NCHANS = 105600;
const Int_t CHxCLU = 13200;
const Int_t NCLU = 8;
const Int_t maxPixTrasm = 1195;

// Types definition.
typedef UShort_t Adcword_t;
typedef Float_t MatrixFloat_t[PIX_R_DIM] [PIX_C_DIM];
typedef Int_t MatrixInt_t[PIX_R_DIM][PIX_C_DIM];
typedef UShort_t MatrixUshort_t[PIX_R_DIM][PIX_C_DIM];

enum {HEADON, THFIX, FILE_OPEN, WEXIT, SEL_EV, SLIDER_EV,   
      OR, AND, ABOUT, UPDATEPED, UPDATEDATA, WLEN, BLUE, RED, NUMEV};

#endif
