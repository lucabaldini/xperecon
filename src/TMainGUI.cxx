#include "TMainGUI.h"
#include <algorithm>

bool sort_by_ph (TCluster* x, TCluster* y) { return (x->fPulseHeight > y->fPulseHeight); }

TMainGUI::TMainGUI(const TGWindow *p, UInt_t w, UInt_t h, Int_t VLEVEL, char* _name):TGMainFrame(p, w, h){

  SetProgParameters(VLEVEL, _name);
  SetWorkingdir();
  Init();
  if (VLEVEL > 0) cout << "TMainGUI: setting VLEVEL=" << VLEVEL << " _name=" << _name << endl; 
  if (VLEVEL > 0) cout << "TMainGUI: setting workingdir=" << workingdir << endl; 

  // Create layout for menubar and popup menus.	
  fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1);
  fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
  fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);
 
  fPopupMenu = new TGPopupMenu(gClient->GetRoot());
  fPopupMenu->AddEntry("&Select file/s", FILE_OPEN);
  fPopupMenu->AddSeparator();
  fPopupMenu->AddEntry("E&xit", WEXIT);
  fPopupMenu->Associate(this);
  
  fPopupHelp = new TGPopupMenu(gClient->GetRoot());
  fPopupHelp->AddEntry("&About", ABOUT);
  fPopupHelp->Associate(this);

  fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
  fMenuBar->AddPopup("&File", fPopupMenu, fMenuBarItemLayout);
  fMenuBar->AddPopup("&Help",fPopupHelp,fMenuBarHelpLayout); 
  AddFrame(fMenuBar, fMenuBarLayout);

  fMainFrameVert = new TGCompositeFrame(this, 150, 150, kVerticalFrame | kRaisedFrame | kFixedWidth);
  fMainFrameHor = new TGCompositeFrame(fMainFrameVert, 150, 150, kHorizontalFrame | kRaisedFrame);
  fRightFrame = new TGCompositeFrame(fMainFrameHor, 100, 100, kVerticalFrame | kRaisedFrame);
  fLeftFrame = new TGCompositeFrame(fMainFrameHor, 50, 100, kVerticalFrame | kRaisedFrame);

  fDataFrame = new TGGroupFrame(fRightFrame, new TGString("Data"));
  fDataEvFrame = new TGCompositeFrame(fDataFrame, 150, 150, kHorizontalFrame);
  fDataEvFrame1 = new TGCompositeFrame(fDataFrame, 150, 150, kHorizontalFrame);
  fWeightFrame = new TGCompositeFrame(fDataFrame, 150, 150, kHorizontalFrame);
  fSradiusFrame = new TGCompositeFrame(fDataFrame, 150, 150, kHorizontalFrame);
  fWradiusFrame = new TGCompositeFrame(fDataFrame, 150, 150, kHorizontalFrame);

  fButtonLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kRaisedFrame, 0, 0, 0, 0);
  fAnalysisLayout = new TGLayoutHints(kLHintsExpandX, 4, 4, 4, 4);

  //---------------------------------------------------------------------------------
  // Add Logo & Title in Left Frame
  TGString *Title;
  Title = new TGString("The PixiE Analysis program");
  fTitle = new TGLabel(fLeftFrame, Title);
  fLeftFrame->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 4, 4, 40, 0));

  fLogo = gClient->GetPicture( "crab.xpm" );
  fLogoImageMap = new TGImageMap( fLeftFrame, fLogo );
  fLeftFrame->AddFrame(fLogoImageMap, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY));

  //------------------DATA------------------------------------------------------------------

  fAnalizeDataButton = new TGTextButton(fDataEvFrame, "Analyse data");
  fAnalizeDataButton->Connect("Clicked()","TMainGUI",this,"DataAnalysis()");
  fAnalizeDataButton->SetToolTipText("Press here to analize data");
  fDataEvFrame->AddFrame(fAnalizeDataButton, fButtonLayout);
  fDataFrame->AddFrame(fDataEvFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));

  fTitleDataEv = new TGLabel(fDataEvFrame1, new TGString("Events from:"));
  fDataEvFrame1->AddFrame(fTitleDataEv, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4, 4, 4, 4));
  fDataEvEntry = new TGTextEntry(fDataEvFrame1, fEvData = new TGTextBuffer(80));
  fEvData->AddText(0,"0");
  fDataEvEntry->SetToolTipText("Furst event to analize");
  fDataEvFrame1->AddFrame(fDataEvEntry, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 4, 4, 4, 4));

  fTitleDataEv1 = new TGLabel(fDataEvFrame1, new TGString("   to:"));
  fDataEvFrame1->AddFrame(fTitleDataEv1, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4, 4, 4, 4));
  fDataEvEntry1 = new TGTextEntry(fDataEvFrame1, fEvData1 = new TGTextBuffer(80));
  fEvData1->AddText(0,"All");
  fDataEvEntry1->SetToolTipText("Last event to analize");
  fDataEvFrame1->AddFrame(fDataEvEntry1, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 4, 4, 4, 4));
  fDataFrame->AddFrame(fDataEvFrame1, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));
  
  fHeadToFile = new TGCheckButton(fDataFrame, "Header On", HEADON);
  fHeadToFile->Associate(this);  
  fHeadToFile->SetState(kButtonUp);
  HeaderOn = false;
  fDataFrame->AddFrame(fHeadToFile,new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4, 4, 4, 4));

  fDataThresholdFrame = new TGCompositeFrame(fDataFrame, 150, 150, kHorizontalFrame);
  
  fThfix = new TGCheckButton(fDataThresholdFrame, "Fixed", THFIX);
  fThfix->Associate(this);
  if (atoi(W5) ) {
    fThfix->SetState(kButtonDown);
    ThfixFlag = 1;
  
  }
  else {
    fThfix->SetState(kButtonUp);
    ThfixFlag = 0;
  }

  fDataThresholdFrame->AddFrame(fThfix, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4, 4, 4, 4));

  fDataThresholdLabel = new TGLabel(fDataThresholdFrame, new TGString("Pixel Thresh."));
  fDataThresholdFrame->AddFrame(fDataThresholdLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4, 4, 4, 4));
  fDataThresholdEntry = new TGTextEntry(fDataThresholdFrame, fThresholdBuf = new TGTextBuffer(40));
  fThresholdBuf->AddText(0,W4);
  fDataThresholdEntry->SetToolTipText("Insert the threshold for noise rejection (number of STDs)");
  fDataThresholdFrame->AddFrame(fDataThresholdEntry, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 4, 4, 4, 4));
  fDataFrame->AddFrame(fDataThresholdFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));

  fTitleSradius = new TGLabel(fSradiusFrame, new TGString("SmallCircleRadius:"));
  fSradiusFrame->AddFrame(fTitleSradius, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4, 4, 4, 4));
  fSradiusEntry = new TGTextEntry(fSradiusFrame, fW1 = new TGTextBuffer(80));
  fW1->AddText(0,W1);
  fSradiusEntry->SetToolTipText("Input SmallCircleRadius for Impact Point reconstruction");
  fSradiusFrame->AddFrame(fSradiusEntry, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 4, 4, 4, 4));
  fDataFrame->AddFrame(fSradiusFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));
  
  fTitleWradius  = new TGLabel(fWradiusFrame, new TGString("WideCircleRadius:"));
  fWradiusFrame->AddFrame(fTitleWradius, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4, 4, 4, 4));
  fWradiusEntry = new TGTextEntry(fWradiusFrame, fW2 = new TGTextBuffer(80));
  fW2->AddText(0,W2);
  fWradiusEntry->SetToolTipText("Input WideCircleRadius for Impact Point reconstruction");
  fWradiusFrame->AddFrame(fWradiusEntry, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 4, 4, 4, 4));
  fDataFrame->AddFrame(fWradiusFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));
  
  fTitleWeight = new TGLabel(fWeightFrame, new TGString("WeightLengthScale:"));
  fWeightFrame->AddFrame(fTitleWeight, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4, 4, 4, 4));
  fWeightEntry = new TGTextEntry(fWeightFrame, fW3 = new TGTextBuffer(80));
  fW3->AddText(0,W3);
  fWeightEntry->SetToolTipText("Input WeightLengthScale for II step reconstruction");
  fWeightFrame->AddFrame(fWeightEntry, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 4, 4, 4, 4));
  fDataFrame->AddFrame(fWeightFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));
  
  fDisplayEventButton = new TGTextButton(fDataFrame, "&Display data");
  fDisplayEventButton->Connect("Clicked()","TMainGUI",this,"DataDisplay()");

  fDisplayEventButton->SetToolTipText("Press here to display event");
  fDataFrame->AddFrame(fDisplayEventButton, fButtonLayout);
  fRightFrame->AddFrame(fDataFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 4, 4, 0, 0));
  //---------------------------------------------------------------------------------
 
  // Compose frames in window
  fMainFrameHor->AddFrame(fLeftFrame, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY, 0, 0, 0, 0));
  fMainFrameHor->AddFrame(fRightFrame, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY, 0, 0, 0, 0)); 
  fMainFrameVert->AddFrame(fMainFrameHor, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY, 0, 0, 0, 0));
  AddFrame(fMainFrameVert, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY, 4, 4, 4, 4));
  
  MapSubwindows();
  Resize(GetDefaultSize());
  SetWindowName("Pixy Control Panel (v6.1)");
  Resize(550, 450);
  MapWindow();
  Move(100,10);
 }


TMainGUI::~TMainGUI() {
  delete fMainFrameHor;
  delete fMainFrameVert;
 
  delete fDataFrame;
  delete fDataEvFrame;
  delete fDataThresholdLabel;
  delete fDataThresholdFrame;
  delete fDataThresholdEntry;
  delete fThresholdBuf;
  delete fRightFrame;
  delete fLeftFrame;
 
  delete fButtonLayout;
  delete fAnalysisLayout;
  delete fMenuBarLayout;
  delete fMenuBarItemLayout;
  delete fPopupMenu;
  delete fMenuBar;
 
  delete fAnalizeDataButton;
  delete fDisplayEventButton;
  delete fDataEvEntry;

  delete fEvData;
  delete fTitle;

  delete fTitleDataEv;
  delete fLogoImageMap;
  delete EventDisplay;
}


Bool_t TMainGUI::ProcessMessage(Long_t msg, Long_t par1, Long_t){
  if (GET_MSG(msg) == kC_COMMAND) {
    if (GET_SUBMSG(msg) == kCM_BUTTON || GET_SUBMSG(msg) == kCM_CHECKBUTTON ) {
      switch(par1) {

      case THFIX:
	if (fThfix->GetState()) {
	  ThfixFlag = 1;
	  cout << "===> Pixels Threshold: FIXED (ADC counts)" << endl;
	}
	else {
	  ThfixFlag = 0;
	  cout << "===> Pixels Threshold: VARIABLE (N sigmas)" << endl;
	}
	break;

      case HEADON:
	if (fHeadToFile->GetState()){
	  HeaderOn = true;
	  cout << "headerOn" << endl;
	}
	else {
	  HeaderOn = false;
	  cout << "headerOff" << endl;
	}
	break;

      default:break;
      }
    }

    if (GET_SUBMSG(msg) == kCM_MENU) {
      switch(par1) {
	
      case  FILE_OPEN:
	DataSelect();
	break;
		
      case WEXIT:
	CloseWindow();   // this also terminates theApp
	break;
      
      case ABOUT:
	ed = new Editor(this, 800, 400);
	ed->LoadFile("ReadMe.txt",0,-1);
	ed->Popup();
	break;
	
      default:break;
      }
    } 
  }  
  return kTRUE;
}


void TMainGUI::DataSelect()
{
  const char *filetypes[] = { "Data files", "*.*",
			      "ROOT files", "*.root",
			      0,0 };
  static TString dir(".");
  fi.fFileTypes = filetypes;
  fi.fIniDir    = StrDup(dir);
  fi.fMultipleSelection = 1;
  new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);  
  dir = fi.fIniDir;
  return;
}


void TMainGUI::DataAnalysis()
{
  TList *_fileNamesList = fi.fFileNamesList;
  if (DebugLevel > 1) cout << "_fileNamesList " << _fileNamesList << endl;
  if (DebugLevel >= 0) {
    if (_fileNamesList == NULL) {
      cerr << "\n[" << progName << " -   ERROR] No data file selected. Use \'File\' menu." << endl;
      return;
    }
  }

  if (DebugLevel >= 0) {
    cout << "\n[" << progName << " - MESSAGE] Input files list:" << endl;
    TObjString *el;
    TIter next(_fileNamesList);
    while ((el = (TObjString *) next())){
      cout << "                     " << el->GetString() << endl;
    }
  }

  tEventAnalysis = new TEventAnalysis();
  tEventAnalysis->SetDatafilesList(_fileNamesList);
  tEventAnalysis->SetWorkingdir(workingdir);
  Float_t guiW1 = atof(fW1->GetString());
  Float_t guiW2 = atof(fW2->GetString());
  Float_t guiW3 = atof(fW3->GetString());
  Float_t guiW4 = atof(fThresholdBuf->GetString());
  Int_t guiW5 = ThfixFlag;
  cout << "guiW1 " << guiW1 << "  W1 " << atof(W1.Data()) << endl;
  tEventAnalysis->SetDefaults(guiW1, guiW2, guiW3, guiW4, guiW5);

  tEventAnalysis->Init(DebugLevel, progName, NULL);
  DataPanelDisable();
  Int_t startEv = 0, stopEv = 0;
  startEv = atoi(fEvData->GetString());
  if (TString(fEvData1->GetString()) == "All") stopEv = -999;
  else stopEv = atoi(fEvData1->GetString());

  tEventAnalysis->DataAnalysis(DebugLevel, startEv, stopEv);
  //delete tEventAnalysis;
  DataPanelEnable();
  return;
}


void TMainGUI::DataDisplay()
{
  EventDisplay = new TEventDisplay(fClient->GetRoot(), this, 300, 300);
  return;
}


void TMainGUI::CloseWindow() {
   gApplication->Terminate(0);
   return;
}

void TMainGUI::DataPanelDisable()
{
  fDataEvEntry->SetState(false);
  fDataEvEntry1->SetState(false);
  fAnalizeDataButton->SetState(kButtonDisabled);
  fDisplayEventButton->SetState(kButtonDisabled);
  fDataThresholdEntry->SetState(false);
  fHeadToFile->SetEnabled(false);
  fSradiusEntry->SetState(false);
  fWradiusEntry->SetState(false);
  fWeightEntry->SetState(false);
  return;
 }

void TMainGUI::DataPanelEnable()
{
  fDataEvEntry->SetState(true);
  fDataEvEntry1->SetState(true);
  fAnalizeDataButton->SetState(kButtonUp);
  fDisplayEventButton->SetState(kButtonUp);
  fDataThresholdEntry->SetState(true);
  fHeadToFile->SetEnabled(true);
  fSradiusEntry->SetState(true);
  fWradiusEntry->SetState(true);
  fWeightEntry->SetState(true);
  return;
 }

void TMainGUI::Init()
{
  ifstream infofilein;
  if ((!gSystem->AccessPathName("info.dat", kFileExists))){
    infofilein.open("info.dat",ios::in);
    infofilein >> W1 >> W1;
    infofilein >> W2 >> W2;
    infofilein >> W3 >> W3;
    infofilein >> W4 >> W4; 
    infofilein >> W5 >> W5;
  }
  else {         // default conditions
    W1 = "1.5";  // Small Radius
    W2 = "3.5";  // Wide Radius
    W3 = "0.05"; // Weight
    W4 = "11";   // Threshold  
    W5 = "1";    // Fixed Threshold Flag
  }
}

ClassImp(TMainGUI)
