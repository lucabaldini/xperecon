
#ifndef TMAINGUI_HH
#define TMAINGUI_HH

#include "TPixy.h"
#include "TEventDisplay.h"
#include "TDetector.h"
#include "TCluster_hex.h"
#include "TEditor.h"
#include "TStopwatch.h"

class TMainGUI: public TGMainFrame{
 
 private:
  TGFileInfo fi;
  TString W1, W2, W3, W4, W5, W6, W7, W8;
  TTree            *fTree, *fRawTree, *tree;
  TFile            *PedsAnalizedFile, *DataAnalizedFile, *RawSignalFile, *EventsFile, *fpedsFile;
  TInputFile       *PixFileName, *RawFileName;
  TDetector        *Polarimeter;
  TEventDisplay    *EventDisplay;
  Editor           *ed;

  const TGPicture  *fLogo;
  const TGPicture  *fFileSel;
  const TGPicture  *fUpdateSel;

  TGLabel          *fTitle, *fTitleDataEv, *fTitleDataEv1;
  TGLabel          *fDataThresholdLabel;
  TGLabel          *fTitleWeight, *fTitleSradius, *fTitleWradius;
 
  TCanvas          *fHistosCanvas, *Canvas, *Canvas1, *Canvas0;
  TCanvas          *fCumulativeHitmapCanvas2D;
  
  TGGroupFrame     *fDataFrame;
  TGCompositeFrame *fMainFrameVert, *fMainFrameHor;
  TGCompositeFrame *fRightFrame, *fLeftFrame;
  TGCompositeFrame *fDataThresholdFrame;
  TGCompositeFrame *fDataEvFrame,*fDataEvFrame1, *fWeightFrame, *fSradiusFrame, *fWradiusFrame;
  TGLayoutHints    *fMenuBarLayout, *fMenuBarItemLayout, *fMenuBarHelpLayout;	

  TGLayoutHints    *fButtonLayout;
  TGLayoutHints    *fAnalysisLayout;

  TGButton         *fDisplayEventButton,*fAnalizeDataButton, *fCheck1;
  TGPictureButton  *fUpdatePedButton;
  TGPictureButton  *fUpdateDataButton;
  TGPictureButton  *fPedSel, *fDataSel ;
  TGTextButton     *fFastPedButton;
  TGCheckButton    *fThfix, *fpedsub;
  TGCheckButton    *fHeadToFile;
  TGImageMap       *fLogoImageMap;

  TGTextEntry      *fDataThresholdEntry;
  TGTextEntry      *fDataEvEntry, *fDataEvEntry1;
  TGTextEntry      *fWeightEntry, *fSradiusEntry, *fWradiusEntry;

  TGTextBuffer     *fThresholdBuf;
  TGTextBuffer     *fEvData1;
  TGTextBuffer     *fW1, *fW2, *fW3;
  TGPopupMenu      *fPopupMenu, *fPopupHelp;
  TGMenuBar        *fMenuBar;
  TGMsgBox         *box;
  EMsgBoxIcon      icontype;

  TH1S *RawPedsHistos[NCHANS];
  TH1F *MeanPedsHisto;
  TH1F *RMSPedsHisto;	  
  TH1F *AveragePedsHisto;
  TH1F *SigmaPedsHisto;
  TH1F *noise;
  TF1 *gfit; 
  
  Int_t buttons;
  Int_t retval;
  const char* dir0;

  Int_t ThfixFlag, PedsubFlag;  
  Char_t pName[120], dName[120], nName[120], fName[120];
  Float_t fPixelThreshold;
  Int_t chans;
  UInt_t  fRunId;
  Int_t   fEventId;
  Int_t   fNClusters;
  ULong64_t fTimeTick;
  Double_t fTimeStamp;
  Int_t   fTrigWindow, fbufferId;
  Int_t   fCluSize[MAXNUMCLUSTS];
  Float_t fPHeight[MAXNUMCLUSTS];
  Float_t fStoN[MAXNUMCLUSTS];  
  Float_t fTotNoise[MAXNUMCLUSTS];
  Float_t fHighestC[MAXNUMCLUSTS];
  Float_t fBaricenterX[MAXNUMCLUSTS];
  Float_t fBaricenterY[MAXNUMCLUSTS];
  Float_t fTheta0[MAXNUMCLUSTS];             
  Float_t fTheta1[MAXNUMCLUSTS];   
  Float_t fThetaR[MAXNUMCLUSTS];
  Float_t fImpactXR[MAXNUMCLUSTS];
  Float_t fImpactYR[MAXNUMCLUSTS];
  Float_t fThetaDifference[MAXNUMCLUSTS];
  Float_t fMomX[MAXNUMCLUSTS];
  Float_t fMomY[MAXNUMCLUSTS];  
  Float_t fMomThirdX[MAXNUMCLUSTS];
  Float_t fMomThirdY[MAXNUMCLUSTS];
  Float_t fImpactX[MAXNUMCLUSTS];
  Float_t fImpactY[MAXNUMCLUSTS];
  Float_t fXpixel[MAXNUMCLUSTS][MAXCLUSIZE];
  Float_t fYpixel[MAXNUMCLUSTS][MAXCLUSIZE];
  Float_t fPHpixel[MAXNUMCLUSTS][MAXCLUSIZE];
  Int_t fUpixel[MAXNUMCLUSTS][MAXCLUSIZE];
  Int_t fVpixel[MAXNUMCLUSTS][MAXCLUSIZE];
  Float_t fRawSignal[maxPixTrasm];
  Float_t fCluLenght[MAXNUMCLUSTS];
  Int_t Nevent;
  Int_t Rcounter;
  Int_t Clusterdim;
  Int_t Channel[MAXCLUSIZE];
  Int_t Charge[MAXCLUSIZE];
  Int_t DigiCharge[MAXCLUSIZE];
  Double_t Phi, XInitial, YInitial;
  Int_t RTFlag;
  Bool_t HeaderOn;
  TString workingdir;

  Int_t nchans;
  Int_t totnev;
  Int_t MCflag;
  Int_t RawFlag;
  Int_t NewDataFlag;
  //Int_t FFflag; 
  Float_t Weight, SmallRadius, WideRadius;


  Double_t par[3];
  //TGTextBuffer *fPName, *fDName, *fEvPed;
  TGTextBuffer *fEvData;
  //Text_t *PedName, *DataName;
  TString PedName, DataName, RunIdName;
 
  TStopwatch timer1;
  ofstream pFileName, dFileName, infofile;
  ifstream pUpdatePedFile, pUpdateDataFile; 
  ifstream inPeds;
  ofstream outPeds;
  
  Int_t eofdata;
  Int_t longEv;

 public:

  TMainGUI(const TGWindow *p, UInt_t w, UInt_t h);
  virtual  ~TMainGUI();
  inline Int_t GetNchans() {return maxPixTrasm;}
  virtual Bool_t ProcessMessage(Long_t msg, Long_t par1, Long_t);
  virtual void CloseWindow();
  Int_t GetRunId(TString RunIdName);
  void InitializeClusterTree(TString);
  void InitializeRawSignalTree();
  void InitializeEventsTree();
  void SaveClusters(Int_t, Int_t);
  void ClearArrays(Int_t);
  void SaveRawSignal(Int_t);
  void SaveEventstree(Int_t, Int_t);
  void WriteClusterTree(TString);
  void WriteRawSignalTree();
  void WriteEventsTree();
  void CloseFiles();
  void DataDisplay();
  void DataAnalysis();
  void DataSelect();
  void DrawCumulativeHitMap(Int_t);
  void DataPanelDisable();
  void DataPanelEnable();
  ClassDef(TMainGUI,0);
};

#endif
