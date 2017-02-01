#ifndef TEVENTANALYSIS_HH
#define TEVENTANALYSIS_HH

#include "TPixy.h"
#include "TInputFile.h"
#include "TDetector.h"
#include "TCluster_hex.h"
#include "TTree.h"
#include "TChain.h"
#include "TStopwatch.h"

#include <sstream>

class TEventAnalysis {

 private:
  char*       progName;
  TString     workingDir;
  TString     outputDir;
  TString     rootExt;
  TGFileInfo  fi;
  TList       *dataflist;
  TFile       *DataAnalizedFile, *RawSignalFile, *EventsFile;
  TInputFile  *RawFileName;
  Float_t     Weight, SmallRadius, WideRadius, fPixelThreshold;
  TString     fEvData1;
  Int_t       nchans; 
  Int_t       eofdata, longEv;
  Int_t       Rcounter;
  Int_t       Nevent;
  Int_t       Clusterdim;
  Int_t       RTFlag, MCflag, NewDataFlag, ThfixFlag; 
  UInt_t      fRunId;
  Int_t       fEventId;
  Int_t       fNClusters, fTrigWindow, fbufferId;
  Float_t     fRawSignal[maxPixTrasm];
  Bool_t      HeaderOn;
  TTree       *fTree, *fRawTree, *tree;
  ULong64_t   fTimeTick;
  Double_t    fTimeStamp;
  TStopwatch  timer2;
  TDetector   *Polarimeter;
  Float_t     W1, W2, W3;
  Int_t       W4, W5;
  Double_t    Phi;
  Int_t       Channel[MAXCLUSIZE];
  Int_t       Charge[MAXCLUSIZE];
  Int_t       DigiCharge[MAXCLUSIZE];

  Int_t       ROI[4];
  Int_t       fNBXpixels[MAXNUMCLUSTS];
  Float_t     fPHBXpixels[MAXNUMCLUSTS];
  Int_t       fNBYpixels[MAXNUMCLUSTS];
  Float_t     fPHBYpixels[MAXNUMCLUSTS];

  Int_t       fCluSize[MAXNUMCLUSTS];
  Float_t     fXpixel[MAXNUMCLUSTS][MAXCLUSIZE];
  Float_t     fYpixel[MAXNUMCLUSTS][MAXCLUSIZE];
  Int_t       fUpixel[MAXNUMCLUSTS][MAXCLUSIZE];
  Int_t       fVpixel[MAXNUMCLUSTS][MAXCLUSIZE];
  Float_t     fPHpixel[MAXNUMCLUSTS][MAXCLUSIZE];
  Float_t     fPHeight[MAXNUMCLUSTS];
  Float_t     fStoN[MAXNUMCLUSTS];  
  Float_t     fTotNoise[MAXNUMCLUSTS];
  Float_t     fHighestC[MAXNUMCLUSTS];
  Float_t     fBaricenterX[MAXNUMCLUSTS];
  Float_t     fBaricenterY[MAXNUMCLUSTS];
  Float_t     fTheta0[MAXNUMCLUSTS];             
  Float_t     fTheta1[MAXNUMCLUSTS];   
  Float_t     fThetaDifference[MAXNUMCLUSTS];
  Float_t     fMomX[MAXNUMCLUSTS];
  Float_t     fMomY[MAXNUMCLUSTS];  
  Float_t     fMomThirdX[MAXNUMCLUSTS];
  //Float_t     fMomThirdY[MAXNUMCLUSTS];
  Float_t     fImpactX[MAXNUMCLUSTS];
  Float_t     fImpactY[MAXNUMCLUSTS];
  Float_t     fThetaR[MAXNUMCLUSTS];
  Float_t     fImpactXR[MAXNUMCLUSTS];
  Float_t     fImpactYR[MAXNUMCLUSTS];
  Float_t     fCluLenght[MAXNUMCLUSTS];


 public:
  TEventAnalysis(Int_t, char*, char*, Int_t, Int_t);
  TEventAnalysis();
  virtual  ~TEventAnalysis();
  void Init(Int_t, char*, char*);
  void  DataAnalysis(Int_t, Int_t, Int_t);
  inline void  SetProgName(char* _name) {progName = _name;}
  inline Int_t GetNchans() {return maxPixTrasm;}
  inline void SetDatafilesList(TList* _fileNamesList) {dataflist = _fileNamesList;}
  inline void SetWorkingdir(TString _workingdir) {workingDir = _workingdir;}
  inline void SetDefaults(Float_t _w1, Float_t _w2, Float_t _w3, Int_t _w4, Int_t _w5) {W1 = _w1; W2 = _w2; W3 = _w3; W4 = _w4; W5 = _w5; };
  Int_t GetRunId(TString);
  void  InitializeClusterTree(TString);
  void  InitializeEventsTree();
  void  SaveClusters(Int_t, Int_t);
  void  SaveEventsTree(Int_t, Int_t);
  void  WriteEventsTree();
  void  WriteClusterTree(TString); 
  void  CloseFiles();
  void  ClearArrays(Int_t);
  //ClassDef(TEventAnalysis,0);

};

#endif
