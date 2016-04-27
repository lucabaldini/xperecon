
#ifndef TDETECTOR_HH
#define TDETECTOR_HH

#include "TPixy.h"
#include "TInputFile.h"
#include "TCluster_hex.h"
#include "TTree.h"

#define COMB(x1,x2) (((x2 & 0xff)<<8)|x1 & 0xff)

class TDetector {

 private:

  TInputFile *fPixFile, *fPedFile;
  TTree *Evtree;
  MatrixUshort_t fPixMask;
  MatrixUshort_t fBorderPixel;
  MatrixFloat_t fNoiseMatrix;
  Int_t MCflag;
  Int_t counter;
  Float_t WeightLengthScale;
  Float_t SmallCircleRadius;
  Float_t WideCircleRadius;
  Float_t Energy, PolDegree, Delta, Epsilon, X1, Y1, Z1;
  Int_t Counts, VersNum;
  Char_t Mixture[1000];

 public:

  TInputFile *fRawFile;
  MatrixInt_t fPixMap;
  Int_t    Clusterdim;
  Int_t    Channel[1000];
  Int_t    Charge[1000];
  Int_t    DigiCharge[1000];
  Double_t Phi;
  Double_t XI, YI; 
  Int_t Roi[4], numPix;    
  Float_t thr;
  UShort_t time1, time2;
  Double_t timestamp;
  Int_t bufferID;
  MatrixFloat_t PixToCartX;
  MatrixFloat_t PixToCartY;
  MatrixFloat_t fSignalMatrix;
  Adcword_t fRawChannelData[NCHANS];
  Float_t fROIRawData[maxPixTrasm];
  Float_t fPedSubtrSignal[NCHANS];
  Float_t fPedSubtrSignalMask[NCHANS];
  Double_t fCumulativeHitmap[NCHANS];
  Double_t fPeds[NCHANS];
  Double_t fSigma[NCHANS];
  Float_t fPixelThresh;
  Int_t fThreshFlag;
  Int_t fSubPedFlag;
  Float_t fGainNormalization[NCHANS];
  Float_t fRawData[NCHANS];
  Bool_t HeaderInFile;
  TCluster *fAllClusts[MAXNUMCLUSTS];

  TDetector(TInputFile*, TInputFile*, TInputFile*);
  TDetector(TTree *,TInputFile*);
  void CreatePixLookup();
  void ReadPeds();
  Int_t ReadRawFile(); //for very old data
  Int_t ReadMCFile(Int_t);
  Int_t ReadROInew(Int_t); // for new data stream (window mode) with timestamp et al. 
  Int_t ReadROIFile(Int_t); // for data stream in window mode without timestamp et al.
  Int_t ReadFullFrame(Int_t);
  Int_t FindClusters();
  void CheckBaselineJump();
  inline void SetWeight(Float_t Weight) {WeightLengthScale = Weight;}
  inline void SetSmallRadius(Float_t SmallRadius) {SmallCircleRadius = SmallRadius;}
  inline void SetWideRadius(Float_t WideRadius) {WideCircleRadius = WideRadius;}
  inline Int_t GetRcounter() {return counter;}
  inline void SetHeaderOn() {HeaderInFile = true;}
};

#endif
