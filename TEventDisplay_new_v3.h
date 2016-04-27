#include "TPixy.h"
#include "THexagonCol.h"
#include "TStyle.h"
#include "TMath.h"
#include "TPaletteAxis.h"
#include "TView.h"
#include <TVirtualPadEditor.h>
#define colInt 89

class TEventDisplay : public TGTransientFrame {

private:
  ifstream infodispIn;  
  ofstream infodispOut;
  Int_t BlueFlag;
  Int_t RedFlag;
  Int_t fEventId;
  Int_t fNClusters;
  Double_t fTimeStamp, T0, T1, TSi, TSf;
  Int_t fTrigWindow, fbufferId;
  Int_t fCluSize[MAXNUMCLUSTS];
  Float_t fPHeight[MAXNUMCLUSTS];
  Float_t fStoN[MAXNUMCLUSTS];  
  Float_t fTotNoise[MAXNUMCLUSTS];
  Float_t fHighestC[MAXNUMCLUSTS];
  Float_t fBaricenterX[MAXNUMCLUSTS];
  Float_t fBaricenterY[MAXNUMCLUSTS];
  Float_t fTheta0[MAXNUMCLUSTS];             
  Float_t fTheta1[MAXNUMCLUSTS];   
  Double_t fThetaR[MAXNUMCLUSTS];
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
  Float_t fRawSignal[maxPixTrasm];
  Double_t rawSignal[maxPixTrasm];
  Float_t fCluLenght[MAXNUMCLUSTS];

  Int_t ncluMax, MaxCluster;
  Float_t fData[11];
  Int_t iData[2];
  Axis_t Chans[NCHANS];  
  Int_t buttons;
  Int_t retval;
  Int_t ColScale;
  Int_t RawSignalFlag;
  Int_t EvtNb;
  Float_t ReCenterX, ReCenterY;
  Float_t Shift;
  Int_t NumberOfEvts;
  Float_t  PulseLoThresh,PulseHiThresh,PulseLoThresh1,PulseHiThresh1,NClustLoThresh,NClustHiThresh;
  Float_t StoNLoThresh, StoNHiThresh, CluSizeLoThresh, TrWinLoThresh, TrWinHiThresh;
  Float_t  CluSizeHiThresh, ShapeLoThresh, ShapeHiThresh, M3LoThresh, M3HiThresh;
  Double_t Signal[NCHANS];
  Int_t OrFlag, AndFlag;
  Float_t *ZoomWindow;
  Float_t A0,B0,ModulationFactor0,Angle0,AError0,BError0;
  Float_t ModulationFactorError0,AngleError0,ChiSquare0;
  Float_t A1,B1,ModulationFactor1,Angle1,AError1,BError1;
  Float_t ModulationFactorError1,AngleError1,ChiSquare1;
  Float_t EnergyResolution;
  Float_t MDP;
  Int_t Entries;
  Int_t EvFlag, Cluflag;

  MatrixFloat_t PixmapX;
  MatrixFloat_t PixmapY;
  MatrixUshort_t PixMask;
  MatrixUshort_t PixBorder;

  Int_t   NHits, HighestSignal;
  Float_t ClusterSignal, SignalToNoise;
  Float_t BaryCenterX, BaryCenterY, Phi1, Phi2, PhiR, RealImpactX, RealImpactY, M2Max;
  Float_t M2Min, M3Max, Shape, ImpactX, ImpactY; 
  Float_t SmallCRadius, WideCRadius;

  Float_t GeomXLeftCut,GeomXRightCut,GeomYUpCut,GeomYDownCut;
  Float_t CircleRadiusCut;
  Float_t XEllipseCut, YEllipseCut, R1EllipseCut, R2EllipseCut, ThetaEllipseCut;

  TString num;
  TString fWindowLow, fWindowHigh, OrFlagSt, AndFlagSt;
  TString fCircleRadius, fLeftCut, fRightCut, fUpCut, fDownCut;
  TString fXEllipse, fYEllipse, fR1Ellipse, fR2Ellipse, fThetaEllipse;
  TString fPulseLow, fPulseHigh, fPulseLow1, fPulseHigh1;
  TString fNClustLow, fNClustHigh, fSignalLow, fSignalHigh;
  TString fShapeLow, fShapeHigh, fM3Low, fM3High, fCluSizeLow, fCluSizeHigh;

  TCanvas           *fDisplayCanvas, *fCluHistosCanvas, *fDisplayRawSigCanvas, *newCanvas, *fDisplay2Canvas;
  TColor *color;
  TGGroupFrame      *fEvSelectFrame, *fCluCutFrame, *fCluDisplayFrame, *fBrowsingFrame;
  TGCompositeFrame  *fMainFrame, *fHorFrame; 
  TGVerticalFrame   *fVframe1, *fVframe2;
  TGHorizontalFrame *fHNavigationFrame,*fHNClustCutFrame,*fHPulseCutFrame,*fHPulseCutFrame1,*fHSignalCutFrame, *fHShapeCutFrame;
  TGHorizontalFrame *fHSizeCutFrame, *fHM3CutFrame, *fRadioFrame, *fTrWindowCutFrame;
  TGHorizontalFrame *fHZoomFrame, *fHCircleFrame,*fHGeomXCutFrame, *fHGeomYCutFrame;
  TGHorizontalFrame *fHColScaleFrame;
  TGHorizontalFrame *fHEllipseFrame;


  TGLayoutHints     *fSliderLayout, *fLayerFrame, *fNavigationLayout;
  TGLayoutHints     *fLayerVframe1, *fLayerVframe2;
  TGLayoutHints     *fLayerSignalLowFrame,*fLayerSignalHighFrame ;
  TGLayoutHints     *fLayerLabelFrame;

  TGPictureButton   *fOutSel;
  const TGPicture   *fFileOutSel;

  TGTextEntry       *fText1, *fnumEv;
  TGTextEntry       *fThrPulseLow,*fThrPulseLow1,*fThrPulseHigh,*fThrPulseHigh1,*fThrAmpLow,*fThrNClustHigh;
  TGTextEntry       *fThrNClustLow, *fThrAmpHigh, *fThrShapeLow;
  TGTextEntry       *fThrShapeHigh, *fThrSizeLow, *fThrSizeHigh, *fThrM3Low, *fThrM3High;
  TGTextEntry       *fZoomWindow, *fThrTrWindowLow, *fThrTrWindowHigh;
  TGTextEntry       *fCircleWindow,  *fGeomXLeftCutWindow, *fGeomXRightCutWindow, *fGeomYUpCutWindow, *fGeomYDownCutWindow;
  TGTextEntry       *XelEntry, *YelEntry, *R1elEntry, *R2elEntry, *ThetaelEntry;

  TGTextBuffer      *fEvents;
  TGTextBuffer      *fTbuffer1, *fTrWindowCutLow, *fTrWindowCutHigh;
  TGTextBuffer      *fZoom, *fCircle, *fGeomXLeftCut, *fGeomXRightCut, *fGeomYUpCut, *fGeomYDownCut;
  TGTextBuffer      *fNClustCutHigh,*fNClustCutLow,*fPulseCutHigh,*fPulseCutHigh1,*fPulseCutLow,*fPulseCutLow1,*fThrLow, *fThrHigh, *fShapeCutLow;
  TGTextBuffer      *fShapeCutHigh, *fSizeCutLow, *fSizeCutHigh, *fM3CutLow, *fM3CutHigh;
  TGTextBuffer      *fXelCut, *fYelCut, *fR1elCut, *fR2elCut, *fThetaelCut;

  TGLabel           *fPulseLabel,*fPulseLabel1,*fNClustLabel,*fSignalLabel, *fSizeLabel, *fShapeLabel, *fM3Label;
  TGLabel           *fZoomLabel,*fCircleLabel, *fGeomXLeftCutLabel, *fGeomXRightCutLabel, *fGeomYUpCutLabel, *fGeomYDownCutLabel;
  TGLabel           *fTrWindowLabel, *ColScaleLab, *fTitleDataEv;
  TGLabel           *Xel, *Yel, *R1el, *R2el, *Thetael;
  TGLabel           *fOutLabel;

  TGHSlider         *fHslider;
  TGButton          *fPlotButton, *fPlotCluButton, *fDisplaySigButton, *fDCirclesButton;
  TGTextButton      *fPlotButtonSel;
  TGCheckButton     *ColdBlue,*HotRed;

  TGPictureButton   *fNextEvt, *fPreviousEvt;
  TGRadioButton     *fOr, *fAnd;
  TGMsgBox          *box;
  EMsgBoxIcon       icontype;

  TObjArray         *hexagonList;
  TObjArray         *histList;
  TObjArray         *branchList;

  TFile             *Clusterdatafile, *RawSignalFile;
  TTree             *ClusterTree, *RawTree;
  TBranch           *branchobj;

  TLine             *ProjectedBaryCenterLine;
  TLine             *ProjectedConversionLine;
  TLine             *ReconstructedDirection;
  TEllipse          *WideCircle;
  TEllipse          *SmallCircle;
  THexagonCol       *hexagon;

  TGaxis            *EvtXAxis;
  TGaxis            *EvtYAxis;
  TGaxis            *EvtZoomXAxis;
  TGaxis            *EvtZoomYAxis;

  TLine             *EvtXLine;
  TLine             *EvtYLine;
  TLine             *EvtZoomXLine;
  TLine             *EvtZoomYLine;

  TF1               *PhiFunction1; 
  TF1               *PhiFunction0;
  TF1               *PHeightFunction;
  TH1F              *ProjectionHisto;
  TH1F              *RawSignalHisto;
  TH1F              *fPHeightHisto;
  TH1F              *fStoNHisto;
  TH1F              *fCluSizeHisto;
  TH1F              *fCluNumHisto;
  TH1F              *fThetaHisto;
  TH1F              *fTheta1Histo;
  TH1F              *fM2XtoM2YHisto;
  TH1F              *fThirdMomHisto;
  TH1F              *fThirdMomHistoNorm;
  TH1F              *fTriWindowHisto;
  //TH2F              *fMomXYHisto;
  TH2F              *fCluMapHisto;
  TH2F              *fImpactPtHisto;

  ifstream          EvtDisplayFile; 
  Char_t buf[10];
 public:

  PixelHit fHits[MAXCLUSIZE];
  
  TEventDisplay(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h);
  virtual ~TEventDisplay();  
  virtual void CloseWindow();

  virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  virtual void DisplayData(Int_t EventNumber);
  virtual void InitializePlots();
  Float_t* InitializeZoom(Float_t, Float_t, Float_t, Float_t);
  void InitializeTree();
  void InitializeEntries();
  virtual void  BookHistos();
  void FillHistos(Int_t);
  void WritePolarizationInfo();
  void WriteInfoFile();
  void ReadInfoFile();
  void DrawDirection(Float_t, Float_t, Float_t, Color_t);
  void ReadPixmap();
  void SetPaletteColdBlue();
  void SetPaletteHotRed();
  void SelectAnalisedFile();
  void NextEv();
  void PrevEv();
  void DisplayRawEv();
  void DisplayEv();
  void DrawCircle();
  void SelectEv();
  void BrowseRFile();
  void DrawHistos();
  void ReadGUIbuff();

  ClassDef(TEventDisplay,0);
};


