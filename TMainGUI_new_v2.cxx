#include "TMainGUI_new_v2.h"

TMainGUI::TMainGUI(const TGWindow *p, UInt_t w, UInt_t h):TGMainFrame(p, w, h){
  
  ifstream infofilein;
  //dir0 = gSystem->pwd();
  //cout << dir0 << endl;
  workingdir = gSystem->WorkingDirectory();
  cout <<"=====>>> " <<  workingdir << endl;
  if ((!gSystem->AccessPathName("info.dat", kFileExists))){
    infofilein.open("info.dat",ios::in);
    infofilein >> W1 >> W1;
    infofilein >> W2 >> W2;
    infofilein >> W3 >> W3;
    infofilein >> W4 >> W4; 
    infofilein >> W5 >> W5;
    infofilein >> W6 >> W6;
    infofilein >> W7 >> W7;
    infofilein >> W8 >> W8;
  }
  else { // default conditions
    W1 = "1.5";  // Small Radius
    W2 = "3.5";  // Wide Radius
    W3 = "0.05"; // Weight
    W4 = "11";   // Threshold  
    W5 = "1";    // Fixed Thr. Flag
    W6 = "0";    // Raw signal Flag
    W7 = "0";    // FullFrame Flag
    W8 = "0";    // Pedsub Flag
  }
  // Create layout for menubar and popup menus.	
  fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1);
  fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
  fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);
 
  fPopupMenu = new TGPopupMenu(gClient->GetRoot());
  fPopupMenu->AddEntry("&Print", PRINT);
  fPopupMenu->AddSeparator();
  fPopupMenu->AddEntry("E&xit", WEXIT);
  fPopupMenu->DisableEntry(PRINT);
  //Menu button messages are handled by the main frame (i.e. "this")
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
  fBottomFrame = new TGCompositeFrame(fMainFrameVert, 150, 20, kVerticalFrame | kRaisedFrame);
  fFilePFrame = new TGCompositeFrame(fBottomFrame, 120, 20, kHorizontalFrame | kRaisedFrame);
  fFileDFrame = new TGCompositeFrame(fBottomFrame, 120, 20, kHorizontalFrame | kRaisedFrame);

  fPedestalFrame = new TGGroupFrame(fRightFrame, new TGString("Pedestal"));
  fPedEvFrame = new TGCompositeFrame(fPedestalFrame, 150, 150, kHorizontalFrame);
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
  //fTitle->SetTextColor(kBlue,kTRUE);
  fLeftFrame->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 4, 4, 40, 0));

  fLogo = gClient->GetPicture( "crab.xpm" );
  fLogoImageMap = new TGImageMap( fLeftFrame, fLogo );
  fLeftFrame->AddFrame(fLogoImageMap, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY));

  //-----------------PEDESTALS----------------------------------------------------------------
  // Add Buttons in Right Frame
  fAnalizePedsButton = new TGTextButton(fPedEvFrame, "Analyse peds");
  fAnalizePedsButton->Connect("Clicked()","TMainGUI",this,"PedAnalysis()");
  fAnalizePedsButton->SetToolTipText("Press here to analize peds");
  fPedEvFrame->AddFrame(fAnalizePedsButton, fButtonLayout);
  fTitleEv = new TGLabel(fPedEvFrame, new TGString("     Events:"));
  fPedEvFrame->AddFrame(fTitleEv, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4, 4, 4, 4));
  fPedEvEntry = new TGTextEntry(fPedEvFrame, fEvPed = new TGTextBuffer(80));
  fEvPed->AddText(0,"All");
  fPedEvEntry->SetToolTipText("Input Number of pedestal events");
  fPedEvFrame->AddFrame(fPedEvEntry, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 4, 4, 4, 4));
  fPedestalFrame->AddFrame(fPedEvFrame, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));
  
  fDisplayPedsButton = new TGTextButton(fPedestalFrame, "Display peds");
  fDisplayPedsButton->Connect("Clicked()","TMainGUI",this,"PedDisplay()");
  fDisplayPedsButton->SetToolTipText("Press here to display peds");
  fPedestalFrame->AddFrame(fDisplayPedsButton, fButtonLayout);

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

  fpedsub = new TGCheckButton(fDataFrame, "Subtract Pedestals", PEDSUB);
  fpedsub->Associate(this);
  if (atoi(W8) ) {
    fpedsub->SetState(kButtonDown);
    PedsubFlag = 1;
  }
  else {
    fpedsub->SetState(kButtonUp);
    PedsubFlag = 0;
  }
  fDataFrame->AddFrame(fpedsub,new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4, 4, 4, 4));
  
  fDataThresholdFrame = new TGCompositeFrame(fDataFrame, 150, 150, kHorizontalFrame);
  fDataThresholdLabel = new TGLabel(fDataThresholdFrame, new TGString("Noise Thr.(n sigmas)"));
  fDataThresholdFrame->AddFrame(fDataThresholdLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4, 4, 4, 4));
  fDataThresholdEntry = new TGTextEntry(fDataThresholdFrame, fThresholdBuf = new TGTextBuffer(80));
  fThresholdBuf->AddText(0,W4);
  fDataThresholdEntry->SetToolTipText("Insert the threshold for noise rejection (number of STDs)");
  fDataThresholdFrame->AddFrame(fDataThresholdEntry, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 4, 4, 4, 4));
 
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
  fThfix->SetEnabled(false);

  fDataThresholdFrame->AddFrame(fThfix, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4, 4, 4, 4));
  fDataFrame->AddFrame(fDataThresholdFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));
  
  fDataFrame->AddFrame(fCheck1 = new TGCheckButton(fDataFrame, "Save Raw Signal", CHKRAW), fButtonLayout );
  fCheck1->Associate(this);
  if (atoi(W6) ) {
    fCheck1->SetState(kButtonDown);
    RawFlag = 1;
  }
  else {
    fCheck1->SetState(kButtonUp);
    RawFlag = 0;
  }
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
  fRightFrame->AddFrame(fPedestalFrame, fAnalysisLayout);
  fRightFrame->AddFrame(fDataFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 4, 4, 0, 0));
 
  //---------------------------------------------------------------------------------
  // Check if files .LastPed and .LastData with the filename of Pedestal and data file exist!
  
  ifstream mFileName, nFileName;

  if (!gSystem->AccessPathName( ".LastPed", kFileExists )) {
    mFileName.open(".LastPed", ios::in);
    //cout << ".LastPed opened." << endl;
    mFileName >> pName;
    sprintf(fName,"%s",pName);
    //cout << fName << endl;
    mFileName.close();
  }
  else {
    cout << "Pedestal filename unknown!!" << endl;
    sprintf(fName,"%s"," ");
  }

  if (!gSystem->AccessPathName( ".LastData", kFileExists )) {
    nFileName.open(".LastData", ios::in);
    //cout << ".LastData opened." << endl;
    nFileName >> dName;
    sprintf(nName,"%s",dName);
    cout << nName << endl;
    nFileName.close();
  }
  else {
    cout << " Data filename unknown!!" << endl;
    sprintf(nName,"%s"," ");
  }

  //---------------------------------------------------------------------------------
  // Add Text Entries in Bottom Frame

  fTitle1 = new TGLabel(fFilePFrame, new TGString("Peds FileName:"));
  fFilePFrame->AddFrame(fTitle1, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4, 4, 4, 4));
  fPedFileEntry = new TGTextEntry(fFilePFrame, fPName = new TGTextBuffer(80));
  fPName->AddText(0,fName);
  fPedFileEntry->SetToolTipText("Input Pedestal Filename");
  fFilePFrame->AddFrame(fPedFileEntry, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 4, 4, 4, 4));

  fFileSel = fClient->GetPicture("folder_t.xpm");
  fPedSel = new TGPictureButton(fFilePFrame, fFileSel);
  fPedSel->Connect("Clicked()","TMainGUI",this,"PedSelect()");
  fFilePFrame->AddFrame(fPedSel, new TGLayoutHints(kLHintsRight| kLHintsTop, 4, 4, 4, 4));  
  fBottomFrame->AddFrame(fFilePFrame, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));

  fTitle2 = new TGLabel(fFileDFrame, new TGString("Data FileName:"));
  fFileDFrame->AddFrame(fTitle2, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4, 4, 4, 4));
  fDataFileEntry = new TGTextEntry(fFileDFrame, fDName = new TGTextBuffer(80));
  fDName->AddText(0,nName);
  fDataFileEntry->SetToolTipText("Input Data Filename");
  fFileDFrame->AddFrame(fDataFileEntry, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 4, 4, 4, 4));  

  fDataSel = new TGPictureButton(fFileDFrame, fFileSel);
  fDataSel->Connect("Clicked()","TMainGUI",this,"DataSelect()");
  fFileDFrame->AddFrame(fDataSel, new TGLayoutHints(kLHintsRight| kLHintsTop, 4, 4, 4, 4));

  fBottomFrame->AddFrame(fFileDFrame, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));

  //---------------------------------------------------------------------------------
  // Compose frames in window
  fMainFrameHor->AddFrame(fLeftFrame, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY, 0, 0, 0, 0));
  fMainFrameHor->AddFrame(fRightFrame, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY, 0, 0, 0, 0)); 
  fMainFrameVert->AddFrame(fMainFrameHor, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY, 0, 0, 0, 0));
  fMainFrameVert->AddFrame(fBottomFrame, new TGLayoutHints(kLHintsExpandX| kLHintsTop, 0, 0, 0, 0));
  AddFrame(fMainFrameVert, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY, 4, 4, 4, 4));
  
  MapSubwindows();
  Resize(GetDefaultSize());
  SetWindowName("Pixy Control Panel (v5)");
  Resize(600, 500);
  MapWindow();
  Move(100,10);
  RawFlag = 0;
}


TMainGUI::~TMainGUI() {

  if (fHistosCanvas) delete fHistosCanvas;
  delete fMainFrameHor;
  delete fMainFrameVert;
  delete fPedestalFrame;
  delete fPedEvFrame;
  delete fDataFrame;
  delete fDataEvFrame;
  delete fDataThresholdLabel;
  delete fDataThresholdFrame;
  delete fDataThresholdEntry;
  delete fThresholdBuf;
  delete fRightFrame;
  delete fLeftFrame;
  delete fFilePFrame;
  delete fFileDFrame;
  delete fBottomFrame;
  delete fButtonLayout;
  delete fAnalysisLayout;
  delete fMenuBarLayout;
  delete fMenuBarItemLayout;
  delete fPopupMenu;
  delete fMenuBar;
  delete PedsAnalizedFile;
  delete fTree;
  delete DataAnalizedFile;
  delete fAnalizePedsButton;
  delete fDisplayPedsButton;
  delete fAnalizeDataButton;
  delete fDisplayEventButton;
  delete fCheck1;
  delete fPedFileEntry;
  delete fPedEvEntry;
  delete fDataFileEntry; 
  delete fDataEvEntry;
  delete fPName;
  delete fDName;
  delete fEvPed;
  delete fEvData;
  delete fTitle;
  delete fTitle1;
  delete fTitle2;
  delete fTitleEv;
  delete fTitleDataEv;
  delete fLogoImageMap;
  delete EventDisplay;
}

Bool_t TMainGUI::ProcessMessage(Long_t msg, Long_t par1, Long_t){

  if (GET_MSG(msg) == kC_COMMAND) {
    if (GET_SUBMSG(msg) == kCM_BUTTON || GET_SUBMSG(msg) == kCM_CHECKBUTTON ) {
      switch(par1) {

      case THFIX:
	if (fThfix->GetState()) ThfixFlag = 1;
	else ThfixFlag = 0;
	break;
	
      case PEDSUB:
	if (fpedsub->GetState()){
	  PedsubFlag = 1;
	  cout << "pedsub on" << endl;
	}
	else {
	  PedsubFlag = 0;
	  cout << "pedsub off" << endl;
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

      case  CHKRAW:
	if (fCheck1->GetState()) {
	RawFlag = 1;
	cout << "\n===>> Raw Signals will be saved in file RawSignals.root!!"<< endl; 
	box = new TGMsgBox(gClient->GetRoot(), gClient->GetRoot(),"Message", 
			   Form("Raw Signals will be saved in file RawSignals.root ONLY for data taken in Window mode!!!",0)
			   ,icontype, buttons, &retval);
	}
	else RawFlag = 0;
	  break;

      default:break;
      }
    }
    if (GET_SUBMSG(msg) == kCM_MENU) {
      switch(par1) {
               
	case PRINT:
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

void TMainGUI::PedSelect()
  {
    const char *filetypes[] = { "All files",     "*",
				"ROOT files",    "*.root",
				"ROOT macros",   "*.C",
				0,               0 };
    static TString dir(".");
    using namespace std;
    TGFileInfo fi1;
    fi1.fFileTypes = filetypes;
    fi1.fIniDir    = StrDup(dir);
    new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi1);
    if(!fi1.fFilename) {
      cout << "No Pedestal File selected! Exit!!!" << endl;
      box = new TGMsgBox(gClient->GetRoot(), gClient->GetRoot(),"Message", 
			   Form("No Pedestal File selected! Exit!!!!!",0)
			   ,kMBIconStop, buttons, &retval);
      return;
    }
    fPName->Clear();
    fPName->AddText(0, fi1.fFilename);
    dir = fi1.fIniDir;
    fPedFileEntry->SetFocus();
    pFileName.open(".LastPed", ios::out);
    pFileName << fi1.fFilename;
    pFileName.close();
  }

void TMainGUI::PedAnalysis()
  {
    //PedPanelDisable();
    gStyle->SetOptFit(1111);
    Char_t tmp1,tmp2;
    Int_t Dummy;
    //Int_t NumberOfEvts;
    Char_t HistogramName[20];
  
    Double_t* Mean = NULL;
    Double_t* RMS = NULL;
    Double_t* Average = NULL;
    Double_t* Sigma = NULL;
    Axis_t* Chans = NULL;
    Mean = new Double_t[NCHANS];
    RMS = new Double_t[NCHANS];
    Average = new Double_t[NCHANS];
    Sigma = new Double_t[NCHANS];
    Chans = new Axis_t[NCHANS];

    PedName  = (Text_t *)fPName->GetString( );
    pFileName.open(".LastPed", ios::out);
    pFileName << PedName;
    pFileName.close();
    if (!gSystem->AccessPathName( PedName, kFileExists )) {
      cout << "====> Opening Pedestal File: " << PedName << endl;	  
      inPeds.open(PedName,ios::in|ios::binary);
      Int_t nPed = 0;
      Int_t nEvPed = 100000000;
      if (TString(fEvPed->GetString()) != "All") nEvPed = atoi(fEvPed->GetString());
      TStopwatch timer;
      timer.Start();
   
      cout << "=============>>> BOOKING.....";
      MeanPedsHisto = new TH1F("MeanPeds", "Mean values", NCHANS, 0, NCHANS);
      RMSPedsHisto = new TH1F("RMSPeds", "RMS values", NCHANS, 0, NCHANS);	  
      AveragePedsHisto = new TH1F("AveragePeds", "Average values", NCHANS, 0, NCHANS);
      SigmaPedsHisto = new TH1F("SigmaPeds", "Sigma values", NCHANS, 0, NCHANS);
      SigmaPedsHisto->SetMinimum(0);
      for (Int_t ch=0; ch<NCHANS; ch++){
	sprintf(HistogramName, "RawPedsHistos%d", ch);
	RawPedsHistos[ch]  = new TH1S(HistogramName, "", 500,400,1400);
      }
      cout << "  done! " << endl;

      cout << "==========>>> READING DATA & FILLING........" << endl;	   
      while(1){
	gSystem->ProcessEvents();
	if(!inPeds.good())break;
	nPed++;
	if (!(nPed%50)) cout << nPed << " events read " << endl;
	for (int ch=0; ch<CHxCLU ;ch++){
	  for (int clu=0; clu<NCLU ;clu++){
	    inPeds.read((char*)&tmp1, sizeof(char));
	    inPeds.read((char *)&tmp2, sizeof(char));
	    if(clu ==0 && ch <=10 && nPed == 1)cout << "cluster:  " << clu 
	      					    << " --- Ped: " << COMB(tmp1,tmp2) << endl;
	    if(clu == 0)RawPedsHistos[ch]->Fill(COMB(tmp1,tmp2));
	    if(clu == 1)RawPedsHistos[ch+CHxCLU]->Fill(COMB(tmp1,tmp2));
	    if(clu == 2)RawPedsHistos[ch+CHxCLU*2]->Fill(COMB(tmp1,tmp2));
	    if(clu == 3)RawPedsHistos[ch+CHxCLU*3]->Fill(COMB(tmp1,tmp2));
	    if(clu == 4)RawPedsHistos[ch+CHxCLU*4]->Fill(COMB(tmp1,tmp2));
	    if(clu == 5)RawPedsHistos[ch+CHxCLU*5]->Fill(COMB(tmp1,tmp2));
	    if(clu == 6)RawPedsHistos[ch+CHxCLU*6]->Fill(COMB(tmp1,tmp2));
	    if(clu == 7)RawPedsHistos[ch+CHxCLU*7]->Fill(COMB(tmp1,tmp2));
	  }
	}
	if(nEvPed && nPed == nEvPed) break;
      }
      cout << ".....Done!!" << endl;
      
      cout << "========> Plotting Mean and RMS!!!! " << endl;
      
      for (Int_t ch=0; ch<NCHANS; ch++){ 
	Mean[ch] = (Float_t)(RawPedsHistos[ch]->GetMean(1));
	RMS[ch] = (Float_t)(RawPedsHistos[ch]->GetRMS(1));
	Chans[ch]  = (Axis_t)ch;
      }
      
      MeanPedsHisto->FillN(NCHANS, Chans, Mean, 1);
      RMSPedsHisto->FillN(NCHANS, Chans, RMS, 1);
      
      cout << "========> FITTING & PLOTTING !!!! " << endl;
      Double_t max, min;
      max = 0;
      min = 10000;
      for (Int_t ch=0; ch<NCHANS; ch++){ 
	gSystem->ProcessEvents();
	double hrms = RMS[ch];
	double hmean = Mean[ch];
	if (hrms > 5) hrms = 5;
	Int_t Min = (Int_t)(hmean - 10*hrms);
	Int_t Max = (Int_t)(hmean + 10*hrms);
	gfit = new TF1("Fitname", "gaus", Min, Max);  
	RawPedsHistos[ch]->Fit(gfit, "RQO");
	Average[ch]  =  gfit->GetParameter(1);
	Sigma[ch] =  gfit->GetParameter(2);
	delete gfit;
	gfit = 0;
	delete RawPedsHistos[ch];
	RawPedsHistos[ch] = 0;
	if (!(ch%1000)) cout << ch << "  channels fitted" << endl;
      }
      
      timer.Stop();
      timer.Print("m");
      Canvas = new TCanvas("c1","FittedPedestals",0,0,900,500);
      Canvas->Divide(1,2);
      AveragePedsHisto->FillN(NCHANS, Chans, Average, 1);
      SigmaPedsHisto->FillN(NCHANS, Chans, Sigma, 1);
      Canvas->cd(1);
      AveragePedsHisto->Draw("");
      Canvas->cd(2);
      SigmaPedsHisto->Draw("");
      
      Canvas1 = new TCanvas("c2","noise distribution",0,0,700,400);
      TH1F *noise = new TH1F("noiseDistr", "Noise distribution",100,0.,10.);
      for(int i=0;i<NCHANS;i++)noise->Fill(SigmaPedsHisto->GetBinContent(i+1));
      noise->Draw();
       
      PedsAnalizedFile = new TFile ("Pedestal.root", "RECREATE");
      cout << "Saving Pedestal Histograms in Pedestal.root file!!!......" << endl;
      AveragePedsHisto->Write();
      SigmaPedsHisto->Write();
      noise->Write();
      MeanPedsHisto->Write();
      RMSPedsHisto->Write();

      cout << "Saving Pedestal Means and Sigmas in LastPeds.dat file!!!......" ;
      outPeds.open("LastPeds.dat",ios::out|ios::binary);
      outPeds.write((Char_t *)Average, NCHANS * sizeof(Double_t));
      outPeds.write((Char_t *)&Dummy, sizeof(Int_t));
      outPeds.write((Char_t *)Sigma, NCHANS * sizeof(Double_t));
      outPeds.close();
      cout << ".... Done! " << endl;
      }
    else {
      cout << "Pedestal File: " << PedName << " not existing!!!" << endl;
    }
    delete [] Mean;
    delete [] RMS;
    delete [] Chans;
    delete [] Sigma;
    delete [] Average;
    //PedPanelEnable();    
}

void TMainGUI::PedDisplay()
{
  if (fpedsFile)fpedsFile->Close();
  if (!gSystem->AccessPathName("Pedestal.root", kFileExists )) {
    if (!PedsAnalizedFile){
      PedsAnalizedFile = new TFile("Pedestal.root");
      cout << "===> Opening root file to display Histos" << endl;
    }	
    if (!gROOT->GetListOfCanvases()->FindObject("Histo_window"))
      {
	fHistosCanvas = new TCanvas("Histo_window", "Display_Histos", 20, 20, 800, 800);
	cout << "Creating canvas to display pedestals" << endl;
      }
    fHistosCanvas->SetFillColor(10);
    fHistosCanvas->Clear();
    fHistosCanvas->Divide(1, 3);
    fHistosCanvas->cd(1);
    AveragePedsHisto = (TH1F*)gDirectory->Get("AveragePeds");
    AveragePedsHisto->Draw();
    fHistosCanvas->cd(2);
    SigmaPedsHisto = (TH1F*)gDirectory->Get("SigmaPeds");
    SigmaPedsHisto->Draw();
    fHistosCanvas->cd(3);
    noise = (TH1F*)gDirectory->Get("noiseDistr");
    noise->Draw();
    fHistosCanvas->Update();
  }
  else cout << "\nPedestal.root not existing!!! ==> Run Analize peds. before" << endl;
}


void TMainGUI::DataSelect()
{
  const char *filetypes[] = { "All files",     "*",
				"ROOT files",    "*.root",
				"ROOT macros",   "*.C",
			      0,               0 };
  static TString dir(".");
  using namespace std;
  TGFileInfo fi;
  fi.fFileTypes = filetypes;
  fi.fIniDir    = StrDup(dir);
  new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
  if(!fi.fFilename) {
      cout << "No Data File selected! Exit!!!" << endl;
      box = new TGMsgBox(gClient->GetRoot(), gClient->GetRoot(),"Message", 
			   Form("No Data File selected! Exit!!!!!",0)
			   ,kMBIconExclamation, buttons, &retval);
      return;
    }
  fDName->Clear();
  fDName->AddText(0, fi.fFilename);
  dir = fi.fIniDir;
  fDataFileEntry->SetFocus();
  TString lastdataname;
  lastdataname = workingdir;
  lastdataname += "/.LastData";
  dFileName.open(lastdataname, ios::out);
  dFileName << fi.fFilename;
  dFileName.close();
}


void TMainGUI::DataAnalysis()
{
  
  //cout << "----->>>" << dir0 << endl;
  //gSystem->cd(dir0);
  
  DataPanelDisable();
  eofdata = 0;
  longEv = 0;
  Int_t totnev = 0, nev;
  Rcounter = 0;
  RTFlag = 0; // O = No EventsTree for MC analisys <<<<<<<<<<<<<============ 
  if (RTFlag) InitializeEventsTree();
  SmallRadius  = atof(fW1->GetString());
  WideRadius   = atof(fW2->GetString());
  Weight = atof(fW3->GetString());
  fPixelThreshold = atof(fThresholdBuf->GetString());
  
  Int_t startEv = 0, stopEv = 0;
  startEv = atoi(fEvData->GetString());
  gSystem->ChangeDirectory(workingdir);

  if ((!gSystem->AccessPathName("RawSignals.root", kFileExists)) && (!RawFlag)) 
    gSystem->Rename("RawSignals.root","RawSignals_old.root");
  DataName  = (Text_t *)fDName->GetString( );	  
  if (!gSystem->AccessPathName(DataName, kFileExists)) {
    cout << DataName << " opened for analysis" << endl;
    MCflag = 0;
    NewDataFlag = 0; // <<<<<<=============== Flag for reading matrix of data from new chip3
    FFflag = atoi(W7);
    if ((TString(fDName->GetString( ))).Contains(".root")) MCflag = 1;        // MonteCarlo data
    if ((TString(fDName->GetString( ))).Contains(".mdat")) NewDataFlag = 1;   // ROI data from new chip3 (Still not ped subtracted)
    if ((TString(fDName->GetString( ))).Contains("ff_")) FFflag = 1;          // Full Frame data
    infofile.open("info.dat",ios::out);
    infofile << "SmallRadius: " << SmallRadius << endl;
    infofile << "WideRadius: " << WideRadius << endl;
    infofile << "Weight: " << Weight << endl;
    infofile << "Threshold: " << fPixelThreshold << endl;
    infofile << "FixThrFlag: " << ThfixFlag << endl;
    infofile << "RawDataFlg: " << RawFlag << endl;
    infofile << "FullFrame: " << FFflag << endl;
    infofile << "pedsub: " << PedsubFlag << endl;
    infofile.close();
    InitializeClusterTree();
    if (RawFlag&&NewDataFlag) InitializeRawSignalTree();
    gSystem->ChangeDirectory(workingdir);
    cout << "======>> " << workingdir << endl;

    if (!gSystem->AccessPathName( "pixmap.dat", kFileExists )) {
      PixFileName = new TInputFile("pixmap.dat", kAscii);

      if (MCflag) {
	TFile *f = new TFile(DataName);
	TTree *t=(TTree*)f->Get("EventsTree");
	if (TString(fEvData1->GetString()) == "All")stopEv=(int)t->GetEntries();
	else stopEv = atoi(fEvData1->GetString());
	cout << "Events to be read in MC tree:  " << stopEv-startEv+1 << endl;
	Polarimeter = new TDetector(t,PixFileName);
	Polarimeter->SetWeight(Weight);
	Polarimeter->SetSmallRadius(SmallRadius);
	Polarimeter->SetWideRadius(WideRadius);
	Polarimeter->fPixelThresh = fPixelThreshold;
	Polarimeter->fThreshFlag = ThfixFlag;
	Polarimeter->fSubPedFlag = PedsubFlag;
	nev = 0;
	for (Int_t i=startEv; i<=stopEv; i++) {
	  f->cd();
	  nev++;
	  gSystem->ProcessEvents(); //Added 29-01-08
	  if (!((i-startEv)%1000)) cout << "========>>> Analized " << i-startEv << " MC events" << endl;
	  eofdata = Polarimeter->ReadMCFile(i);
	  Int_t NbClusters = Polarimeter->FindClusters();
	  if (Polarimeter->GetRcounter()) Rcounter++;
	  if (!NbClusters) nev--;
	  if (RawFlag) SaveRawSignal(nev);
	  SaveClusters(NbClusters,nev);
	  totnev++;
	}
	totnev = totnev + startEv;
      }

      else {
	if (!gSystem->AccessPathName( "LastPeds.dat", kFileExists )) {	      
	  PedFileName = new TInputFile("LastPeds.dat", kBinary);
	  RawFileName = new TInputFile(DataName, kBinary);
	  Polarimeter = new TDetector(PixFileName, PedFileName, RawFileName);
	  if(HeaderOn)Polarimeter->SetHeaderOn();
	  Polarimeter->SetWeight(Weight);
	  Polarimeter->SetSmallRadius(SmallRadius);
	  Polarimeter->SetWideRadius(WideRadius);
	  Polarimeter->fPixelThresh = fPixelThreshold;
	  Polarimeter->fThreshFlag = ThfixFlag;
	  Polarimeter->fSubPedFlag = PedsubFlag;
	  nev = 0;
	  totnev = 0;
	  
	  if (TString(fEvData1->GetString()) == "All") cout << "Read ALL events in file" << endl;
	  else {
	    stopEv = atoi(fEvData1->GetString());
	    cout << stopEv-startEv+1 << " events to be read" << endl;
	  }
	  timer1.Start(kTRUE);

	  while(!Polarimeter->fRawFile->fStream.eof()){
	    if (stopEv && totnev == stopEv+1) break;
	    nev++;
	    totnev++;
	    gSystem->ProcessEvents(); //Added 29-01-08
	    
	    if(FFflag) eofdata =  Polarimeter->ReadFullFrame(totnev);
	    // (!NewDataFlag) eofdata = Polarimeter->ReadRawFile();// <<<<<<== Flag for reading matrix of data from new chip3
	    // else eofdata =  Polarimeter->ReadROFile(totnev);
	    else eofdata =  Polarimeter->ReadROInew(totnev);
	    
	    if(eofdata==1) {
	      cout << "===========>>>> Event : " << totnev << "  end of file!!" << endl;
	      break;
	    }
	    if(eofdata==2){
	      cout << totnev << " bad window ==> event rejected " << endl;
	      longEv++;
	      nev--;  
	      continue;
	    }
	    
	    if(totnev < startEv){
	      nev = 0;
	      continue;
	    }
	    
	    if (!((totnev-startEv)%1000)) cout << (totnev-startEv) << " events analized starting from ev: " << totnev << endl;

	    Int_t NbClusters = Polarimeter->FindClusters();
	    if (Polarimeter->GetRcounter()) Rcounter++;
	    //SaveRawSignal(nev);
	    if (!NbClusters)
	      {
		//cout << "nev:  " << nev << "   NClusters:  " << NbClusters << " ---> NO CLUSTER FOUND in tot ev " << totnev <<  endl;
		nev--;  
		continue;
	      }
	    else {
	      //cout << " OK nev: " << nev << " toteV " << totnev << endl;
	      if (RTFlag && NbClusters==1) SaveEventstree(NbClusters,nev);
	      if (RawFlag && NbClusters>=1 && NewDataFlag) SaveRawSignal(nev);
	      if (NbClusters>=1) SaveClusters(NbClusters,nev);
	    }
	  }// end while
	}
	else {
	  cout << " LastPeds.dat file not existing!!! Run Analize peds" << endl;
	  return;
	}
      }

      cout << "===>>> end loop on events" << endl;
      timer1.Stop();
      timer1.Print("m");
      //WriteRawSignalTree();
      cout << " +++++  Rejected for numPixels Xeedoing MAX transf. : " << longEv << endl;
      cout << nev << " events found over a total of " << (totnev-startEv) << " (" << (Float_t)(nev*100)/(totnev-startEv) << "%)" << endl;	
      //if(nev ==0)break;
      cout << "Impact points within selected circles: " << Rcounter << " -->> " << Float_t (Rcounter*100/nev) << " %" << endl;
      if (RTFlag&&nev) WriteEventsTree();
      WriteClusterTree();
      if (RawFlag&&NewDataFlag&&nev) WriteRawSignalTree();
      if(!FFflag&&nev)DrawCumulativeHitMap(nev);
      CloseFiles();	    
    }
    else {
      cout << "Pixmap file not existing!!!" << endl;
      return;
    }
  }
  else {
    cout << "Data File: " << DataName << " not existing!!!" << endl;
    DataPanelEnable();
    return;
  }
  DataPanelEnable();
}


void TMainGUI::DataDisplay()
{
  cout << "Data Display " << endl;
  /*
  if (gROOT->GetListOfCanvases()->FindObject(fHistosCanvas)){
    fHistosCanvas->Close();
    delete fHistosCanvas;
  }
  */
  EventDisplay = new TEventDisplay(fClient->GetRoot(), this, 300, 300);
  //if(!EventDisplay) cout << "==>>> ClustersData.root not existing !!!! " << endl;
}

void TMainGUI::CloseFiles(){
  
  PixFileName->fStream.close();
  delete PixFileName;
  if (!MCflag) {
    PedFileName->fStream.close();
    RawFileName->fStream.close();
    delete PedFileName;
    delete RawFileName;
  }
  delete Polarimeter;
}

void TMainGUI::WriteClusterTree(){
  DataAnalizedFile->cd();
  fTree->Write();
  cout << "Writing all clusters found in root file ==>> ClustersData.root" << endl;
  delete fTree;
  DataAnalizedFile->Close();
  delete DataAnalizedFile;
  }

void TMainGUI::WriteRawSignalTree(){
  RawSignalFile->cd();
  fRawTree->Write();
  cout << "Writing Raw Signals in root file ==>> RawSignals.root" << endl;
  delete fRawTree;
  RawSignalFile->Close();
  delete RawSignalFile;
  }

void TMainGUI::WriteEventsTree(){
  EventsFile->cd();
  tree->Write();
  cout << "Writing real data clusters for MC analysis in root file ==>> Eventstree.root" << endl;
  delete tree;
  EventsFile->Close();
  delete EventsFile;
  }

void TMainGUI::InitializeClusterTree() {
  DataAnalizedFile = new TFile("ClustersData.root", "RECREATE");
  cout << ".........Initializing Clusters Tree........";
  fTree = new TTree("tree","Analized Data Tree");
  fTree->Branch("fEventId", &fEventId, "fEventId/I");
  fTree->Branch("fNClusters", &fNClusters, "fNClusters/I");
  fTree->Branch("fTrigWindow", &fTrigWindow, "fTrigWindow/I");
  fTree->Branch("fTimeStamp",&fTimeStamp,"fTimeStamp/D");
  fTree->Branch("fbufferId",&fbufferId,"fbufferId/I");
  fTree->Branch("fCluSize", &fCluSize, "fCluSize[fNClusters]/I"); 
  fTree->Branch("fXpixel", fXpixel,"fXpixel[fNClusters][400]/F"); //One dim MUST be fixed   
  fTree->Branch("fYpixel", fYpixel,"fYpixel[fNClusters][400]/F"); //Chosen 400 as the MAXCLUSIZE
  fTree->Branch("fUpixel", fUpixel,"fUpixel[fNClusters][400]/I"); //One dim MUST be fixed   
  fTree->Branch("fVpixel", fVpixel,"fVpixel[fNClusters][400]/I"); //Chosen 400 as the MAXCLUSIZE
  fTree->Branch("fPHpixel", fPHpixel,"fPHpixel[fNClusters][400]/F"); 
  fTree->Branch("fPHeight", fPHeight, "fPHeight[fNClusters]/F");
  fTree->Branch("fStoN", fStoN, "fStoN[fNClusters]/F");
  fTree->Branch("fTotNoise", fTotNoise, "fTotNoise[fNClusters]/F");
  fTree->Branch("fHighestC", fHighestC, "fHighestC[fNClusters]/F");
  fTree->Branch("fBaricenterX", fBaricenterX, "fBaricenterX[fNClusters]/F");
  fTree->Branch("fBaricenterY", fBaricenterY, "fBaricenterY[fNClusters]/F");
  fTree->Branch("fTheta0", fTheta0, "fTheta0[fNClusters]/F");
  fTree->Branch("fTheta1", fTheta1, "fTheta1[fNClusters]/F");
  fTree->Branch("fThetaDifference", fThetaDifference, "fThetaDifference[fNClusters]/F");
  fTree->Branch("fMomX", fMomX, "fMomX[fNClusters]/F");
  fTree->Branch("fMomY", fMomY, "fMomY[fNClusters]/F");
  fTree->Branch("fMomThirdX", fMomThirdX, "fMomThirdX[fNClusters]/F");
  //fTree->Branch("fMomThirdY", fMomThirdY, "fMomThirdY[fNClusters]/F");
  fTree->Branch("fImpactX", fImpactX, "fImpactX[fNClusters]/F");
  fTree->Branch("fImpactY", fImpactY, "fImpactY[fNClusters]/F");
  fTree->Branch("fThetaR", fThetaR, "fThetaR[fNClusters]/D");
  fTree->Branch("fImpactXR", fImpactXR, "fImpactXR[fNClusters]/F");
  fTree->Branch("fImpactYR", fImpactYR, "fImpactYR[fNClusters]/F");
  fTree->Branch("fCluLenght",fCluLenght,"fCluLenght[fNClusters]/F");
  cout << "...done! " << endl;
}


void TMainGUI::InitializeRawSignalTree() {
  RawSignalFile = new TFile("RawSignals.root", "RECREATE");
  cout << ".........Initializing RawSignals Tree........ONLY for events read in Window mode!!! ";
  nchans = GetNchans();
  fRawTree = new TTree("RawSignaltree","Raw Signal");
  fRawTree->Branch("fEventId", &fEventId, "fEventId/I");
  fRawTree->Branch("nchans",&nchans,"nchans/I");
  fRawTree->Branch("fRawSignal",fRawSignal, "fRawSignal[nchans]/F");
  cout << "...done! " << endl;
}

void TMainGUI::InitializeEventsTree(){
  EventsFile = new TFile("Eventstree.root", "RECREATE");
  cout << ".........Initializing EventsTree with real data for MC analysis........";
  tree = new TTree("EventsTree","data");
  tree->Branch("Nevent",&Nevent,"Nevent/I");
  tree->Branch("Clusterdim",&Clusterdim,"Clusterdim/I");
  tree->Branch("Channel",Channel,"Channel[Clusterdim]/I");
  tree->Branch("Charge",Charge,"Charge[Clusterdim]/I");
  tree->Branch("DigiCharge",DigiCharge,"DigiCharge[Clusterdim]/I");
  tree->Branch("InitialPhi",&Phi,"Phi/D");
  //tree->Branch("XInitial",&XInitial,"XI/D");
  //tree->Branch("YInitial",&YInitial,"YI/D");
  cout << "...done! " << endl;
}

void TMainGUI::SaveEventstree(Int_t NbClusters, Int_t nev){
  Int_t u,v;
  Nevent   = nev;
  Nevent   = 0;
  Phi = Polarimeter->fAllClusts[0]->fTheta2;
  Clusterdim =  Polarimeter->fAllClusts[0]->fClusterSize;
    if (Clusterdim >= 0) {
      for (Int_t j=0; j<Clusterdim; j++){
	u = (Int_t)Polarimeter->fAllClusts[0]->fPixelsInCluster[j][0];
	v = (Int_t)Polarimeter->fAllClusts[0]->fPixelsInCluster[j][1];
	Channel[j]   = (Int_t)Polarimeter->fPixMap[u][v];
	Charge[j] = (Int_t) (Polarimeter->fSignalMatrix[u][v]);
	//Nevent += Charge[j];
	DigiCharge[j] = 1400 - Charge[j]; //
      }
      tree->Fill();
    }
}

void TMainGUI::SaveClusters(Int_t NbClusters, Int_t nev){
  fEventId   = nev;
  fNClusters = NbClusters;
  fTrigWindow = Polarimeter->numPix;
  fTimeStamp = Polarimeter->timestamp;
  fbufferId = Polarimeter->bufferID;
  for (Int_t i=0; i<NbClusters; i++){
    fPHeight[i]            = Polarimeter->fAllClusts[i]->fPulseHeight;
    fStoN [i]              = Polarimeter->fAllClusts[i]->fSignalToNoise;
    fTotNoise[i]           = Polarimeter->fAllClusts[i]->fTotalNoise;
    fHighestC [i]          = Polarimeter->fAllClusts[i]->fHighestCharge;
    fBaricenterX[i]        = Polarimeter->fAllClusts[i]->fCenterOfGravityX;
    fBaricenterY[i]        = Polarimeter->fAllClusts[i]->fCenterOfGravityY;
    fTheta0[i]             = Polarimeter->fAllClusts[i]->fTheta;
    fTheta1[i]             = Polarimeter->fAllClusts[i]->fTheta2;
    fThetaDifference[i]    = Polarimeter->fAllClusts[i]->fThetaDiff;
    fMomX[i]               = Polarimeter->fAllClusts[i]->fMomentumX;
    fMomY[i]               = Polarimeter->fAllClusts[i]->fMomentumY;
    fMomThirdX[i]          = Polarimeter->fAllClusts[i]->fThirdMomentumX;
    //fMomThirdY[i]          = Polarimeter->fAllClusts[i]->fThirdMomentumY;
    fImpactX[i]            = Polarimeter->fAllClusts[i]->fImpactPosX;
    fImpactY[i]            = Polarimeter->fAllClusts[i]->fImpactPosY;
    fCluSize[i]            = Polarimeter->fAllClusts[i]->fClusterSize;
    fCluLenght[i]          = Polarimeter->fAllClusts[i]->cluLenght;
    if (MCflag) {
      fThetaR[i] = Polarimeter->Phi;
      fImpactXR[i]= Polarimeter->XI;
      fImpactYR[i]= Polarimeter->YI;
     }
    else        {
      fThetaR[i] = 0;
      fImpactXR[i]= 0;
      fImpactYR[i]= 0;
    }
    for (Int_t j=0; j<fCluSize[i]; j++){
      Float_t x = Polarimeter->fAllClusts[i]->fPixelsCoordInCluster[j][0];
      Float_t y = Polarimeter->fAllClusts[i]->fPixelsCoordInCluster[j][1];
      Float_t height = Polarimeter->fSignalMatrix[Polarimeter->fAllClusts[i]->fPixelsInCluster[j][0]]
	[Polarimeter->fAllClusts[i]->fPixelsInCluster[j][1]];
      fUpixel[i][j] = Polarimeter->fAllClusts[i]->fPixelsInCluster[j][0];
      fVpixel[i][j] = Polarimeter->fAllClusts[i]->fPixelsInCluster[j][1];
      fXpixel[i][j] = x;
      fYpixel[i][j] = y;
      fPHpixel[i][j] = height;
    }
    delete Polarimeter->fAllClusts[i];  //free memory
    Polarimeter->fAllClusts[i] = 0;
  }//close loop on clusters!!!
  DataAnalizedFile->cd();
  fTree->Fill();
  ClearArrays(NbClusters);
  return;
}

void TMainGUI::ClearArrays(Int_t NbClusters){
  for (Int_t i=0; i<NbClusters; i++){
    fPHeight[i] = fStoN [i] = fTotNoise[i] = fHighestC [i]= 0;
    fBaricenterX[i] = fBaricenterY[i] = 0;
    fTheta0[i] = fTheta1[i] = fThetaDifference[i] = 0;
    fMomX[i] = fMomY[i] = fMomThirdX[i] = 0;       
    fImpactX[i] = fImpactY[i] = 0;
    
    fCluLenght[i] = 0;
    fThetaR[i] = 0;
    fImpactXR[i]= 0;
    fImpactYR[i]= 0;
     for (Int_t j=0; j<fCluSize[i]; j++){
      fXpixel[i][j] = fYpixel[i][j] = fPHpixel[i][j] = 0;
      fUpixel[i][j] = fVpixel[i][j] = 0;
    }
    fCluSize[i] = 0;
  }
  return;
}

void TMainGUI::SaveRawSignal(Int_t nev) {
  fEventId   = nev+1;
  //for (Int_t j=0; j<nchans; j++)fRawSignal[j] = Polarimeter->fPedSubtrSignalMask[j];
  for (Int_t j=0; j<nchans; j++)fRawSignal[j] = Polarimeter->fROIRawData[j];
   fRawTree->Fill();
}

void TMainGUI::DrawCumulativeHitMap(Int_t nev){
  
  // Display cumulative Hitmap!
  Float_t DeathThreshold = nev/10.0;
  TCanvas *fCumulativeHitmapCanvas;
  fCumulativeHitmapCanvas = new TCanvas("Cumulative Hitmap", " Cumulative Hitmap", 900, 10, 1000, 500);
  fCumulativeHitmapCanvas->SetFillColor(10);
  TH1F *fCumulativeHitmapHisto;
  fCumulativeHitmapHisto = new TH1F("Cumulative Hitmap", "Cumulative Hitmap", NCHANS, 0, NCHANS);
  Float_t MaxHitmap = 0;
  for (Int_t ch=0; ch<NCHANS; ch++){
    fCumulativeHitmapHisto->SetBinContent(ch+1, Polarimeter->fCumulativeHitmap[ch]);
    if (Polarimeter->fCumulativeHitmap[ch]>MaxHitmap) MaxHitmap = Polarimeter->fCumulativeHitmap[ch];
  }
  fCumulativeHitmapHisto->Draw();

  
  TCanvas *fCumulativeHitmapCanvas2D;
  fCumulativeHitmapCanvas2D = new TCanvas("Cumulative Hitmap 2D", " Cumulative Hitmap 2D", 900, 200, 700, 700);
  fCumulativeHitmapCanvas2D->SetFillColor(10);
  ifstream PxFile;
  PxFile.open("pixmap.dat", ios::in);
  PixelHit pix;
  THexagonCol *hexagon;
  Int_t DummyInt;
  Char_t DummyChar[10];
  Bool_t Border;
  Int_t MediumX, MediumY;
  PxFile >> MediumX >> MediumY;
  gPad->Range(-MediumY*PITCH, -MediumY*PITCH, MediumY*PITCH, MediumY*PITCH);
  PxFile >> DummyChar >> DummyChar >> DummyChar >> DummyChar >> DummyChar >> DummyChar >> DummyChar ;
  for (Int_t ch=0; ch<NCHANS; ch++){
    PxFile >> pix.X;
    PxFile >> pix.Y;
    PxFile >> DummyInt >> DummyInt >> DummyInt >> DummyInt >> Border;
    // If the pixel is on the border, draw a red hexagon, full scale!
    if (Border){
      pix.Height = PITCH/2;
      hexagon = new THexagonCol(pix);
      hexagon->DrawEmpty(kRed);
      delete hexagon;
    }
    if (Polarimeter->fCumulativeHitmap[ch] > DeathThreshold){
      pix.Height = Polarimeter->fCumulativeHitmap[ch]/MaxHitmap*PITCH/2;
      hexagon = new THexagonCol(pix);
      hexagon->DrawEmpty(kBlack);
      delete hexagon;
    }
  }
  
}

void TMainGUI::CloseWindow() {
   // Terminate the application
   gApplication->Terminate(0);
}

void TMainGUI::DataPanelDisable()
{
  fDataEvEntry->SetState(false);
  fDataEvEntry1->SetState(false);
  fPedEvEntry->SetState(false);
  fAnalizeDataButton->SetState(kButtonDisabled);
  fDisplayEventButton->SetState(kButtonDisabled);
  fDataThresholdEntry->SetState(false);
  fCheck1->SetEnabled(false);
  fpedsub->SetEnabled(false);
  fHeadToFile->SetEnabled(false);
  fThfix->SetEnabled(false);
  fSradiusEntry->SetState(false);
  fWradiusEntry->SetState(false);
  fWeightEntry->SetState(false);
  fDataFileEntry->SetState(false);
  fPedFileEntry->SetState(false);
  fDataSel->SetState(kButtonDisabled);
}

void TMainGUI::DataPanelEnable()
{
  fDataEvEntry->SetState(true);
  fDataEvEntry1->SetState(true);
  fPedEvEntry->SetState(true);
  fAnalizeDataButton->SetState(kButtonUp);
  fDisplayEventButton->SetState(kButtonUp);
  fDataThresholdEntry->SetState(true);
  fCheck1->SetEnabled(true);
  fpedsub->SetEnabled(true);
  fHeadToFile->SetEnabled(true);
  fThfix->SetEnabled(true);
  fSradiusEntry->SetState(true);
  fWradiusEntry->SetState(true);
  fWeightEntry->SetState(true);
  fDataFileEntry->SetState(true);
  fPedFileEntry->SetState(true);
  fDataSel->SetState(kButtonUp);
}

ClassImp(TMainGUI)
