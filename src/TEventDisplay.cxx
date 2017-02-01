#include "TEventDisplay.h"
#include <fitsio.h>

TEventDisplay::TEventDisplay(const TGWindow *p, const TGWindow *main,
			     UInt_t w, UInt_t h):TGTransientFrame(p, main, w, h)
{ 
  Char_t Dummy[20];
  ifstream infofile;
  Clusterdatafile = 0;
  gStyle->SetOptStat(1);
  workingdir = gSystem->pwd();

  infofn = workingdir +"/info.dat";
  infofile.open(infofn,ios::in);
  infofile >> Dummy  >> SmallCRadius;
  infofile >> Dummy  >>  WideCRadius;
  infofile.close();

  infodispfn = workingdir +"/infodisplay.dat";
  if (!gSystem->AccessPathName(infodispfn, kFileExists)) { // reads last settings of the display control panel
    ReadInfoFile(infodispfn);
  }
  else {
    InitializeEntries();
  }
   
  SelectAnalisedFile();

  fMainFrame = new TGCompositeFrame(this, 300, 300, kHorizontalFrame); 
  //++++ Main Frame ++++
  fVframe1 = new TGVerticalFrame(fMainFrame, 0, 0, 0);

  //++++ LEFT VERTICAL FRAME ++++
  fEvSelectFrame = new TGGroupFrame(fVframe1, new TGString("Event navigator"));
  fHNavigationFrame = new TGHorizontalFrame(fEvSelectFrame, 0, 0, kRaisedFrame);
  fBrowsingFrame = new TGGroupFrame(fVframe1, new TGString("Directory Browsing"));
  fHorFrame = new TGCompositeFrame(fBrowsingFrame, 150, 150, kHorizontalFrame);

  //Slider 
  fHslider = new TGHSlider(fEvSelectFrame, 100, kSlider1 | kScaleBoth, SLIDER_EV);
  fHslider->Associate(this);
  fHslider->SetRange(0,NumberOfEvts-1);
  fHslider->SetPosition(0); 

  //Event navigator 
  Pixel_t green;
  fClient->GetColorByName("green", green);  
  fText1 = new TGTextEntry(fHNavigationFrame, fTbuffer1 = new TGTextBuffer(10),SEL_EV);
  fTbuffer1->AddText(0, "0");
  EvtNb = 0;
  fText1->Associate(this);
  const TGPicture *NextPic;
  const TGPicture *PreviousPic;
  NextPic = fClient->GetPicture("arrow_right.xpm");
  PreviousPic = fClient->GetPicture("arrow_left.xpm");
  fNextEvt = new TGPictureButton(fHNavigationFrame, NextPic);
  fNextEvt->Connect("Clicked()","TEventDisplay",this,"NextEv()");
  fPreviousEvt = new TGPictureButton(fHNavigationFrame, PreviousPic);
  fPreviousEvt->Connect("Clicked()","TEventDisplay",this,"PrevEv()");

  //Display single event
  fPlotButton = new TGTextButton(fEvSelectFrame, "&Display Track");
  fPlotButton->ChangeBackground(green);
  fPlotButton->Connect("Clicked()","TEventDisplay",this,"DisplayEv()");
  fPlotButton->SetToolTipText("Press here to display event");

  //Show selection regions (circles)
  fDCirclesButton = new TGTextButton(fEvSelectFrame, "Display S/W &Circles");
  fDCirclesButton->Connect("Clicked()","TEventDisplay",this,"DrawCircle()");
  fDCirclesButton->SetToolTipText("Display Small/Wide Impact Point research Circles");
  fDCirclesButton->SetState(kButtonDisabled);

  //Zoom factor
  TString fZoomFact;
  fHZoomFrame = new  TGHorizontalFrame(fEvSelectFrame, 0, 0, 0);
  fZoomFact = "4";
  fZoomLabel = new TGLabel(fHZoomFrame, new TGString("Zoom Window"));
  fZoomWindow = new TGTextEntry(fHZoomFrame, fZoom = new TGTextBuffer(10));
  fZoom->AddText(0,fZoomFact);

  //Display event selected through cuts 
  fPlotButtonSel = new TGTextButton(fEvSelectFrame, "&Select Track");
  fPlotButtonSel->Connect("Clicked()","TEventDisplay",this,"SelectEv()");
  fPlotButtonSel->SetToolTipText("Press here to select events which pass all the CUTS");

  //Colored event
  fHColScaleFrame = new  TGHorizontalFrame(fEvSelectFrame, 0, 0, 0);
  ColScaleLab = new TGLabel(fHColScaleFrame, new TGString("Color Scale:"));
  ColdBlue = new TGCheckButton(fHColScaleFrame, "Cold", BLUE);
  ColdBlue->Associate(this);
  ColdBlue->SetState(kButtonUp);
  BlueFlag = 0;
  HotRed = new TGCheckButton(fHColScaleFrame, "Hot", RED);
  HotRed->Associate(this);
  HotRed->SetState(kButtonUp);
  RedFlag = 0;
  
  //Directory browsing (new frame at the bottom of event navigation)
  fFileOutSel = fClient->GetPicture("folder_t.xpm");
  fOutSel = new TGPictureButton(fHorFrame, fFileOutSel);
  fOutSel->Connect("Clicked()","TEventDisplay",this,"BrowseRFile()");
  fTitleDataEv = new TGLabel(fHorFrame, new TGString("     Events:"));
  fnumEv = new TGTextEntry(fHorFrame, fEvents = new TGTextBuffer(10), NUMEV);
  fEvents->AddText(0,"All");
  fnumEv->Associate(this);

  //++++ RIGHT VERTICAL FRAME ++++  
  fVframe2 = new TGVerticalFrame(fMainFrame, 0, 0, 0);
  
  fCluDisplayFrame = new TGGroupFrame(fVframe2, new TGString("Display Clusters Plots"),kVerticalFrame);
  fCluCutFrame = new TGGroupFrame(fCluDisplayFrame, new TGString("Cuts on events"),kVerticalFrame);

  fHPulseCutFrame = new  TGHorizontalFrame(fCluCutFrame, 0, 0, 0);
  fHPulseCutFrame1 = new  TGHorizontalFrame(fCluCutFrame, 0, 0, 0);
  fHSignalCutFrame = new  TGHorizontalFrame(fCluCutFrame, 0, 0, 0);
  fHSizeCutFrame = new  TGHorizontalFrame(fCluCutFrame, 0, 0, 0);
  fHShapeCutFrame = new  TGHorizontalFrame(fCluCutFrame, 0, 0, 0);
  fHNClustCutFrame = new  TGHorizontalFrame(fCluCutFrame, 0, 0, 0);
  fHM3CutFrame = new  TGHorizontalFrame(fCluCutFrame, 0, 0, 0);
  fTrWindowCutFrame = new  TGHorizontalFrame(fCluCutFrame, 0, 0, 0); 
 
  fThrPulseLow = new TGTextEntry(fHPulseCutFrame, fPulseCutLow = new TGTextBuffer(10));
  fPulseCutLow->AddText(0,fPulseLow);
  fPulseLabel = new TGLabel(fHPulseCutFrame, new TGString("Cut PH Out"));
  fThrPulseHigh = new TGTextEntry(fHPulseCutFrame, fPulseCutHigh = new TGTextBuffer(10));
  fPulseCutHigh->AddText(0,fPulseHigh);

  fThrPulseLow1 = new TGTextEntry(fHPulseCutFrame1, fPulseCutLow1 = new TGTextBuffer(10));
  fPulseCutLow1->AddText(0,fPulseLow1);
  fPulseLabel1 = new TGLabel(fHPulseCutFrame1, new TGString("Cut PH In"));
  fThrPulseHigh1 = new TGTextEntry(fHPulseCutFrame1, fPulseCutHigh1 = new TGTextBuffer(10));
  fPulseCutHigh1->AddText(0,fPulseHigh1);

  fThrNClustLow = new TGTextEntry(fHNClustCutFrame, fNClustCutLow = new TGTextBuffer(10));
  fNClustCutLow->AddText(0,fNClustLow);
  fNClustLabel = new TGLabel(fHNClustCutFrame, new TGString("Cluster Num"));
  fThrNClustHigh = new TGTextEntry(fHNClustCutFrame, fNClustCutHigh = new TGTextBuffer(10));
  fNClustCutHigh->AddText(0,fNClustHigh);

  fThrAmpLow = new TGTextEntry(fHSignalCutFrame, fThrLow = new TGTextBuffer(10));
  fThrLow->AddText(0,fSignalLow);
  //fThrAmpLow->SetState(false);
  fSignalLabel = new TGLabel(fHSignalCutFrame, new TGString("Signal/Noise"));
  fThrAmpHigh = new TGTextEntry(fHSignalCutFrame, fThrHigh = new TGTextBuffer(10));
  fThrHigh->AddText(0,fSignalHigh);
  //fThrAmpHigh->SetState(false);

  fThrSizeLow = new TGTextEntry(fHSizeCutFrame, fSizeCutLow = new TGTextBuffer(10));
  fSizeCutLow->AddText(0,fCluSizeLow);
  fSizeLabel = new TGLabel(fHSizeCutFrame, new TGString("Cluster Size"));
  fThrSizeHigh = new TGTextEntry(fHSizeCutFrame, fSizeCutHigh = new TGTextBuffer(10));
  fSizeCutHigh->AddText(0,fCluSizeHigh);

  fThrShapeLow = new TGTextEntry(fHShapeCutFrame, fShapeCutLow = new TGTextBuffer(10));
  fShapeCutLow->AddText(0,fShapeLow);
  fShapeLabel = new TGLabel(fHShapeCutFrame, new TGString("Shape"));
  fThrShapeHigh = new TGTextEntry(fHShapeCutFrame, fShapeCutHigh = new TGTextBuffer(10));
  fShapeCutHigh->AddText(0,fShapeHigh);

  fThrM3Low = new TGTextEntry(fHM3CutFrame, fM3CutLow = new TGTextBuffer(10));
  fM3CutLow->AddText(0,fM3Low);
  fM3Label = new TGLabel(fHM3CutFrame, new TGString("3rd-Mom (M3)"));
  fThrM3High = new TGTextEntry(fHM3CutFrame, fM3CutHigh = new TGTextBuffer(10));
  fM3CutHigh->AddText(0,fM3High);

  fThrTrWindowLow = new TGTextEntry(fTrWindowCutFrame, fTrWindowCutLow = new TGTextBuffer(10));
  fTrWindowCutLow->AddText(0,fWindowLow);
  fTrWindowLabel = new TGLabel(fTrWindowCutFrame, new TGString("Tr. Window"));
  fThrTrWindowHigh = new TGTextEntry(fTrWindowCutFrame, fTrWindowCutHigh = new TGTextBuffer(10));
  fTrWindowCutHigh->AddText(0,fWindowHigh);

  //Radio buttons
  fRadioFrame = new  TGHorizontalFrame(fCluCutFrame, 0, 0, 0);
  fOr = new TGRadioButton(fRadioFrame, "M3 &In Range", OR);
  fOr->Associate(this);
  if(OrFlag == 0)fOr->SetState(kButtonUp);
  else fOr->SetState(kButtonDown);
  //OrFlag = 0;
  fAnd = new TGRadioButton(fRadioFrame, "M3 &Out Range", AND);
  fAnd->Associate(this);
  if(AndFlag == 0)fAnd->SetState(kButtonUp);
  else fAnd->SetState(kButtonDown);
  //AndFlag = 1;

  //======= GEOMETRICAL CUTS =========================================
  //Circle
  fHCircleFrame = new  TGHorizontalFrame(fCluCutFrame, 0, 0, 0);
  fCircleLabel = new TGLabel(fHCircleFrame, new TGString("Circle cut radius of Baricenters"));
  fCircleWindow = new TGTextEntry(fHCircleFrame, fCircle = new TGTextBuffer(10));
  fCircle->AddText(0,fCircleRadius);
   
  //Ellipse
  fHEllipseFrame = new  TGHorizontalFrame(fCluCutFrame, 0, 0, 0);
  Xel = new TGLabel(fHEllipseFrame, new TGString("Xell"));
  XelEntry = new TGTextEntry(fHEllipseFrame, fXelCut = new TGTextBuffer(10));
  fXelCut->AddText(0,fXEllipse);
 
  Yel = new TGLabel(fHEllipseFrame, new TGString("Yell"));
  YelEntry = new TGTextEntry(fHEllipseFrame, fYelCut = new TGTextBuffer(10));
  fYelCut->AddText(0,fYEllipse);

  R1el = new TGLabel(fHEllipseFrame, new TGString("Rmin"));
  R1elEntry = new TGTextEntry(fHEllipseFrame, fR1elCut = new TGTextBuffer(10));
  fR1elCut->AddText(0,fR1Ellipse);

  R2el = new TGLabel(fHEllipseFrame, new TGString("Rmax"));
  R2elEntry = new TGTextEntry(fHEllipseFrame, fR2elCut = new TGTextBuffer(10));
  fR2elCut->AddText(0,fR2Ellipse);

  Thetael = new TGLabel(fHEllipseFrame, new TGString("Theta"));
  ThetaelEntry = new TGTextEntry(fHEllipseFrame, fThetaelCut = new TGTextBuffer(10));
  fThetaelCut->AddText(0,fThetaEllipse);
  
  //Square
  fHGeomXCutFrame = new  TGHorizontalFrame(fCluCutFrame, 0, 0, 0);
  fGeomXLeftCutLabel = new TGLabel(fHGeomXCutFrame, new TGString("Left Cut"));
  fGeomXLeftCutWindow = new TGTextEntry(fHGeomXCutFrame, fGeomXLeftCut = new TGTextBuffer(10));
  fGeomXLeftCut->AddText(0,fLeftCut);
  fGeomXRightCutLabel = new TGLabel(fHGeomXCutFrame, new TGString("Right Cut"));
  fGeomXRightCutWindow = new TGTextEntry(fHGeomXCutFrame, fGeomXRightCut = new TGTextBuffer(10));
  fGeomXRightCut->AddText(0,fRightCut);

  fHGeomYCutFrame = new  TGHorizontalFrame(fCluCutFrame, 0, 0, 0);
  fGeomYUpCutLabel = new TGLabel(fHGeomYCutFrame, new TGString("Up Cut"));
  fGeomYUpCutWindow = new TGTextEntry(fHGeomYCutFrame, fGeomYUpCut = new TGTextBuffer(10));
  fGeomYUpCut->AddText(0,fUpCut);
  fGeomYDownCutLabel = new TGLabel(fHGeomYCutFrame, new TGString("Down Cut"));
  fGeomYDownCutWindow = new TGTextEntry(fHGeomYCutFrame, fGeomYDownCut = new TGTextBuffer(10));
  fGeomYDownCut->AddText(0,fDownCut);

  //========================= Start analysis button =====================
  fPlotCluButton = new TGTextButton(fCluCutFrame, "Apply &Cuts & Draw");
  fPlotCluButton->ChangeBackground(green);
  fPlotCluButton->Connect("Clicked()","TEventDisplay",this,"DrawHistos()");
  fPlotCluButton->SetToolTipText("Press here to draw histos");
  
  //LAYOUTS
  //--- layout for slider: top align, equally expand horizontally
  fSliderLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 0, 0, 4, 4);
  fNavigationLayout = new TGLayoutHints(kLHintsCenterY | kLHintsCenterX | kLHintsExpandX, 4, 4, 4, 4);

  
  //--- layout for the frame: place at bottom, right aligned
  fLayerFrame = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 4, 4, 4, 4);
  fLayerVframe1 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 8, 0, 0);
  fLayerVframe2 = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 0, 0, 0, 0);
  
  fLayerSignalLowFrame = new TGLayoutHints(kLHintsTop | kLHintsLeft| kLHintsExpandX, 0, 8, 0, 0);
  fLayerSignalHighFrame = new TGLayoutHints(kLHintsTop | kLHintsRight | kLHintsExpandX, 4, 0, 0, 0);
  fLayerLabelFrame = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 0, 0, 0, 0);
   
  //Compose frames
  fHorFrame->AddFrame(fOutSel, new TGLayoutHints(kLHintsLeft| kLHintsTop, 4, 4, 4, 4));
  fHorFrame->AddFrame(fTitleDataEv, new TGLayoutHints(kLHintsLeft| kLHintsTop, 4, 4, 4, 4));
  fHorFrame->AddFrame(fnumEv, new TGLayoutHints(kLHintsLeft| kLHintsTop, 4, 4, 4, 4)); 
 
  fHZoomFrame->AddFrame(fZoomLabel,new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
  fHZoomFrame->AddFrame(fZoomWindow,new TGLayoutHints(kLHintsTop | kLHintsRight | kLHintsExpandX, 0, 2, 0, 0));
 
  fHColScaleFrame->AddFrame(ColScaleLab,new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
  fHColScaleFrame->AddFrame(ColdBlue,new TGLayoutHints(kLHintsTop | kLHintsRight, 0, 4, 0, 0));
  fHColScaleFrame->AddFrame(HotRed,new TGLayoutHints(kLHintsTop | kLHintsRight | kLHintsExpandX, 0, 2, 0, 0));
 
  fHNavigationFrame->AddFrame(fPreviousEvt, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 2, 2, 2));
  fHNavigationFrame->AddFrame(fText1, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY, 1, 2, 2, 2));
  fHNavigationFrame->AddFrame(fNextEvt, new TGLayoutHints(kLHintsRight | kLHintsCenterY, 2, 2, 2, 2 ));

  fEvSelectFrame->AddFrame(fHslider, fSliderLayout);
  fEvSelectFrame->AddFrame(fHNavigationFrame, fSliderLayout);
  fEvSelectFrame->AddFrame(fPlotButton, fSliderLayout);
  fEvSelectFrame->AddFrame(fDCirclesButton, fSliderLayout);
  fEvSelectFrame->AddFrame(fHZoomFrame, fSliderLayout);
  fEvSelectFrame->AddFrame(fPlotButtonSel, fSliderLayout);
  fEvSelectFrame->AddFrame(fHColScaleFrame, fSliderLayout);
   
  fBrowsingFrame->AddFrame(fHorFrame, fSliderLayout);
 
  fVframe1->AddFrame(fEvSelectFrame, fSliderLayout);
  fVframe1->AddFrame(fBrowsingFrame, fSliderLayout); 

  ////////////////////////////
  
  fHPulseCutFrame->AddFrame(fThrPulseLow, fLayerSignalLowFrame);
  fHPulseCutFrame->AddFrame(fPulseLabel, fLayerLabelFrame);
  fHPulseCutFrame->AddFrame(fThrPulseHigh, fLayerSignalHighFrame);

  fHPulseCutFrame1->AddFrame(fThrPulseLow1, fLayerSignalLowFrame);
  fHPulseCutFrame1->AddFrame(fPulseLabel1, fLayerLabelFrame);
  fHPulseCutFrame1->AddFrame(fThrPulseHigh1, fLayerSignalHighFrame);

  fHNClustCutFrame->AddFrame(fThrNClustLow, fLayerSignalLowFrame);
  fHNClustCutFrame->AddFrame(fNClustLabel, fLayerLabelFrame);
  fHNClustCutFrame->AddFrame(fThrNClustHigh, fLayerSignalHighFrame);

  fHSignalCutFrame->AddFrame(fThrAmpLow, fLayerSignalLowFrame);
  fHSignalCutFrame->AddFrame(fSignalLabel, fLayerLabelFrame);
  fHSignalCutFrame->AddFrame(fThrAmpHigh, fLayerSignalHighFrame);

  fHSizeCutFrame->AddFrame(fThrSizeLow, fLayerSignalLowFrame);
  fHSizeCutFrame->AddFrame(fSizeLabel, fLayerLabelFrame);
  fHSizeCutFrame->AddFrame(fThrSizeHigh, fLayerSignalHighFrame);
  
  fHShapeCutFrame->AddFrame(fThrShapeLow, fLayerSignalLowFrame);
  fHShapeCutFrame->AddFrame(fShapeLabel, fLayerLabelFrame);
  fHShapeCutFrame->AddFrame(fThrShapeHigh, fLayerSignalHighFrame);

  fHM3CutFrame->AddFrame(fThrM3Low, fLayerSignalLowFrame);
  fHM3CutFrame->AddFrame(fM3Label, fLayerLabelFrame);
  fHM3CutFrame->AddFrame(fThrM3High, fLayerSignalHighFrame);
  
  fTrWindowCutFrame->AddFrame(fThrTrWindowLow, fLayerSignalLowFrame);
  fTrWindowCutFrame->AddFrame(fTrWindowLabel, fLayerLabelFrame);
  fTrWindowCutFrame->AddFrame(fThrTrWindowHigh, fLayerSignalHighFrame); 

  fRadioFrame->AddFrame(fOr, fLayerLabelFrame);
  fRadioFrame->AddFrame(fAnd, fLayerLabelFrame);

  fHCircleFrame->AddFrame(fCircleLabel,new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
  fHCircleFrame->AddFrame(fCircleWindow,new TGLayoutHints(kLHintsTop | kLHintsRight | kLHintsExpandX, 0, 2, 0, 0));

  fHEllipseFrame->AddFrame(Xel, fLayerLabelFrame);
  fHEllipseFrame->AddFrame(XelEntry, fLayerSignalLowFrame);
  fHEllipseFrame->AddFrame(Yel, fLayerLabelFrame);
  fHEllipseFrame->AddFrame(YelEntry, fLayerSignalLowFrame);
  fHEllipseFrame->AddFrame(R1el, fLayerLabelFrame);
  fHEllipseFrame->AddFrame(R1elEntry, fLayerSignalLowFrame);
  fHEllipseFrame->AddFrame(R2el, fLayerLabelFrame);
  fHEllipseFrame->AddFrame(R2elEntry, fLayerSignalLowFrame);
  fHEllipseFrame->AddFrame(Thetael, fLayerLabelFrame);
  fHEllipseFrame->AddFrame(ThetaelEntry, fLayerSignalLowFrame);

  fHGeomXCutFrame->AddFrame(fGeomXLeftCutLabel, fLayerLabelFrame);
  fHGeomXCutFrame->AddFrame(fGeomXLeftCutWindow, fLayerSignalLowFrame);
  fHGeomXCutFrame->AddFrame(fGeomXRightCutLabel, fLayerLabelFrame);
  fHGeomXCutFrame->AddFrame(fGeomXRightCutWindow, fLayerSignalHighFrame);

  fHGeomYCutFrame->AddFrame(fGeomYUpCutLabel, fLayerLabelFrame);
  fHGeomYCutFrame->AddFrame(fGeomYUpCutWindow, fLayerSignalLowFrame);
  fHGeomYCutFrame->AddFrame(fGeomYDownCutLabel, fLayerLabelFrame);
  fHGeomYCutFrame->AddFrame(fGeomYDownCutWindow, fLayerSignalHighFrame);

  fCluCutFrame->AddFrame(fHPulseCutFrame, fSliderLayout);
  fCluCutFrame->AddFrame(fHPulseCutFrame1, fSliderLayout);
  fCluCutFrame->AddFrame(fHNClustCutFrame, fSliderLayout);
  fCluCutFrame->AddFrame(fHSignalCutFrame, fSliderLayout);
  fCluCutFrame->AddFrame(fHSizeCutFrame, fSliderLayout);
  fCluCutFrame->AddFrame(fHShapeCutFrame, fSliderLayout);
  fCluCutFrame->AddFrame(fHM3CutFrame, fSliderLayout);
  fCluCutFrame->AddFrame(fTrWindowCutFrame, fSliderLayout);
  fCluCutFrame->AddFrame(fRadioFrame, fSliderLayout);
  fCluCutFrame->AddFrame(fHCircleFrame, fSliderLayout);
  fCluCutFrame->AddFrame(fHEllipseFrame, fSliderLayout);
  fCluCutFrame->AddFrame(fHGeomXCutFrame, fSliderLayout);
  fCluCutFrame->AddFrame(fHGeomYCutFrame, fSliderLayout);
  fCluCutFrame->AddFrame(fPlotCluButton, new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 0, 0, 4, 0));

  fCluDisplayFrame->AddFrame(fCluCutFrame, new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 0, 0, 4, 0));
  fVframe2->AddFrame(fCluDisplayFrame, new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 0, 0, 4, 0));
  

  fMainFrame->AddFrame(fVframe1, fLayerVframe1);
  fMainFrame->AddFrame(fVframe2, fLayerVframe2);
  AddFrame(fMainFrame, fLayerFrame);
  Resize(700,500);
 
  MapSubwindows();
  SetWindowName("Pixy Data Display (v6.1)");
  MapWindow();
  Move(100,400);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEventDisplay::~TEventDisplay()
{
  // Delete dialog.
  SetCleanup(kDeepCleanup);
  Cleanup();
  /*
    histList->Delete();
    delete histList;
    delete EvtZoomYAxis;
    delete EvtZoomXAxis;
    delete hexagon;
    delete EvtYLine;
    delete EvtXLine;
    delete EvtYAxis;
    delete EvtXAxis;
    delete ProjectedConversionLine;
    delete ProjectedBaryCenterLine;
    delete ReconstructedDirection;
    delete ProjectionHisto;
    delete SmallCircle;
    delete WideCircle;
    delete PhiFunction1;
    delete PhiFunction0;
    delete PHeightFunction;
    delete branchList;
    delete Clusterdatafile;
    delete fLayerLabelFrame;
    delete fLayerSignalHighFrame;
    delete fLayerSignalLowFrame;
    delete fLayerVframe2;
    delete fLayerVframe1;
    delete fLayerFrame;
    delete fNavigationLayout;
    delete fSliderLayout;
    delete fPlotCluButton;
    delete fAnd;
    delete fOr;
    delete fThrM3High;
    delete fM3Label;
    delete fThrM3Low;
    delete fThrShapeHigh;
    delete fShapeLabel;
    delete fThrShapeLow;
    delete fThrSizeHigh;
    delete fSizeLabel;
    delete fThrSizeLow;
    delete fThrAmpHigh;
    delete fSignalLabel;
    delete fThrAmpLow;
    delete fThrNClustHigh;
    delete fNClustLabel;
    delete fThrNClustLow;
    delete fThrPulseHigh;
    delete fPulseLabel;
    delete fThrPulseLow;
    delete fCircleWindow;
    delete fCircleLabel;
    delete fHCircleFrame;
    delete fGeomXLeftCutWindow;
    delete fGeomXRightCutWindow;
    delete fGeomYUpCutWindow;
    delete fGeomYDownCutWindow;
    delete fGeomXLeftCutLabel;
    delete fGeomXRightCutLabel;
    delete fGeomYUpCutLabel;
    delete fGeomYDownCutLabel;
    delete fHGeomXCutFrame;
    delete fHGeomYCutFrame;
    delete fZoomWindow;
    delete fZoomLabel;
    delete fHZoomFrame;
    delete fDisplaySigButton;
    delete fPlotButton;
    delete fPreviousEvt;
    delete fNextEvt;
    delete fHslider;
    delete fText1;
    delete fRadioFrame; 
    delete fHM3CutFrame;
    delete fHNClustCutFrame; 
    delete fHShapeCutFrame;
    delete fHSizeCutFrame; 
    delete fHSignalCutFrame;
    delete fHPulseCutFrame;
    delete fHPulseCutFrame1;
    delete fHNavigationFrame;
    delete fCluCutFrame;
    delete fCluDisplayFrame;
    delete fEvSelectFrame;
    delete fVframe2;
    delete fVframe1;
    delete fMainFrame;

    if (gROOT->GetListOfCanvases()->FindObject(fDisplayCanvas))delete fDisplayCanvas;
    if (gROOT->GetListOfCanvases()->FindObject(fCluHistosCanvas))delete fCluHistosCanvas;

    //if (Clusterdatafile)  Clusterdatafile->Close();
    */
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TEventDisplay::CloseWindow()
{
  // Called when window is closed via the window manager.
  DeleteWindow();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Bool_t TEventDisplay::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{ 
  // Process slider messages.
  switch (GET_MSG(msg)) {

  case kC_TEXTENTRY:
    switch (GET_SUBMSG(msg)) {
    case kTE_TEXTCHANGED:
      switch (parm1) {	
      case SEL_EV:
	EvtNb = atoi(fTbuffer1->GetString());
	fHslider->SetPosition(EvtNb);
	break;
		     
      }
      break;
    case kTE_ENTER:
      switch (parm1) {
      case NUMEV:
	Int_t evMax = (Int_t) ClusterTree->GetEntries();
	NumberOfEvts = atoi(fEvents->GetString());
	if(NumberOfEvts<evMax){
	  fHslider->SetRange(0,NumberOfEvts-1);
	  fHslider->SetPosition(0);  
	  fTbuffer1->Clear();
	  fTbuffer1->AddText(0, "0");
	  fClient->NeedRedraw(fText1);
	}
	else {
	  box = new TGMsgBox(gClient->GetRoot(), gClient->GetRoot(),"Message", 
			     "Execeeded events in file!", kMBIconExclamation, kMBClose, &retval);
	  fHslider->SetRange(0,evMax-1);
	  fHslider->SetPosition(0);  
	  fTbuffer1->Clear();
	  fTbuffer1->AddText(0, "0");
	  fClient->NeedRedraw(fText1);
	  fEvents->Clear();
	  TString num = "";
	  num += evMax;
	  fEvents->AddText(0, num);
	  fClient->NeedRedraw(fnumEv);
	  NumberOfEvts = evMax;
	}
	break;
      }
    }
    break;

  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {
    case kCM_CHECKBUTTON:
      switch (parm1) {
      case BLUE:
	if(ColdBlue->GetState()){
	  if(RedFlag){ //delete hot red palette
	    //for (Int_t i=0; i<colInt-1; i++)delete((TColor*)gROOT->GetColor(501+i)); 
	    HotRed->SetState(kButtonUp);
	    RedFlag = 0;
	  }
	  for (Int_t i=0; i<colInt-1; i++)delete((TColor*)gROOT->GetColor(501+i)); 
	  BlueFlag = 1;
	  cout << "Cold Blue palette selected!! " << endl;
	  SetPaletteColdBlue();
	}
	else{
	  if(RedFlag == 0) BlueFlag = 0;  // both BLUE and RED button unchecked
	}
	break;
	
      case RED:
	if(HotRed->GetState()){
	  if(BlueFlag){//delete cold blue palette 
	    //for (Int_t i=0; i<colInt-1; i++)delete((TColor*)gROOT->GetColor(501+i)); 
	    ColdBlue->SetState(kButtonUp);
	    BlueFlag = 0;
	  }
	  RedFlag = 1;
	  for (Int_t i=0; i<colInt-1; i++)delete((TColor*)gROOT->GetColor(501+i)); 
	  cout << "Hot Red palette selected!! " << endl;
	  SetPaletteHotRed();
	}
	else {
	  if(BlueFlag == 0 ) RedFlag = 0; // both BLUE and RED button unchecked
	}
	break;
      }
      break;

    case kCM_RADIOBUTTON:
      switch (parm1) {
      case OR:
	fAnd->SetState(kButtonUp);
	OrFlag = 1;
	AndFlag = 0; 
	break;
	
      case AND:
	fOr->SetState(kButtonUp);
	OrFlag = 0;
	AndFlag = 1;
	break;
      }
      break;
    }
    break;
    
  case kC_HSLIDER:
    switch (GET_SUBMSG(msg)) {
    case kSL_POS:
      EvtNb = (Int_t)parm2;
      sprintf(buf, "%d", EvtNb);
      switch (parm1) {
      case SLIDER_EV:
	fTbuffer1->Clear();
	fTbuffer1->AddText(0, buf);
	fClient->NeedRedraw(fText1);
	break;		    
      }
      break;
    }
    break;
  default:
    break;
  }
  return kTRUE;
  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::DisplayData(Int_t EventNumber)
{   
  Float_t MinimumX = 1000.0, MaximumX = -1000.0, MinimumY = 1000.0, MaximumY = -1000.0;
  Int_t MaxClus = 0;
  Int_t   npixels, nclusters;
  Float_t ZoomFactor;
  Int_t DEBUG = 0;
  Int_t buttons = 0;
  Int_t retval;
  Int_t Rflag = 1;
  Int_t col = 0;
  Float_t PHrange[colInt];
  Float_t phMax, phMin,dPH; 
  EMsgBoxIcon icontype = kMBIconExclamation;
  PixelHit pixel;

  if(ProjectionHisto){
    ProjectionHisto=0;
    delete ProjectionHisto;
  }

  ClusterSignal = SignalToNoise = 0.;
  BaryCenterX = BaryCenterY = 0;
  Phi1 = Phi2 = PhiR = 0.;
  M2Max = M2Min = 0.;
  Shape = M3Max = 0.;
  ImpactX = ImpactY = 0.;
  fDCirclesButton->SetState(kButtonUp);
  
  if(EventNumber>=NumberOfEvts) {
    box = new TGMsgBox(gClient->GetRoot(), gClient->GetRoot(),"Error Message", 
		       Form("No such event in ClustersData.root(tot ev =%d)",NumberOfEvts),icontype, buttons, &retval);
    fDisplayCanvas->Clear();
    fDisplayCanvas->Update();
    return;
  }   
  ClusterTree->GetEntry(EventNumber);

  if(!fNClusters) {
    cout << "no clusters in the selected event n:" << EventNumber << endl;	      
    return;
  } 

  // Display the event.
  fDisplayCanvas->cd(1);   // <<<===============   PAD1
  for (nclusters=0; nclusters<fNClusters; nclusters++){	 
    NHits = fCluSize[nclusters];
    HighestSignal = (Int_t)fHighestC[nclusters];
    if (MaxClus < NHits) { // ===>>> SELECT the highest clusters in terms of number of hits
      MaxClus = NHits;
      ClusterSignal = fPHeight[nclusters];
      SignalToNoise = fStoN[nclusters];
      BaryCenterX = fBaricenterX[nclusters];
      BaryCenterY = fBaricenterY[nclusters];
      Phi1 = fTheta0[nclusters];             
      Phi2 = fTheta1[nclusters];
      PhiR = fThetaR[nclusters];
      RealImpactX = fImpactXR[nclusters];
      RealImpactY = fImpactYR[nclusters];
      M2Max  = fMomX[nclusters];
      M2Min = fMomY[nclusters];
      M3Max = fMomThirdX[nclusters];
      ImpactX = fImpactX[nclusters];
      ImpactY = fImpactY[nclusters];     
      if (M2Min) Shape = M2Max/M2Min;
      else Shape = 0;
      if (DEBUG){
	cout << "ev:  " << EventNumber << "  clunum:  " << nclusters << " clusize: "  << NHits << endl;
	for (Int_t j=0; j< NHits; j++)
	  cout << "pixel height:  " << fPHpixel[nclusters][j] << "  x - pixel :  " << fXpixel[nclusters][j] << endl;
      }
      ZoomFactor = atof(fZoom->GetString());
      if (ZoomFactor <= 0) ZoomFactor = 1;
      //cout << "ZOOM:    =====>>> " << ZoomFactor << endl;
      ZoomFactor = ZoomFactor*TMath::Power((double)M2Max,(double)0.5);
      MinimumX = BaryCenterX - ZoomFactor;
      MaximumX = BaryCenterX + ZoomFactor;
      MinimumY = BaryCenterY - ZoomFactor;
      MaximumY = BaryCenterY + ZoomFactor;
    }
    for (npixels=0; npixels<NHits; npixels++){
      pixel.X = fXpixel[nclusters][npixels];
      pixel.Y = fYpixel[nclusters][npixels];
      pixel.Height = fPHpixel[nclusters][npixels];
      pixel.Height = pixel.Height/HighestSignal*PITCH/2;
      hexagon = new THexagonCol(pixel);
      hexagon->DrawEmpty(kBlue);
    }
  } 
  ReCenterX = 0;
  ReCenterY = 0;  
  // =========>>>>>>>>> Zoom the event.
  // Initialize the projection histogram first.
  if (gROOT->FindObject("Event Projection")){
    delete gROOT->FindObject("Event Projection");
  }
  Float_t ProjectedBaryCenter = cos(Phi1)*(BaryCenterX+ReCenterX)+sin(Phi1)*(BaryCenterY+ReCenterY);
  Float_t ProjectedConversion = cos(Phi1)*(ImpactX+ReCenterX)+sin(Phi1)*(ImpactY+ReCenterY);
  Int_t ProjectionHistoMin = (Int_t)(ProjectedBaryCenter - 4*TMath::Sqrt(M2Max) - 1);
  Int_t ProjectionHistoMax = (Int_t)(ProjectedBaryCenter + 4*TMath::Sqrt(M2Max) + 1);
  ProjectionHisto = new TH1F("Event Projection", "Event Projection", 20, ProjectionHistoMin, ProjectionHistoMax);
  Float_t projMax = -999999;
  Float_t projMin = 999999;
  // Go on with the zoom;
  fDisplayCanvas->cd(2); // <<<===============   PAD2
  ZoomWindow = InitializeZoom(MinimumX, MaximumX, MinimumY, MaximumY);  
  for (nclusters=0; nclusters<fNClusters; nclusters++){
    for (npixels=0; npixels<fCluSize[nclusters]; npixels++){
      pixel.X = fXpixel[nclusters][npixels];
      pixel.Y = fYpixel[nclusters][npixels];
      pixel.Height = fPHpixel[nclusters][npixels];
      pixel.X += ReCenterX;
      pixel.Y += ReCenterY;
      Float_t Projection = (cos(Phi1)*pixel.X+sin(Phi1)*pixel.Y);
      if(Projection>projMax) projMax = Projection;
      if(Projection<projMin) projMin = Projection;
      ProjectionHisto->Fill(Projection, pixel.Height);
      if (pixel.X > MinimumX && pixel.X < MaximumX && pixel.Y > MinimumY && pixel.Y < MaximumY){
	pixel.Height = TMath::Sqrt(pixel.Height/fHighestC[nclusters])*PITCH/2;
	hexagon = new THexagonCol(pixel);
	hexagon->Draw(kBlue);
      }
    }
    //    cout << "=====>>>>> " << projMax - projMin << "   ---  " << fCluLenght[nclusters]<<  endl;
  }

  //Draw ROI
  float xmin,ymin,xmax,ymax;
 
  xmin = PixmapX[ROI[2]][ROI[0]];
  ymin = PixmapY[ROI[2]][ROI[0]];
  xmax = PixmapX[ROI[3]][ROI[1]];
  ymax = PixmapY[ROI[3]][ROI[1]];
  
  if(fNBXpixels[0]>0||fNBYpixels[0]>0){ 
    window->DrawLine(xmin,ymin,xmin,ymax);
    window->DrawLine(xmin,ymax,xmax,ymax);
    window->DrawLine(xmax,ymax,xmax,ymin);
    window->DrawLine(xmax,ymin,xmin,ymin);
  }

  Float_t x, y;
  if(PhiR && Rflag) {
    x = RealImpactX*10+ReCenterX; // RealImpactX/Y from MC in cm must be converted in mm
    y = RealImpactY*10+ReCenterY;
    DrawDirection(x,y,PhiR,kGreen);  // Real Impact Point & emission direction (from MC)
  }
  if(Phi1) {
    x = BaryCenterX+ReCenterX;
    y = BaryCenterY+ReCenterY;
    DrawDirection(x,y,Phi1,kBlack);   // Reconstructed Baricenter & emission direction
  }
  if(Phi2) {
    x = ImpactX+ReCenterX;
    y = ImpactY+ReCenterY;
    DrawDirection(x,y,Phi2,kRed);    // Reconstructed Impact Point & emission direction (II iteration)
  } 
    
  // Draw the projection of the event on the principal axis.
  fDisplayCanvas->cd(3);  // <<<===============   PAD3
  Float_t YMin = ProjectionHisto->GetMinimum();
  Float_t YMax = 1.05*ProjectionHisto->GetMaximum();
  ProjectedBaryCenterLine = new TLine(ProjectedBaryCenter, YMin, ProjectedBaryCenter, YMax);
  ProjectedConversionLine = new TLine(ProjectedConversion, YMin, ProjectedConversion, YMax);
  ProjectedBaryCenterLine->SetLineColor(kBlack);
  ProjectedConversionLine->SetLineColor(kRed);
  ProjectedBaryCenterLine->SetLineStyle(2);
  ProjectedConversionLine->SetLineStyle(2);
  ProjectionHisto->Draw();
  ProjectedBaryCenterLine->Draw();
  ProjectedConversionLine->Draw();  

  // Add event informations.
  fDisplayCanvas->cd(4);  // <<<===============   PAD4
  Char_t DummyChar[20];
  TText *t = new TText();
  TText *w = new TText();
  w->SetTextColor(2);
  t->SetTextSize(0.04);
  w->SetTextSize(0.04);
  t->DrawText(0.1,0.90,"Event Number:");
  sprintf(DummyChar, "%d", EvtNb);
  w->DrawText(0.7,0.90, DummyChar);
  t->DrawText(0.1,0.85,"Number of Clusters:");
  sprintf(DummyChar, "%d", fNClusters);
  w->DrawText(0.7,0.85, DummyChar);
  if (fNClusters) {
    t->DrawText(0.1,0.80,"Cluster Size (largest):");
    sprintf(DummyChar, "%d", MaxClus);
    w->DrawText(0.7,0.80, DummyChar);
    t->DrawText(0.1,0.75,"Pulse Height:");
    sprintf(DummyChar, "%6.1f",ClusterSignal);
    w->DrawText(0.7,0.75, DummyChar);
    t->DrawText(0.1,0.70,"Signal to Noise:");
    sprintf(DummyChar, "%4.1f",SignalToNoise);
    w->DrawText(0.7,0.70, DummyChar);
    t->DrawText(0.1,0.65,"Baricenter:");
    sprintf(DummyChar, "%2.2f   %2.2f", BaryCenterX+ReCenterX, BaryCenterY+ReCenterY);
    w->DrawText(0.7,0.65, DummyChar);
    t->DrawText(0.1,0.60,"Conversion Point:");
    sprintf(DummyChar, "%2.2f   %2.2f", ImpactX+ReCenterX, ImpactY+ReCenterY);
    w->DrawText(0.7,0.60, DummyChar);
    t->DrawText(0.1,0.55,"Second Mom Max:");
    sprintf(DummyChar, "%2.4f", M2Max);
    w->DrawText(0.7,0.55, DummyChar);
    t->DrawText(0.1,0.50,"Second Mom Min:");
    sprintf(DummyChar, "%2.4f", M2Min);
    w->DrawText(0.7,0.50, DummyChar);
    t->DrawText(0.1,0.45,"Shape (ratio of moments):");
    sprintf(DummyChar, "%2.2f", Shape);
    w->DrawText(0.7,0.45, DummyChar);
    t->DrawText(0.1,0.40,"Third Mom Max:");
    sprintf(DummyChar, "%1.1e", M3Max);
    w->DrawText(0.7,0.40, DummyChar);
    t->DrawText(0.1,0.35,"Phi (iteration 1)");
    sprintf(DummyChar, "%2.4f", Phi1);
    w->DrawText(0.7,0.35, DummyChar);
    t->DrawText(0.1,0.30,"Phi (iteration 2)");
    sprintf(DummyChar, "%2.4f", Phi2);
    w->DrawText(0.7,0.30, DummyChar);
    if(PhiR && Rflag){
      t->DrawText(0.1,0.25,"PhiReal");
      sprintf(DummyChar, "%2.4f", PhiR);
      w->DrawText(0.7,0.25, DummyChar);
    }
    w->SetTextColor(1);
    TMarker *center;
    center = new TMarker(0.1,0.20 , 2);
    center->SetMarkerColor(kBlack);
    center->SetMarkerStyle(28);
    center->SetMarkerSize(1.3);
    center->Draw();
    w->DrawText(0.2,0.20, "Reconstructed Baricenter");
    center = new TMarker(0.1,0.15 , 2);
    center->SetMarkerColor(kRed);
    center->SetMarkerStyle(28);
    center->SetMarkerSize(1.3);
    center->Draw();
    w->DrawText(0.2,0.15, "Reconstructed Impact Pt.");
    if(PhiR && Rflag){ 
      TMarker *center;
      center = new TMarker(0.1,0.10 , 2);
      center->SetMarkerColor(kGreen);
      center->SetMarkerStyle(28);
      center->SetMarkerSize(1.3);
      center->Draw();
      w->DrawText(0.2,0.10, "Real Impact Pt.");
    }
  }

  if (RedFlag==1 || BlueFlag==1){
    if(!(gROOT->GetListOfCanvases()->FindObject(fDisplay2Canvas)))
      fDisplay2Canvas = new TCanvas("Colorevent", "Color Plot event", 800, 20, 500, 500);
    // =========>>>> Zoom the event in  with color scale// <<<===============  new canvas
    fDisplay2Canvas->cd();
    fDisplay2Canvas->Clear();
    ZoomWindow = InitializeZoom(MinimumX, MaximumX, MinimumY, MaximumY);     
    //change backgroudand and axis color
    //gPad->SetFillColor(kBlue+4);
    gPad->SetFillColor(kBlack);
    EvtZoomXAxis->SetLabelColor(kWhite);
    EvtZoomXAxis->SetTitleColor(kWhite);
    EvtZoomXAxis->SetLineColor(kWhite);
    EvtXLine->SetLineColor(kWhite);
    EvtZoomYAxis->SetTitleColor(kWhite);
    EvtZoomYAxis->SetLineColor(kWhite);
    EvtZoomYAxis->SetLabelColor(kWhite);
    EvtYLine->SetLineColor(kWhite);
    //================================    
    for (nclusters=0; nclusters<fNClusters; nclusters++){
      phMax = TMath::MaxElement((Long64_t)fCluSize[nclusters],fPHpixel[nclusters]);
      phMin = TMath::MinElement((Long64_t)fCluSize[nclusters],fPHpixel[nclusters]);
      dPH = (phMax - phMin)/(Float_t)(colInt-1); // colInt-1 = 88 colors available in the palette 
      if (dPH <= 0) return;
      // Set PHrange for color plot
      for (Int_t ic=0; ic<colInt; ic++)PHrange[ic] = phMin + ic*dPH;
      PHrange[colInt-1] *= 1.01;
      
      for (npixels=0; npixels<fCluSize[nclusters]; npixels++){
	pixel.X = fXpixel[nclusters][npixels];
	pixel.Y = fYpixel[nclusters][npixels];
	pixel.Height = fPHpixel[nclusters][npixels];
	
	for (Int_t ic=0; ic<colInt-1; ic++)if((pixel.Height>=PHrange[ic])&&(pixel.Height<=PHrange[ic+1])){
	    col = 501+ic;
	  }
	pixel.X += ReCenterX;
	pixel.Y += ReCenterY;
	if (pixel.X > MinimumX && pixel.X < MaximumX && pixel.Y > MinimumY && pixel.Y < MaximumY){
	  //pixel.Height = PITCH/2.2;
	  pixel.Height = PITCH/1.8;
	  hexagon = new THexagonCol(pixel);
	  hexagon->Draw(col);
	}
      }
    }
    
    TMarker *bari2;
    bari2 = new TMarker(BaryCenterX, BaryCenterY, 2);
    bari2->SetMarkerColor(kRed);
    bari2->SetMarkerStyle(2);
    bari2->SetMarkerSize(1.8);
    bari2->Draw();
    fDisplay2Canvas->Update();
  }
  else {
    if(gROOT->GetListOfCanvases()->FindObject(fDisplay2Canvas)){
      fDisplay2Canvas->Close();
      delete fDisplay2Canvas;
    }
  }
  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::InitializePlots(){
  // Define some variables (relevant in this scope).
  Int_t fEvtWidthY;
  Float_t fEvtBorder;
  fEvtWidthY = Int_t(1.2*PIX_C_DIM*PITCH/2);
  fEvtBorder = 0.2*fEvtWidthY;
  // Draw the frame containing the display of single events.
  fDisplayCanvas->cd(1);
  gPad->Range(-fEvtBorder-fEvtWidthY ,-fEvtBorder-fEvtWidthY, fEvtWidthY+fEvtBorder, fEvtWidthY+fEvtBorder);  
  EvtXAxis = new TGaxis(-fEvtWidthY, -fEvtWidthY, fEvtWidthY, -fEvtWidthY, -fEvtWidthY, fEvtWidthY, 510, "");  
  EvtXAxis->SetLabelSize(0.03);
  EvtXAxis->SetTitle("X coordinate (mm)");
  EvtXAxis->Draw();
  EvtYAxis = new TGaxis(-fEvtWidthY, -fEvtWidthY, -fEvtWidthY, fEvtWidthY, -fEvtWidthY, fEvtWidthY, 510, ""); 
  EvtYAxis->SetTitle("Y coordinate (mm)");
  EvtYAxis->SetLabelSize(0.03);
  EvtYAxis->Draw();
  EvtXLine = new TLine(-fEvtWidthY, fEvtWidthY, fEvtWidthY, fEvtWidthY);
  EvtXLine->Draw();
  EvtYLine = new TLine(fEvtWidthY, -fEvtWidthY, fEvtWidthY, fEvtWidthY);
  EvtYLine->Draw(); 
  ReadPixmap();
  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////   
Float_t* TEventDisplay::InitializeZoom(Float_t xmin, Float_t xmax, Float_t ymin, Float_t ymax){
  
  // Define some variables (relevant in this scope).

  Float_t fEvtZoomWidth;
  Float_t fEvtZoomBorder;
  Float_t *fEvtZoom = new Float_t[4];
  Float_t *fPadRange = new Float_t[4];

  fEvtZoomWidth = max((xmax-xmin), (ymax-ymin));
  fEvtZoomBorder = 0.1*fEvtZoomWidth;

  fPadRange[0] = -fEvtZoomBorder+xmin;
  fPadRange[1] = -fEvtZoomBorder+ymin;
  fPadRange[2] = xmin+fEvtZoomWidth+fEvtZoomBorder;
  fPadRange[3] = ymin+fEvtZoomWidth+fEvtZoomBorder;

  fEvtZoom[0] = xmin;
  fEvtZoom[1] = ymin;
  fEvtZoom[2] = xmin+fEvtZoomWidth;
  fEvtZoom[3] = ymin+fEvtZoomWidth;

  // Draw the frame containing the display of single events.
  gPad->Range(fPadRange[0], fPadRange[1], fPadRange[2], fPadRange[3]);
  EvtZoomXAxis = new TGaxis(xmin, ymin, xmin+fEvtZoomWidth, ymin, xmin, xmin+fEvtZoomWidth, 510, "");
  EvtZoomXAxis->SetLabelSize(0.03);
  EvtZoomXAxis->SetTitle("X coordinate (mm)");
  EvtZoomXAxis->Draw();
  EvtZoomYAxis = new TGaxis(xmin, ymin, xmin, ymin+fEvtZoomWidth, ymin, ymin+fEvtZoomWidth, 510, "");
  EvtZoomYAxis->SetTitle("Y coordinate (mm)");
  EvtZoomYAxis->SetLabelSize(0.03);
  EvtZoomYAxis->Draw();
  EvtXLine = new TLine(xmin, ymin+fEvtZoomWidth, xmin+fEvtZoomWidth, ymin+fEvtZoomWidth);
  EvtXLine->Draw();
  EvtYLine = new TLine(xmin+fEvtZoomWidth, ymin, xmin+fEvtZoomWidth, ymin+fEvtZoomWidth);
  EvtYLine->Draw();
  return (fEvtZoom);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::BookHistos(){
  histList = new TObjArray();
  TObject* obj = 0;
  fPHeightHisto = new TH1F("PHeight", "Pulse Height", 600, 0, 12000);
  fPHeightHisto->SetFillColor(60);
  fPHeightHisto->GetXaxis()->SetTitle("Pulse height (ADC counts)");
  obj = fPHeightHisto;
  histList->Add(obj);

  fCluSizeHisto = new TH1F("CluSize", "Cluster Size", 50, 0, 200);
  fCluSizeHisto->SetFillColor(60);
  fCluSizeHisto->GetXaxis()->SetTitle("Number of fired pixels per event");
  obj = fCluSizeHisto;
  histList->Add(obj);

  fCluNumHisto = new TH1F("CluNum", "Cluster Multiplicity", 10, 0, 10);
  fCluNumHisto->SetFillColor(60);
  fCluNumHisto->GetXaxis()->SetTitle("Number of clusters per event");
  obj = fCluNumHisto;
  histList->Add(obj);

  fTriWindowHisto = new TH1F("EventWindow", "Event Window Size", 100, 0, 2500);
  fTriWindowHisto->SetFillColor(50);
  fTriWindowHisto->GetXaxis()->SetTitle("Window Size (num. pixels)");
  obj = fTriWindowHisto;
  histList->Add(obj);

  fThetaHisto = new TH1F("Theta0", "Angular Distribution - iteration 0", 100, -kPI, kPI);
  fThetaHisto->SetFillColor(50);
  fThetaHisto->GetXaxis()->SetTitle("Phi (rad)");
  fThetaHisto->SetMinimum(0);
  obj = fThetaHisto;
  histList->Add(obj);

  fTheta1Histo = new TH1F("Theta1", "Angular Distribution - iteration 1", 100, -kPI, kPI);
  fTheta1Histo->SetFillColor(50);
  fTheta1Histo->GetXaxis()->SetTitle("Phi (rad)");
  fTheta1Histo->SetMinimum(0);
  obj = fTheta1Histo;
  histList->Add(obj);

  fThirdMomHisto = new TH1F("Third Moment", "Major Third Moment", 500, -0.0031, 0.003);
  fThirdMomHisto->GetXaxis()->SetTitle("Third Momentum along principal axis");
  obj = fThirdMomHisto;
  histList->Add(obj);

  fThirdMomHistoNorm = new TH1F("Skewness", "Skewness", 600, -1.5, 1.5);
  fThirdMomHistoNorm->GetXaxis()->SetTitle("Third Momentum/PHeight"); 
  obj = fThirdMomHistoNorm;
  histList->Add(obj);

  fM2XtoM2YHisto = new TH1F("MxDivMy", "MomX/MomY", 100, 0.0, 300.0);
  fM2XtoM2YHisto->SetFillColor(50);
  fM2XtoM2YHisto->GetXaxis()->SetTitle("Ratio of second moments");
  obj = fM2XtoM2YHisto;
  histList->Add(obj); 

  fCluMapHisto = new TH2F("BaricenterMap", "BaricenterMap", 300, -7.5, 7.5, 300, -7.5, 7.5);
  fCluMapHisto->GetXaxis()->SetTitle("X position (mm)");
  fCluMapHisto->GetYaxis()->SetTitle("Y position (mm)");
  obj = fCluMapHisto;
  histList->Add(obj);

  fImpactPtHisto = new TH2F("ImpactPointMap", "ImpactPointposition",  500, -7.5, 7.5, 500, -7.5, 7.5);
  fImpactPtHisto->GetXaxis()->SetTitle("X position (mm)");
  fImpactPtHisto->GetYaxis()->SetTitle("Y position (mm)");
  obj = fImpactPtHisto;
  histList->Add(obj);

  TH2F *fCumulativeHisto = new TH2F("CumulativeHitMap", "CumulativeHitMap", 346, -7.5, 7.5, 300, -7.5, 7.5 );
  fCumulativeHisto->GetXaxis()->SetTitle("X position (mm)");
  fCumulativeHisto->GetYaxis()->SetTitle("Y position (mm)");
  obj = fCumulativeHisto;
  histList->Add(obj);

  fStoNHisto = new TH1F("SDivN", "Signal to Noise", 200, 0, 800);
  fStoNHisto->SetFillColor(60);
  fStoNHisto->GetXaxis()->SetTitle("Signal to Noise");
  obj = fStoNHisto;
  histList->Add(obj);

  TH1F *fTimeStampHisto = new TH1F("TimeStamp", "TimeStamp",100,0,500);
  fTimeStampHisto->SetFillColor(60);
  fTimeStampHisto->GetXaxis()->SetTitle("Time Stamp (msec)");
  obj = fTimeStampHisto;
  histList->Add(obj);

  TH1F *fRateHisto = new TH1F("Rate", "Rate",50,0,500);
  fRateHisto->SetFillColor(60);
  fRateHisto->GetXaxis()->SetTitle("Rate (Hz)");
  obj = fRateHisto;
  histList->Add(obj);
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::InitializeTree(){
  window = new TLine();
  window->SetLineColor(kRed);
  ClusterTree = (TTree*)gDirectory->Get( "tree" );
  ClusterTree->SetBranchAddress("fEventId", &fEventId);
  ClusterTree->SetBranchAddress("fNClusters", &fNClusters);
  ClusterTree->SetBranchAddress("fTrigWindow", &fTrigWindow); 
  ClusterTree->SetBranchAddress("fTimeStamp", &fTimeStamp); 
  ClusterTree->SetBranchAddress("fbufferId", &fbufferId);
  ClusterTree->SetBranchAddress("ROI", ROI);

  ClusterTree->SetBranchAddress("fPHBXpixels", &fPHBXpixels);
  ClusterTree->SetBranchAddress("fNBXpixels", &fNBXpixels);
  ClusterTree->SetBranchAddress("fPHBYpixels", &fPHBYpixels);
  ClusterTree->SetBranchAddress("fNBYpixels", &fNBYpixels);

  ClusterTree->SetBranchAddress("fCluSize", fCluSize);
  ClusterTree->SetBranchAddress("fXpixel", fXpixel);
  ClusterTree->SetBranchAddress("fYpixel", fYpixel);
  ClusterTree->SetBranchAddress("fPHpixel", fPHpixel);
  ClusterTree->SetBranchAddress("fPHeight", fPHeight);
  ClusterTree->SetBranchAddress("fStoN", fStoN);
  ClusterTree->SetBranchAddress("fTotNoise", fTotNoise);
  ClusterTree->SetBranchAddress("fHighestC", fHighestC);
  ClusterTree->SetBranchAddress("fBaricenterX", fBaricenterX);
  ClusterTree->SetBranchAddress("fBaricenterY", fBaricenterY);
  ClusterTree->SetBranchAddress("fTheta0", fTheta0);
  ClusterTree->SetBranchAddress("fTheta1", fTheta1);
  ClusterTree->SetBranchAddress("fThetaDifference", fThetaDifference);
  ClusterTree->SetBranchAddress("fMomX", fMomX);
  ClusterTree->SetBranchAddress("fMomY", fMomY);
  ClusterTree->SetBranchAddress("fMomThirdX", fMomThirdX);
  //ClusterTree->SetBranchAddress("fMomThirdY", fMomThirdY);
  ClusterTree->SetBranchAddress("fImpactX", fImpactX);
  ClusterTree->SetBranchAddress("fImpactY", fImpactY);
  ClusterTree->SetBranchAddress("fThetaR", fThetaR);
  ClusterTree->SetBranchAddress("fImpactXR", fImpactXR);
  ClusterTree->SetBranchAddress("fImpactYR", fImpactYR);
  ClusterTree->SetBranchAddress("fCluLenght",fCluLenght);
  NumberOfEvts = (Int_t) ClusterTree->GetEntries();
  cout << "Number of events: " << NumberOfEvts << endl;
  ClusterTree->GetEntry(0);
  TSi =  fTimeStamp;
  ClusterTree->GetEntry(NumberOfEvts-1);
  TSf =  fTimeStamp;
  Double_t MeanRateDT = (Double_t)(NumberOfEvts/(Double_t)(TSf-TSi))*TMath::Power(10,6);
  cout << "Average rate (Hz) = " << MeanRateDT  << " (reconstructed events NOT filtered) " << endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::FillHistos(Int_t ncl){ 
  ((TH1F*)histList->At(0))->Fill(fPHeight[ncl]);
  ((TH1F*)histList->At(1))->Fill(fCluSize[ncl]);
  //At(2) CluNUM
  //At(3) EventWindow
  ((TH1F*)histList->At(4))->Fill(fTheta0[ncl]);
  ((TH1F*)histList->At(5))->Fill(fTheta1[ncl]); 
  ((TH1F*)histList->At(6))->Fill(fMomThirdX[ncl]);
  
  if(fMomX[ncl])((TH1F*)histList->At(7))->Fill(fMomThirdX[ncl]/TMath::Power(TMath::Sqrt(fMomX[ncl]),3));
  else ((TH1F*)histList->At(7))->Fill(-300);

  if(fMomY[ncl]>0)((TH1F*)histList->At(8))->Fill(fMomX[ncl]/fMomY[ncl]);
  else ((TH1F*)histList->At(8))->Fill(0.);

  ((TH2F*)histList->At(9))->Fill(fBaricenterX[ncl],fBaricenterY[ncl]);	
  ((TH2F*)histList->At(10))->Fill(fImpactX[ncl],fImpactY[ncl]);	
  for (Int_t jj=0; jj<fCluSize[ncl];jj++)
    ((TH2F*)histList->At(11))->Fill((Double_t)fXpixel[ncl][jj],(Double_t)fYpixel[ncl][jj],(Double_t)fPHpixel[ncl][jj]);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::WritePolarizationInfo(){
  // Write infos on the last canvas.
  fCluHistosCanvas->cd(12);
  Char_t DummyChar[20];
  TText *t = new TText();
  TText *w = new TText();
  w->SetTextColor(2);
  t->SetTextSize(0.05);
  w->SetTextSize(0.05);
  // Iteration0.
  t->DrawText(0.05,0.90,"Angular reconstruction - iteration 0:");
  t->DrawText(0.05,0.80,"Modulation factor (%):");
  sprintf(DummyChar, "%3.2f", ModulationFactor0);
  w->DrawText(0.65,0.80, DummyChar);
  w->DrawText(0.80, 0.80, "+-");
  sprintf(DummyChar, "%3.2f", ModulationFactorError0);
  w->DrawText(0.85, 0.80,DummyChar);
  t->DrawText(0.05,0.75,"Polarization Angle (degrees):");
  sprintf(DummyChar, "%3.2f", TMath::Abs(Angle0));
  //sprintf(DummyChar, "%3.2f", Angle0);
  w->DrawText(0.65,0.75, DummyChar);
  w->DrawText(0.80, 0.75, "+-");
  sprintf(DummyChar, "%3.2f", AngleError0);
  w->DrawText(0.85, 0.75,DummyChar);
  t->DrawText(0.05,0.70,"Reduced ChiSquare:");
  sprintf(DummyChar, "%3.2f", ChiSquare0);
  w->DrawText(0.65, 0.70,DummyChar);
  // Iteration 1.
  t->DrawText(0.05,0.55,"Angular reconstruction - iteration 1:");
  t->DrawText(0.05,0.45,"Modulation factor (%):");
  sprintf(DummyChar, "%3.2f", ModulationFactor1);
  w->DrawText(0.65,0.45, DummyChar);
  w->DrawText(0.80, 0.45, "+-");
  sprintf(DummyChar, "%3.2f", ModulationFactorError1);
  w->DrawText(0.85, 0.45,DummyChar);
  t->DrawText(0.05,0.40,"Polarization Angle (degrees):");
  //   sprintf(DummyChar, "%3.2f", TMath::Abs(Angle1));
  sprintf(DummyChar, "%3.2f", Angle1);
  w->DrawText(0.65,0.40, DummyChar);
  w->DrawText(0.80, 0.40, "+-");
  sprintf(DummyChar, "%3.2f", AngleError1);
  w->DrawText(0.85, 0.40,DummyChar);
  t->DrawText(0.05,0.35,"Reduced ChiSquare:");
  sprintf(DummyChar, "%3.2f", ChiSquare1);
  w->DrawText(0.65, 0.35,DummyChar);
  // Energy resolution.
  t->DrawText(0.05,0.25,"Energy Resolution (% FWHM):");
  sprintf(DummyChar, "%3.2f", EnergyResolution);
  w->DrawText(0.65,0.25, DummyChar);
  fCluHistosCanvas->Update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::DrawDirection(Float_t x, Float_t y, Float_t phi, Color_t color){
  Float_t x1, x2, y1, y2;
  TMarker *center;
  TLine *direction;
  center = new TMarker(x, y, 2);
  center->SetMarkerColor(color);
  center->SetMarkerStyle(28);
  center->SetMarkerSize(1.3);
  center->Draw();
  x1 = ZoomWindow[0];
  y1 = y-x*tan(phi)+tan(phi)*ZoomWindow[0];
  x2 = ZoomWindow[2];
  y2 = y-x*tan(phi)+tan(phi)*ZoomWindow[2];
  if (y1<ZoomWindow[1])
    {
      x1 = ZoomWindow[0] + (ZoomWindow[1]-y1)/tan(phi);
      y1 = ZoomWindow[1];
    }
  if (y1>ZoomWindow[3])
    {
      x1 = ZoomWindow[0] - (y1-ZoomWindow[3])/tan(phi);
      y1 = ZoomWindow[3]; 
    }      
  if (y2<ZoomWindow[1])
    {
      x2 = ZoomWindow[2] + (ZoomWindow[1]-y2)/tan(phi);
      y2 = ZoomWindow[1];
    }
  if (y2>ZoomWindow[3])
    {
      x2 = ZoomWindow[2] - (y2-ZoomWindow[3])/tan(phi);
      y2 = ZoomWindow[3]; 
    }
  direction = new TLine(x1, y1, x2, y2);
  direction->SetLineColor(color);
  direction->Draw();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::ReadPixmap(){
  // Draw the border pixels;
  ifstream PixmapFile1;
 
  fitsfile *fptr;         /* FITS file pointer, defined in fitsio.h */
  int status = 0;   /* CFITSIO status value MUST be initialized to zero! */
  long ntrows;
  char TABNAME[] = "PIXMAP";

  float X[NCHANS], Y[NCHANS];
  int I[NCHANS], J[NCHANS];

  string fullpath = workingdir + "/pixmap_xpe.fits";
  if (!fits_open_file(&fptr, fullpath.c_str(), READONLY, &status)) {
  fits_movnam_hdu(fptr, BINARY_TBL, TABNAME,0, &status);
  fits_get_num_rows(fptr, &ntrows, &status);
  fits_read_col(fptr, TINT, 1, 1, 1, ntrows, NULL, J, NULL, &status);
  fits_read_col(fptr, TINT, 2, 1, 1, ntrows, NULL, I, NULL, &status);
  fits_read_col(fptr, TFLOAT, 3, 1, 1, ntrows, NULL, X, NULL, &status);
  fits_read_col(fptr, TFLOAT, 4, 1, 1, ntrows, NULL, Y, NULL, &status);  
  }
  else 
    {cout << "ERROR!!!!!" << endl;
      return;
    }

  for (int ch =0; ch<NCHANS;ch++){
       PixmapX[I[ch]][J[ch]] = X[ch];
       PixmapY[I[ch]][J[ch]] = Y[ch];
     }
  /*
  if (!gSystem->AccessPathName(fullpath.c_str(), kFileExists)) {
    // Read the file for the first time as to evaluate minX, maxX, minY and maxY.
  
    PixelHit pixel;
    THexagonCol *hexagon;
    
    for (int nr=0; nr< PIX_R_DIM; nr++)
      for (int nc=0; nc< PIX_C_DIM; nc++)
	{
	  if (PixBorder[nr][nc]){
	    pixel.X = PixmapX[nr][nc];
	    pixel.Y = PixmapY[nr][nc];
	    pixel.Height = PITCH/2;
	    hexagon = new THexagonCol(pixel);
	    hexagon->Draw(kRed);
	  }
	}
  }
  */
  gSystem->cd(datadir);
  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::ReadInfoFile(TString fname){ 
  infodispIn.open(fname,ios::in);
  infodispIn >> fPulseLow>> fPulseLow;
  infodispIn >> fPulseHigh >> fPulseHigh;
  infodispIn >> fPulseLow1 >> fPulseLow1;
  infodispIn >> fPulseHigh1 >> fPulseHigh1;
  infodispIn >> fNClustLow >> fNClustLow;
  infodispIn >> fNClustHigh >> fNClustHigh;
  infodispIn >> fSignalLow >> fSignalLow;
  infodispIn >> fSignalHigh >> fSignalHigh;
  infodispIn >> fCluSizeLow >> fCluSizeLow;
  infodispIn >> fCluSizeHigh >> fCluSizeHigh;
  infodispIn >> fShapeLow >> fShapeLow;
  infodispIn >> fShapeHigh >> fShapeHigh;
  infodispIn >> fM3Low >> fM3Low;
  infodispIn >> fM3High >> fM3High;
  infodispIn >> fCircleRadius >> fCircleRadius;
    
  infodispIn >> fXEllipse >> fXEllipse;
  infodispIn >> fYEllipse >> fYEllipse;
  infodispIn >> fR1Ellipse >> fR1Ellipse;
  infodispIn >> fR2Ellipse >> fR2Ellipse;
  infodispIn >> fThetaEllipse >> fThetaEllipse;
    
  infodispIn >> fLeftCut >> fLeftCut;
  infodispIn >> fRightCut >> fRightCut;
  infodispIn >> fUpCut >> fUpCut;
  infodispIn >> fDownCut >> fDownCut;
  infodispIn >> fWindowLow >> fWindowLow;
  infodispIn >> fWindowHigh >> fWindowHigh;
  infodispIn >> OrFlagSt >> OrFlag;
  infodispIn >> AndFlagSt >> AndFlag;
  infodispIn.close();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::WriteInfoFile(TString fname){
  cout << "WRITEINFO in : " << fname << endl;
  infodispOut.open(fname,ios::out);
  infodispOut << "PH_LowTh: " << fPulseCutLow->GetString() << endl;	  
  infodispOut << "PH_HighTh: " << fPulseCutHigh->GetString() << endl;
  infodispOut << "PH_LowTh2: " << fPulseCutLow1->GetString() << endl;
  infodispOut << "PH_HighTh2: " << fPulseCutHigh1->GetString() << endl;
  infodispOut << "NCluster_LowTh: " << fNClustCutLow->GetString() << endl;
  infodispOut << "NCluster_HighTh: " << fNClustCutHigh->GetString() << endl;
  infodispOut << "StoN_LowTh: " << fThrLow->GetString() << endl;
  infodispOut << "StoN_HighTh: " << fThrHigh->GetString() << endl;
  infodispOut << "CluSize_LowTh: " << fSizeCutLow->GetString() << endl;
  infodispOut << "CluSize_HighTh: " << fSizeCutHigh->GetString() << endl;
  infodispOut << "Shape_LowTh: " << fShapeCutLow->GetString() << endl;
  infodispOut << "Shape__HighTh: " << fShapeCutHigh->GetString() << endl;
  infodispOut << "M3_LowTh: " << fM3CutLow->GetString() << endl;
  infodispOut << "M3_HighTh: " << fM3CutHigh->GetString() << endl;
  infodispOut << "CircleRadiusCut: " << fCircle->GetString() << endl;
  
  infodispOut << "XellipseCut: " << fXelCut->GetString() << endl;
  infodispOut << "YellipseCut: " << fYelCut->GetString() << endl;
  infodispOut << "R1ellipseCut: " << fR1elCut->GetString() << endl;
  infodispOut << "R2ellipseCut: " << fR2elCut->GetString() << endl;
  infodispOut << "ThetaellipseCut: " << fThetaelCut->GetString() << endl;
  
  infodispOut << "Geo_LeftCut: " << fGeomXLeftCut->GetString() << endl;
  infodispOut << "Geo_RightCut: " << fGeomXRightCut->GetString() << endl;
  infodispOut << "Geo_TopCut: " << fGeomYUpCut->GetString() << endl;
  infodispOut << "Geo_BottomCut: " << fGeomYDownCut->GetString() << endl;
  infodispOut << "Window_LowTh: " << fTrWindowCutLow->GetString() << endl;
  infodispOut << "Window_HighTh: " << fTrWindowCutHigh->GetString() << endl;
  infodispOut << "Orflag: " << OrFlag << endl;
  infodispOut << "AndFlag: " << AndFlag << endl;
  infodispOut.close();
  gSystem->cd(datadir);
  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::InitializeEntries(){
  fPulseLow = "50.0";
  fPulseHigh = "15000.0";
  fPulseLow1 = "0.0";
  fPulseHigh1 = "0.0";
  fNClustLow = "0.0";
  fNClustHigh = "1.0";
  fSignalLow = "0.0";
  fSignalHigh = "500.0";
  fCluSizeLow = "5.0";
  fCluSizeHigh = "200.0";
  fShapeLow = "1.0";
  fShapeHigh = "100.0";
  fM3Low  = "-1.0";
  fM3High  = "1.0";
  fWindowLow  = "100.0";
  fWindowHigh = "900.0";
  fCircleRadius = "10";
  fLeftCut = "-7.6";
  fRightCut = "7.6";
  fUpCut = "7.6";
  fDownCut = "-7.6";
  OrFlag  = 0;
  AndFlag = 1;
    
  fXEllipse = "1.";
  fYEllipse = "1.";
  fR1Ellipse = "2.";
  fR2Ellipse = "3.";
  fThetaEllipse = "0.";
  return;  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::SetPaletteColdBlue(){
  Int_t palette[colInt];
  Int_t pt;
  for (pt=0;pt<int(colInt-1)/2;pt++) {
    color = new TColor(501+pt
		       ,0
		       ,0
		       ,Float_t(pt)/((colInt - 3) / 2),"");
    palette[pt] = 501+pt;
  }
  for (; pt<int(colInt-1) ;pt++) {
    color = new TColor(501+pt
		       ,Float_t(pt-(colInt -1) /2)/((colInt - 3) / 2)
		       ,Float_t(pt-(colInt -1) /2)/((colInt - 3) / 2)
		       ,1,"");
    palette[pt] = 501+pt;
  }
  Int_t col = colInt;
  gStyle->SetPalette(col,palette);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::SetPaletteHotRed(){
  Int_t palette[colInt];
  Int_t pt;
  for (pt=0;pt<int(colInt-1)/2;pt++) {
    color = new TColor(501+pt
		       ,Float_t(pt)/((colInt - 3) / 2)
		       ,0
		       ,0,"");
    palette[pt] = 501+pt;
  }
  for (; pt<int(colInt-1) ;pt++) {
    color = new TColor(501+pt
		       ,1
		       ,Float_t(pt-(colInt -1) /2)/((colInt - 3) / 2)
		       ,Float_t(pt-(colInt -1) /2)/((colInt - 3) / 2),"");
    palette[pt] = 501+pt;
  }
  Int_t col=colInt;
  gStyle->SetPalette(col,palette);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::SelectAnalisedFile(){
  const char *filetypes[] = {"ROOT files",    "*.root",
			      
			     0,               0 };
  static TString dir(".");  
  TGFileInfo fi1;
  fi1.fFileTypes = filetypes;
  fi1.fIniDir    = StrDup(dir);
  new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi1);
  cludata = fi1.fFilename;
  if(cludata=="") return; 
  if(cludata.Contains("RawSignals")){
    box = new TGMsgBox(gClient->GetRoot(), gClient->GetRoot(),"Message", 
    		     "Wrong ROOT file selected!!!", icontype, buttons, &retval);
    cout << "===> ATTENTION!!! Wrong ROOT file selected!!!" << endl;
    CloseWindow();
      return;
    }
  cout << "Load output file: " << cludata << endl;
  dir = fi1.fIniDir;
  datadir = dir + "/";
  if(Clusterdatafile>0){
    delete Clusterdatafile;
    Clusterdatafile = 0;
  }
  if(!Clusterdatafile)Clusterdatafile = new TFile(cludata, "r");
  InitializeTree();
  BookHistos();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::PrevEv(){
  EvtNb --;
  if (EvtNb<0) EvtNb = 0;
  sprintf(buf, "%d", EvtNb);
  fTbuffer1->Clear();
  fTbuffer1->AddText(0, buf);
  fClient->NeedRedraw(fText1);
  fHslider->SetPosition(EvtNb);	
  // Display the event.
  if (gROOT->GetListOfCanvases()->FindObject(fCluHistosCanvas)){
    fCluHistosCanvas->Close();
    delete fCluHistosCanvas;
  }	
 
  if (!gROOT->GetListOfCanvases()->FindObject(fDisplayCanvas))
    {
      fDisplayCanvas = new TCanvas("Single event display", "Single event display", 10, 120, 800, 800);
      cout << "Creating canvas to display single events" << endl;
    }
  fDisplayCanvas->Clear();
  fDisplayCanvas->SetFillColor(10);
  fDisplayCanvas->Divide(2, 2);
  InitializePlots();
  DisplayData(EvtNb);
  fDisplayCanvas->Update();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::DisplayEv(){
  if (gROOT->GetListOfCanvases()->FindObject(fCluHistosCanvas)){
    fCluHistosCanvas->Close();
    delete fCluHistosCanvas;
  }
  
  if (!gROOT->GetListOfCanvases()->FindObject(fDisplayCanvas))
    {
      fDisplayCanvas = new TCanvas("Single event display", "Single event display", 900, 10, 800, 800);
      cout << "Creating canvas to display single events" << endl;
    }
  fDisplayCanvas->Clear();
  fDisplayCanvas->SetFillColor(10);
  fDisplayCanvas->Divide(2, 2);
  InitializePlots();
  DisplayData(EvtNb);
  fDisplayCanvas->Update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::SelectEv(){
  Cluflag = 0;
  //  MaxCluster = 0;
  //  ncluMax = 0;
  ReadGUIbuff();
  EvtNb = atoi(fTbuffer1->GetString());
  if(EvFlag) EvtNb+=1 ;
  cout << "evtflag :" << EvFlag << "  Cluflag: " << Cluflag << " start from Event : " << EvtNb << endl;
  for (Int_t events=EvtNb; events <NumberOfEvts; events++) {
    MaxCluster = 0;
    ncluMax = 0;
    ClusterTree->GetEntry(events);
    for (Int_t ncl=0; ncl<fNClusters;ncl++) {
      if(MaxCluster < fCluSize[ncl]) {
	MaxCluster = fCluSize[ncl]; //highest cluster with highest CluSize
	ncluMax = ncl;
      }
    }
    if (fMomY[ncluMax]){
      if (AndFlag ==1){
	if ((fStoN[ncluMax]>=StoNLoThresh)            &&  (fStoN[ncluMax]<=StoNHiThresh)       &&
	    (fTrigWindow>=TrWinLoThresh               &&  fTrigWindow <=TrWinHiThresh)         &&    
	    (fCluSize[ncluMax]>=CluSizeLoThresh)      &&  (fCluSize[ncluMax]<=CluSizeHiThresh) &&
	    (fPHeight[ncluMax]>=PulseLoThresh)        &&  (fPHeight[ncluMax]<=PulseHiThresh)   &&
	    ((fPHeight[ncluMax]<=PulseLoThresh1)      ||  (fPHeight[ncluMax]>=PulseHiThresh1)) &&
	    (fNClusters>=NClustLoThresh)              &&  (fNClusters<=NClustHiThresh)         &&
	    (((fMomX[ncluMax]/fMomY[ncluMax])>=ShapeLoThresh) &&  ((fMomX[ncluMax]/fMomY[ncluMax])<=ShapeHiThresh) &&
	     ((fMomThirdX[ncluMax] >= M3LoThresh)         &&  (fMomThirdX[ncluMax] <= M3HiThresh))) &&
	    (TMath::Sqrt((fBaricenterY[ncluMax]*fBaricenterY[ncluMax]) + (fBaricenterX[ncluMax]*fBaricenterX[ncluMax]))<=CircleRadiusCut)   &&
	    (fBaricenterY[ncluMax]>=GeomYDownCut)     &&  (fBaricenterY[ncluMax]<=GeomYUpCut)  &&
	    (fBaricenterX[ncluMax]>=GeomXLeftCut)     &&  (fBaricenterX[ncluMax]<=GeomXRightCut)
	    )
	  {
	    cout << events << "   --- " << fPHeight[ncluMax] << endl;
	    EvtNb = events;
	    EvFlag = 1;
	    Cluflag = 1;
	    break;
	  }
      }
      if (OrFlag ==1){
	if ((fStoN[ncluMax]>=StoNLoThresh)            && (fStoN[ncluMax]<=StoNHiThresh)         &&
	    (fTrigWindow>=TrWinLoThresh               &&  fTrigWindow <=TrWinHiThresh)          &&  
	    (fCluSize[ncluMax]>=CluSizeLoThresh)      && (fCluSize[ncluMax]<=CluSizeHiThresh)   &&
	    (fPHeight[ncluMax]>=PulseLoThresh)        && (fPHeight[ncluMax]<=PulseHiThresh)     &&
	    ((fPHeight[ncluMax]<=PulseLoThresh1)      || (fPHeight[ncluMax]>=PulseHiThresh1))   &&
	    (fNClusters>=NClustLoThresh)              && (fNClusters<=NClustHiThresh)           &&
	    (((fMomX[ncluMax]/fMomY[ncluMax])>=ShapeLoThresh) && ((fMomX[ncluMax]/fMomY[ncluMax])<=ShapeHiThresh)  &&
	     ((fMomThirdX[ncluMax] <= M3LoThresh)   || (fMomThirdX[ncluMax] >= M3HiThresh)))  &&
	    (TMath::Sqrt((fBaricenterY[ncluMax]*fBaricenterY[ncluMax]) + (fBaricenterX[ncluMax]*fBaricenterX[ncluMax]))<=CircleRadiusCut)  &&
	    (fBaricenterY[ncluMax]>=GeomYDownCut)     && (fBaricenterY[ncluMax]<=GeomYUpCut)    &&
	    (fBaricenterX[ncluMax]>=GeomXLeftCut)     && (fBaricenterX[ncluMax]<=GeomXRightCut)
	    )	
	  {
	    EvtNb = events;
	    EvFlag = 1;
	    Cluflag = 1;
	    break;
	  }
      }	    
    }
  }// loop on events!!!
  
  if (!Cluflag)
    {box = new TGMsgBox(gClient->GetRoot(), gClient->GetRoot(),"Message", "No more clusters found!", kMBIconExclamation, kMBClose, &retval);
      cout << "=========>>> No more clusters found. Change PH cuts!" << endl;
      //reset to event 0
      fTbuffer1->Clear();
      fTbuffer1->AddText(0, "0");
      fClient->NeedRedraw(fText1);
      fHslider->SetPosition(0);
      EvFlag=0;
    }
  else {
    cout << "Event: " << EvtNb << "  - PH: " << fPHeight[ncluMax] << endl;
    sprintf(buf, "%d", EvtNb);
    fTbuffer1->Clear();
    fTbuffer1->AddText(0, buf);
    fClient->NeedRedraw(fText1);
    fHslider->SetPosition(EvtNb);
  }
  if (!gROOT->GetListOfCanvases()->FindObject(fDisplayCanvas))
    {
      fDisplayCanvas = new TCanvas("Single event display", "Single event display", 10, 120, 800, 800);
      cout << "Creating canvas to display single events" << endl;
    }
  fDisplayCanvas->Clear();
  fDisplayCanvas->SetFillColor(10);
  fDisplayCanvas->Divide(2, 2);
  InitializePlots();
  DisplayData(EvtNb);
  fDisplayCanvas->Update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::DrawCircle(){
  if (gROOT->GetListOfCanvases()->FindObject(fDisplayCanvas)){
    fDisplayCanvas->cd(1);
    Float_t WideRadius = WideCRadius*TMath::Sqrt(M2Max);
    Float_t SmallRadius = SmallCRadius*TMath::Sqrt(M2Max);
    
    WideCircle = new TEllipse(BaryCenterX, BaryCenterY, WideRadius, WideRadius,
			      0.0, 180.0, (Phi1*180.0/kPI + 90.0));

    SmallCircle = new TEllipse(BaryCenterX, BaryCenterY, SmallRadius, SmallRadius,
			       0.0, 180.0, (Phi1*180.0/kPI + 90.0));
    WideCircle->SetLineColor(kBlue);
    SmallCircle->SetLineColor(kRed);
    fDisplayCanvas->cd(2);
    WideCircle->Draw();
    SmallCircle->Draw();
    DisplayData(EvtNb);
    fDisplayCanvas->Update();
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::BrowseRFile(){
  SelectAnalisedFile();
  num ="";
  num += NumberOfEvts;
  fEvents->Clear();
  fEvents->AddText(0,num);
  fClient->NeedRedraw(fnumEv);
  fHslider->SetRange(0,NumberOfEvts-1);
  fHslider->SetPosition(0);  
  fTbuffer1->Clear();
  fTbuffer1->AddText(0, "0");
  fClient->NeedRedraw(fText1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::DrawHistos(){
  ReadGUIbuff();
  WriteInfoFile(infodispfn);

  ellips = new TEllipse(XEllipseCut,YEllipseCut,R1EllipseCut,R2EllipseCut,0.,360.,ThetaEllipseCut);
  ellips->SetFillStyle(4000);
  
  Double_t st = TMath::Sin(TMath::Pi()*ThetaEllipseCut/180);
  Double_t ct = TMath::Cos(TMath::Pi()*ThetaEllipseCut/180); 
  
  if (gROOT->GetListOfCanvases()->FindObject(fDisplayCanvas)){
    fDisplayCanvas->Close();
    delete fDisplayCanvas;
  }

  if (!gROOT->GetListOfCanvases()->FindObject(fCluHistosCanvas))
    { 
      fCluHistosCanvas = new TCanvas("Clusters Histos window", " Clusters Histos", 900, 20, 1000, 650);
      fCluHistosCanvas->SetFillColor(10);
      cout << "Creating canvas to display Clusters Histos" << endl;
    }
  // Read data and fill histos applying cuts

  Double_t xh,yh,dist,xh1,yh1;

  Int_t jj =0;	 
  Int_t flagClu;
  T0 = 0., T1 = 0.;
  Int_t bufIdStart = 0;
  //Display Histos
  fCluHistosCanvas->Clear();
  fCluHistosCanvas->Divide(4,3);
  ((TH2F*)histList->At(11))->Reset();
  for (Int_t nhis=0; nhis<11; nhis++) ((TH1F*)histList->At(nhis))->Reset();
  for (Int_t events=0; events <NumberOfEvts; events++) {
    gSystem->ProcessEvents(); //Added 29-01-08
    flagClu = 1;	    
    ClusterTree->GetEntry(events);  
    T1 = fTimeStamp;
    if(fbufferId== bufIdStart){  // we are inside the same buffer
      ((TH1F*)histList->At(13))->Fill((Float_t)(T1-T0)/TMath::Power(10,3));
      if((T1-T0)!=0)((TH1F*)histList->At(14))->Fill(TMath::Power(10,6)/(Float_t)(T1-T0));
    }
    else{ // buffer change
      bufIdStart = fbufferId;
    }
    for (Int_t ncl=0; ncl<fNClusters;ncl++){
      xh1 = (fImpactX[ncl]-XEllipseCut);
      yh1 = (fImpactY[ncl]-YEllipseCut);
      xh = xh1*ct+yh1*st;
      yh = xh1*st-yh1*ct;
      dist = TMath::Power(xh,2)/TMath::Power(R1EllipseCut,2)+TMath::Power(yh,2)/TMath::Power(R2EllipseCut,2);
      //Fill Histos applying selected cuts
      if (fMomY[ncl]){
	if(AndFlag){
	  if ((fStoN[ncl]>=StoNLoThresh)                &&  (fStoN[ncl]<=StoNHiThresh)               &&
	      (fTrigWindow>=TrWinLoThresh               &&  fTrigWindow <=TrWinHiThresh)             &&    
	      (fCluSize[ncl]>=CluSizeLoThresh)          &&  (fCluSize[ncl]<=CluSizeHiThresh)         &&
	      (fPHeight[ncl]>=PulseLoThresh)            &&  (fPHeight[ncl]<=PulseHiThresh)           &&
	      ((fPHeight[ncl]<=PulseLoThresh1)          ||  (fPHeight[ncl]>=PulseHiThresh1))         &&
	      (fNClusters>=NClustLoThresh)              &&  (fNClusters<=NClustHiThresh)             &&
	      (((fMomX[ncl]/fMomY[ncl])>=ShapeLoThresh) &&  ((fMomX[ncl]/fMomY[ncl])<=ShapeHiThresh) &&
	       ((fMomThirdX[ncl] >= M3LoThresh)         &&  (fMomThirdX[ncl] <= M3HiThresh))) &&
	      (TMath::Sqrt((fBaricenterY[ncl]*fBaricenterY[ncl]) + (fBaricenterX[ncl]*fBaricenterX[ncl]))<=CircleRadiusCut)   &&
	      (fImpactY[ncl]>=GeomYDownCut)  &&  (fImpactY[ncl]<=GeomYUpCut)          &&
	      (fImpactX[ncl]>=GeomXLeftCut)    &&  (fImpactX[ncl]<=GeomXRightCut) && dist<1.0
	      )
	    {
	      FillHistos(ncl);
	      if (flagClu){
		((TH1F*)histList->At(2))->Fill(fNClusters);
		((TH1F*)histList->At(3))->Fill(fTrigWindow);
	      }
	      flagClu = 0;		    
	    }
	}  		
	if(OrFlag){
	  if ((fStoN[ncl]>=StoNLoThresh)                && (fStoN[ncl]<=StoNHiThresh)                &&
	      (fTrigWindow>=TrWinLoThresh               &&  fTrigWindow <=TrWinHiThresh)             &&  
	      (fCluSize[ncl]>=CluSizeLoThresh)          && (fCluSize[ncl]<=CluSizeHiThresh)          &&
	      (fPHeight[ncl]>=PulseLoThresh)            && (fPHeight[ncl]<=PulseHiThresh)            &&
	      ((fPHeight[ncl]<=PulseLoThresh1)          ||  (fPHeight[ncl]>=PulseHiThresh1))         &&
	      (fNClusters>=NClustLoThresh)              && (fNClusters<=NClustHiThresh)              &&
	      (((fMomX[ncl]/fMomY[ncl])>=ShapeLoThresh) && ((fMomX[ncl]/fMomY[ncl])<=ShapeHiThresh)  &&
	       ((fMomThirdX[ncl] <= M3LoThresh)           || (fMomThirdX[ncl] >= M3HiThresh)))           &&
	      (TMath::Sqrt((fBaricenterY[ncl]*fBaricenterY[ncl]) + (fBaricenterX[ncl]*fBaricenterX[ncl]))<=CircleRadiusCut)  &&
	      (fImpactY[ncl]>=GeomYDownCut)  &&  (fImpactY[ncl]<=GeomYUpCut)          &&
	      (fImpactX[ncl]>=GeomXLeftCut)    &&  (fImpactX[ncl]<=GeomXRightCut) && dist<1.0
	      )				     
	    {
	      FillHistos(ncl);
	      if (flagClu){
		((TH1F*)histList->At(2))->Fill(fNClusters);
		((TH1F*)histList->At(3))->Fill(fTrigWindow);
	      }
	      flagClu = 0;		    
	    }
	}
      }
      else  jj++;// Count clusters with MomY=0	  
    }
    
    // Fill with the cluster multiplicity of the event after applied cuts
    if (events%5000==0) {
      cout << events << " events displayed!" << endl;
      for (Int_t pad=1; pad<12; pad++) {
	fCluHistosCanvas->cd(pad);
	if(pad<9)((TH1F*)histList->At(pad-1))->Draw();
	else ((TH2F*)histList->At(pad-1))->Draw("colz");
      }	    
      fCluHistosCanvas->Update();	 
    } //end update histos
    T0 = T1;
  } //end loop on events

  gStyle->SetPalette(0,0);
  
  // Draw histos for the remaining events   
  for (Int_t pad=1; pad<12; pad++) {
    fCluHistosCanvas->cd(pad);	
    if(pad<9)((TH1F*)histList->At(pad-1))->Draw();
    else {
      ((TH2F*)histList->At(pad-1))->Draw("colz");    
    }
  }
  
  newCanvas = new TCanvas("Cumulative","Cumulative Hit Map",500,300,600,600);
  gStyle->SetPalette(1);
  ((TH2F*)histList->At(11))->Draw("colz");
  newCanvas->Update();

  //ellips->Draw();
  
  fCluHistosCanvas->Update();
  cout << "Clusters scartati per MomY=0: " << jj << endl;
  
  fCluHistosCanvas->cd(1);	  
  
  PHeightFunction = new TF1("PHeightFunction", "gaus");
  PHeightFunction->SetLineWidth(2);
  //fPHeightHisto->Fit("PHeightFunction");
  // EnergyResolution = 100.0*2.35*PHeightFunction->GetParameter(2)/PHeightFunction->GetParameter(1);
 
  // Phi - iteration 0.
  fCluHistosCanvas->cd(5);	  
  PhiFunction0 = new TF1("PhiFunction0", "[0] + [1]*cos(x-[2])*cos(x-[2])", -kPI, kPI);
  PhiFunction0->SetLineWidth(2); 
  PhiFunction0->SetParLimits(2,-kPI,kPI);
  PhiFunction0->SetParLimits(0,0.,50000);
  PhiFunction0->SetParLimits(1,0.,50000);
  PhiFunction0->SetParNames("Flat term", "Modulated term", "Angle");
  PhiFunction0->SetParameters(fThetaHisto->GetMaximum(), fThetaHisto->GetMaximum(), 0.0);
  fThetaHisto->Fit("PhiFunction0", "MLR");
  A0 = PhiFunction0->GetParameter(0);
  B0 = PhiFunction0->GetParameter(1);
  ModulationFactor0 = B0/(B0+2*A0)*100.0;
  Angle0 = PhiFunction0->GetParameter(2)*180.0/kPI;
  AError0 = PhiFunction0->GetParError(0);
  BError0 = PhiFunction0->GetParError(1);
  //ModulationFactorError0 = 2.0*100.0*(A0*BError0+B0*AError0)/((B0+2*A0)*(B0+2*A0));
  Float_t dummy1 = (2*A0*BError0)/pow((B0+2*A0),2);
  Float_t dummy2 = (2*B0*AError0)/pow((B0+2*A0),2);
  ModulationFactorError0 = 100*TMath::Power(TMath::Power((double)dummy1,2)+TMath::Power((double)dummy2,2),0.5);
  AngleError0 = PhiFunction0->GetParError(2)*180.0/kPI;
  ChiSquare0 = PhiFunction0->GetChisquare()/( Float_t )PhiFunction0->GetNDF();
  
  // Phi - iteration 1.
  fCluHistosCanvas->cd(6);	  
  PhiFunction1 = new TF1("PhiFunction1", "[0] + [1]*cos(x-[2])*cos(x-[2])", -kPI, kPI);
  PhiFunction1->SetLineWidth(2);
  PhiFunction1->SetParLimits(2,-kPI,kPI);
  PhiFunction1->SetParLimits(0,0.,50000);
  PhiFunction1->SetParLimits(1,0.,50000);
  PhiFunction1->SetParNames("Flat term", "Modulated term", "Angle");
  PhiFunction1->SetParameters(fTheta1Histo->GetMaximum(), fTheta1Histo->GetMaximum(), 0.0);
  fTheta1Histo->Fit("PhiFunction1", "MLR");
  A1 = PhiFunction1->GetParameter(0);
  B1 = PhiFunction1->GetParameter(1);
  ModulationFactor1 = B1/(B1+2*A1)*100.0;
  Angle1 = PhiFunction1->GetParameter(2)*180.0/kPI;
  AError1 = PhiFunction1->GetParError(0);
  BError1 = PhiFunction1->GetParError(1);
  //ModulationFactorError1 = 2.0*100.0*(A1*BError1+B1*AError1)/((B1+2*A1)*(B1+2*A1));
  Float_t dummy11 = (2*A1*BError1)/pow((B1+2*A1),2);
  Float_t dummy22 = (2*B1*AError1)/pow((B1+2*A1),2);
  ModulationFactorError1 = 100*TMath::Power(TMath::Power((double)dummy11,2)+TMath::Power((double)dummy22,2),0.5);
  AngleError1 = PhiFunction1->GetParError(2)*180.0/kPI;
  ChiSquare1 = PhiFunction1->GetChisquare()/( Float_t )PhiFunction1->GetNDF();
  fCluHistosCanvas->Update();
  WritePolarizationInfo();
  Entries = (Int_t)((TH1F*)histList->At(0))->GetEntries();
  MDP = 4.29/(ModulationFactor1*TMath::Sqrt(Entries));

  cout << "Number of Events: " << NumberOfEvts << " --- Entries in PH histos after cuts: " << Entries << endl; 
 
  nCanvas = new TCanvas("secondstepMod","Modulation second step");
  TString pp = "Modulation factor: ";
  pp+= ModulationFactor1;
  pp.Resize(25);
  ((TH1F*)histList->At(5))->SetTitle(pp);
  //((TH1F*)histList->At(5))->Draw();
  gPad->Update();
  TPaveStats *stpave = (TPaveStats*)((TH1F*)histList->At(5))->FindObject("stats");
  stpave->SetOptStat(0);
  stpave->SetOptFit(11);
  //cout << stpave->GetX1NDC() << endl; 
  //cout << stpave->GetX2NDC() << endl; 
  //cout << stpave->GetY1NDC() << endl;
  //cout << stpave->GetY2NDC() << endl;
  stpave->SetX1NDC(0.7);
  stpave->SetY1NDC(0.885);
  ((TH1F*)histList->At(5))->Draw();
  TString name = cludata;
  name.Replace(cludata.Length()-5,14,"-IIstepMod.png");
  nCanvas->Print(name);
  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEventDisplay::NextEv(){
  EvtNb ++;
  if (EvtNb >= NumberOfEvts) EvtNb = NumberOfEvts-1;
  sprintf(buf, "%d", EvtNb);
  fTbuffer1->Clear();
  fTbuffer1->AddText(0, buf);
  fClient->NeedRedraw(fText1);
  fHslider->SetPosition(EvtNb);	
  // Display the event.
  if (gROOT->GetListOfCanvases()->FindObject(fCluHistosCanvas)){
    fCluHistosCanvas->Close();
    delete fCluHistosCanvas;
  }	
 
  if (!gROOT->GetListOfCanvases()->FindObject(fDisplayCanvas))
    {
      fDisplayCanvas = new TCanvas("Single event display", "Single event display", 10, 120, 800, 800);
      cout << "Creating canvas to display single events" << endl;
    }
  fDisplayCanvas->Clear();
  fDisplayCanvas->SetFillColor(10);
  fDisplayCanvas->Divide(2, 2);
  InitializePlots();
  DisplayData(EvtNb);
  fDisplayCanvas->Update();
  return;
}

void TEventDisplay::ReadGUIbuff(){
  CircleRadiusCut    = atof(fCircle->GetString());	    //cout << "Circle Radius Cut: " << CircleRadiusCut << endl;  
  
  XEllipseCut        = atof(fXelCut->GetString());
  YEllipseCut        = atof(fYelCut->GetString());
  R1EllipseCut       = atof(fR1elCut->GetString());
  R2EllipseCut       = atof(fR2elCut->GetString());
  ThetaEllipseCut    = atof(fThetaelCut->GetString());
  
  GeomXLeftCut	     = atof(fGeomXLeftCut->GetString());
  GeomXRightCut      = atof(fGeomXRightCut->GetString());
  GeomYUpCut         = atof(fGeomYUpCut->GetString());
  GeomYDownCut       = atof(fGeomYDownCut->GetString());
  
  PulseLoThresh      = atof(fPulseCutLow->GetString());       //cout << "PH Th. Low: " << PulseLoThresh << endl;
  PulseHiThresh1     = atof(fPulseCutHigh1->GetString());      //cout << "PH Th. High: " << PulseHiThresh << endl;
  PulseLoThresh1     = atof(fPulseCutLow1->GetString());       //cout << "PH Th. Low1: " << PulseLoThresh << endl;
  PulseHiThresh      = atof(fPulseCutHigh->GetString());      //cout << "PH Th. High1: " << PulseHiThresh << endl;
  NClustLoThresh     = atof(fNClustCutLow->GetString());      //cout << "NCluster Th. Low: " << NClustLoThresh << endl;
  NClustHiThresh     = atof(fNClustCutHigh->GetString());     //cout << "NCluster Th. High: " << NClustHiThresh << endl;
  StoNLoThresh       = atof(fThrLow->GetString());            //cout << "S/N Th. Low: " << StoNLoThresh << endl;
  StoNHiThresh       = atof(fThrHigh->GetString());           //cout << "S/N Th. High: " << StoNHiThresh << endl;
  CluSizeLoThresh    = atof(fSizeCutLow->GetString());        //cout << "Clu Size Th. Low: " << CluSizeLoThresh << endl;
  CluSizeHiThresh    = atof(fSizeCutHigh->GetString());       //cout << "Clu Size Th. High: " << CluSizeHiThresh << endl;
  ShapeLoThresh      = atof(fShapeCutLow->GetString());       //cout << "Shape Th. Low:" << ShapeLoThresh << endl;
  ShapeHiThresh      = atof(fShapeCutHigh->GetString());      //cout << "Shape Th. High:" << ShapeHiThresh << endl;
  M3LoThresh         = atof(fM3CutLow->GetString());          //cout << "Mom 3rd Th. Low:" << M3LoThresh << endl;
  M3HiThresh         = atof(fM3CutHigh->GetString());         //cout << "Mom 3rd Th. High:" << M3HiThresh << endl;
  TrWinLoThresh      = atof(fTrWindowCutLow->GetString());
  TrWinHiThresh      = atof(fTrWindowCutHigh->GetString());
  return;
}

ClassImp(TEventDisplay)
