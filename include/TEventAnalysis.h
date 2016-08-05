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
  TString     _progName;
  TString     _workingDir;
  ofstream    infofile;
  TChain      datafchain;
  TString     infofilename, configfilename, dataflistname;
  TGFileInfo fi;
  TList       *dataflist;
  TFile       *DataAnalizedFile, *RawSignalFile, *EventsFile;
  TInputFile  *PixFileName, *RawFileName;
  Double_t    fW1, fW2, fW3, fThresholdBuf;
  bool        fCheck1;
  Float_t     Weight, SmallRadius, WideRadius, fPixelThreshold;
  TString     fEvData1;
  TString     DataName;
  Int_t       nchans; 
  Int_t       eofdata, longEv;
  Int_t       Rcounter;
  Int_t       Nevent;
  Int_t       Clusterdim;
  Int_t       FFflag, RTFlag, RawFlag, MCflag, NewDataFlag; 
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
  Int_t       W4;
  bool        W6, W7;
  Double_t    Phi;
  Int_t       Channel[MAXCLUSIZE];
  Int_t       Charge[MAXCLUSIZE];
  Int_t       DigiCharge[MAXCLUSIZE];
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
  TEventAnalysis(Int_t, char*, char*, Int_t);
  virtual  ~TEventAnalysis();
  void Init(Int_t, char*, char*);
  void  DataAnalysis(Int_t, Int_t);
  inline void  SetProgName(char* _name) {_progName = _name;}
  inline Int_t GetNchans() {return maxPixTrasm;}
  Int_t GetRunId(TString);
  void  InitializeRawSignalTree();
  void  InitializeClusterTree(TString);
  void  InitializeEventsTree();
  void  SaveRawSignal(Int_t);
  void  SaveClusters(Int_t, Int_t);
  void  SaveEventsTree(Int_t, Int_t);
  void  WriteRawSignalTree();
  void  WriteEventsTree();
  void  WriteClusterTree(TString);
  //  void  DrawCumulativeHitMap(Int_t);
  void  CloseFiles();
  void  ClearArrays(Int_t);
  //ClassDef(TEventAnalysis,0);

};

#endif
