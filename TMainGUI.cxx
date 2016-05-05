#include "TMainGUI.h"

TMainGUI::TMainGUI(const TGWindow *p, UInt_t w, UInt_t h):TGMainFrame(p, w, h){
  ifstream infofilein;
  workingdir = gSystem->WorkingDirectory();
  cout <<"Working directory: =====>>> " <<  workingdir << endl;
  if ((!gSystem->AccessPathName("info.dat", kFileExists))){
    infofilein.open("info.dat",ios::in);
    infofilein >> W1 >> W1;
    infofilein >> W2 >> W2;
    infofilein >> W3 >> W3;
    infofilein >> W4 >> W4; 
    infofilein >> W6 >> W6;
  }
  else { // default conditions
    W1 = "1.5";  // Small Radius
    W2 = "3.5";  // Wide Radius
    W3 = "0.05"; // Weight
    W4 = "11";   // Threshold  
    W6 = "0";    // Raw signal Flag
  }

  W7 = "0"; // FullFrame Flag

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
  fDataThresholdLabel = new TGLabel(fDataThresholdFrame, new TGString("Pixel Thresh. (ADC cnts.)"));
  fDataThresholdFrame->AddFrame(fDataThresholdLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4, 4, 4, 4));
  fDataThresholdEntry = new TGTextEntry(fDataThresholdFrame, fThresholdBuf = new TGTextBuffer(80));
  fThresholdBuf->AddText(0,W4);
  fDataThresholdEntry->SetToolTipText("Insert the threshold for noise rejection (number of STDs)");
  fDataThresholdFrame->AddFrame(fDataThresholdEntry, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 4, 4, 4, 4));

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
  fRightFrame->AddFrame(fDataFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 4, 4, 0, 0));
  //---------------------------------------------------------------------------------
 
  // Compose frames in window
  fMainFrameHor->AddFrame(fLeftFrame, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY, 0, 0, 0, 0));
  fMainFrameHor->AddFrame(fRightFrame, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY, 0, 0, 0, 0)); 
  fMainFrameVert->AddFrame(fMainFrameHor, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY, 0, 0, 0, 0));
  AddFrame(fMainFrameVert, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY, 4, 4, 4, 4));
  
  MapSubwindows();
  Resize(GetDefaultSize());
  SetWindowName("Pixy Control Panel (v5)");
  Resize(550, 450);
  MapWindow();
  Move(100,10);
 
  fCumulativeHitmapCanvas = 0;
  fCumulativeHitmapHisto = 0;
  fCumulativeHitmapCanvas2D = 0;
}


TMainGUI::~TMainGUI() {

  if (fHistosCanvas) delete fHistosCanvas;
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
  delete PedsAnalizedFile;
  delete fTree;
  delete DataAnalizedFile;
 
  delete fAnalizeDataButton;
  delete fDisplayEventButton;
  delete fCheck1;
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
	else {
	  RawFlag = 0;
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
  Int_t totnev = 0, nev;
  TObjString *el;
  TIter next(fi.fFileNamesList);
  TString rootFile;
  TString rootExt = "_TH";
  Int_t size;
  cout << "=====> START ANALYSIS!!!!! " << endl;
  DataPanelDisable();
  eofdata = 0;
  longEv = 0;
  Rcounter = 0;
  RTFlag = 0; // O = No EventsTree for MC analisys <<<<<<<<<<<<<============ 
  FFflag = atoi(W7);

  if (RTFlag) InitializeEventsTree();
  SmallRadius  = atof(fW1->GetString());
  WideRadius   = atof(fW2->GetString());
  Weight = atof(fW3->GetString());
  fPixelThreshold = atof(fThresholdBuf->GetString());
  
  rootExt += (TString)fThresholdBuf->GetString();
  rootExt += ".root";

  Int_t startEv = 0, stopEv = 0;
  startEv = atoi(fEvData->GetString());

  RawFlag = fCheck1->GetState();
  gSystem->ChangeDirectory(workingdir);
  if ((!gSystem->AccessPathName("RawSignals.root", kFileExists)) && (!RawFlag)) 
    gSystem->Rename("RawSignals.root","RawSignals_old.root");
  

  //start LOOP on Input files!!!!!
  while ((el = (TObjString *) next())){
    DataName = el->GetString(); 
    size = DataName.Length();
    rootFile = DataName;	
    rootFile.Replace(size-5,rootExt.Length(),rootExt);
    cout << "\nInput file       --> " << DataName << endl;
    cout << "Clusters saved in --> " << rootFile << endl;
    
    if (!gSystem->AccessPathName(DataName, kFileExists)) {
      MCflag = 0;
      NewDataFlag = 0; // <<<<<<=============== Flag for reading matrix of data from new chip3
 
      if (DataName.Contains(".root")) MCflag = 1;        // MonteCarlo data
      if (DataName.Contains(".mdat")) NewDataFlag = 1;   // ROI data from new chip3 (Still not ped subtracted)
      if (DataName.Contains("fullFrame_")) FFflag = 1;   // Full Frame data
 
      infofile.open("info.dat",ios::out);
      infofile << "SmallRadius: " << SmallRadius << endl;
      infofile << "WideRadius: " << WideRadius << endl;
      infofile << "Weight: " << Weight << endl;
      infofile << "Threshold: " << fPixelThreshold << endl;
      infofile << "RawDataFlg: " << RawFlag << endl;
      infofile.close();
       
      InitializeClusterTree(rootFile);
       
      if (RawFlag&&NewDataFlag) InitializeRawSignalTree();
      gSystem->ChangeDirectory(workingdir);
         
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
	    SaveClusters(NbClusters,nev);
	    totnev++;
	  }
	  totnev = totnev + startEv;
	}//MonteCarlo flag 

	else {
	  RawFileName = new TInputFile(DataName, kBinary);
	  Polarimeter = new TDetector(PixFileName, RawFileName);
	   
	  if(HeaderOn)Polarimeter->SetHeaderOn();
	  Polarimeter->SetWeight(Weight);
	  Polarimeter->SetSmallRadius(SmallRadius);
	  Polarimeter->SetWideRadius(WideRadius);
	  Polarimeter->fPixelThresh = fPixelThreshold;
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
	     
	    eofdata =  Polarimeter->ReadROInew(totnev);
	     
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
	     
	    if (!((totnev-startEv)%1000)) cout << (totnev-startEv) << " events analized starting from ev: " << startEv << endl;
	     
	    Int_t NbClusters = Polarimeter->FindClusters();
	    if (Polarimeter->GetRcounter()) Rcounter++;
	    if (RawFlag) SaveRawSignal(nev);
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
	  
	}//Real Data
	 
	cout << "===>>> end loop on events" << endl;
	timer1.Stop();
	timer1.Print("m");
	 
	//WriteRawSignalTree();
	cout << " +++++  Rejected for numPixels Xeedoing MAX transf. : " << longEv << endl;
	cout << nev << " events found over a total of " << (totnev-startEv) << " (" << (Float_t)(nev*100)/(totnev-startEv) << "%)" << endl;	
	//if(nev ==0)break;
	cout << "Impact points within selected circles: " << Rcounter << " -->> " << Float_t (Rcounter*100/nev) << " %" << endl;

	if (RTFlag&&nev) WriteEventsTree();
	WriteClusterTree(rootFile);
	if (RawFlag&&NewDataFlag&&nev) WriteRawSignalTree();
	if(nev)DrawCumulativeHitMap(nev);

	CloseFiles();	   
	 
      }//pixmap
      else {
	cout << "Pixmap file not existing!!!" << endl;
	return;
      } 
	   
    }//datafile
    else {
      cout << "Data File: " << DataName << " not existing!!!" << endl;
    }
   
  } //END LOOP ON FILES!!!!
  DataPanelEnable();
  return;
}


void TMainGUI::DataDisplay()
{
  EventDisplay = new TEventDisplay(fClient->GetRoot(), this, 300, 300);
  return;
}

void TMainGUI::CloseFiles(){
  gSystem->ChangeDirectory(workingdir);
  PixFileName->fStream.close();
  delete PixFileName;
  if (!MCflag) {
    RawFileName->fStream.close();
    delete RawFileName;
  }
  delete Polarimeter;
  return;
}

void TMainGUI::WriteClusterTree(TString rootFile){
  DataAnalizedFile->cd();
  fTree->Write();
  cout << "Writing all clusters found in root file ==>> " << rootFile << endl;
  delete fTree;
  DataAnalizedFile->Close();
  delete DataAnalizedFile;
  return;
}

void TMainGUI::WriteRawSignalTree(){
  RawSignalFile->cd();
  fRawTree->Write();
  cout << "Writing Raw Signals in root file ==>> RawSignals.root" << endl;
  delete fRawTree;
  RawSignalFile->Close();
  delete RawSignalFile;
  return;
}

void TMainGUI::WriteEventsTree(){
  EventsFile->cd();
  tree->Write();
  cout << "Writing real data clusters for MC analysis in root file ==>> Eventstree.root" << endl;
  delete tree;
  EventsFile->Close();
  delete EventsFile;
  return;
}

void TMainGUI::InitializeClusterTree(TString rootFile) {
  DataAnalizedFile = new TFile(rootFile, "RECREATE");
  cout << ".........Initializing Clusters Tree........";
  fTree = new TTree("tree","Analized Data Tree");
  fTree->Branch("fEventId", &fEventId, "fEventId/I");
  fTree->Branch("fNClusters", &fNClusters, "fNClusters/I");
  fTree->Branch("fTrigWindow", &fTrigWindow, "fTrigWindow/I");
  fTree->Branch("fTimeTick",&fTimeTick,"fTimeTick/l");//https://root.cern.ch/root/html534/TBranch.html
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
  return;
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
  return;
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
  return;
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
      DigiCharge[j] = 1400 - Charge[j]; //
    }
    tree->Fill();
  }
  return;
}

void TMainGUI::SaveClusters(Int_t NbClusters, Int_t nev){
  fEventId   = nev;
  fNClusters = NbClusters;
  fTrigWindow = Polarimeter->numPix;
  fTimeTick   = Polarimeter->timetick;
  fTimeStamp  = Polarimeter->timestamp;
  fbufferId   = Polarimeter->bufferID;
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
  for (Int_t j=0; j<nchans; j++)fRawSignal[j] = Polarimeter->fROIRawData[j];
  fRawTree->Fill();
  return;
}

void TMainGUI::DrawCumulativeHitMap(Int_t nev){  
  // Display cumulative Hitmap!
  Float_t DeathThreshold = nev/10.0;
  if(fCumulativeHitmapCanvas>0){
    delete fCumulativeHitmapCanvas;
    fCumulativeHitmapCanvas = 0;
  }
  if(!fCumulativeHitmapCanvas)fCumulativeHitmapCanvas = new TCanvas("Cumulative Hitmap", " Cumulative Hitmap", 900, 10, 1000, 500);
  fCumulativeHitmapCanvas->SetFillColor(10);

  if(fCumulativeHitmapHisto>0){
    delete fCumulativeHitmapHisto;
    fCumulativeHitmapHisto = 0;
  }
  if(!fCumulativeHitmapHisto)fCumulativeHitmapHisto = new TH1F("Cumulative Hitmap", "Cumulative Hitmap", NCHANS, 0, NCHANS);
  Float_t MaxHitmap = 0;
  for (Int_t ch=0; ch<NCHANS; ch++){
    fCumulativeHitmapHisto->SetBinContent(ch+1, Polarimeter->fCumulativeHitmap[ch]);
    if (Polarimeter->fCumulativeHitmap[ch]>MaxHitmap) MaxHitmap = Polarimeter->fCumulativeHitmap[ch];
  }
  fCumulativeHitmapHisto->Draw(); 
  fCumulativeHitmapCanvas->Update();

  if(fCumulativeHitmapCanvas2D>0){
    delete fCumulativeHitmapCanvas2D;
    fCumulativeHitmapCanvas2D = 0;
  }
  if(!fCumulativeHitmapCanvas2D)fCumulativeHitmapCanvas2D = new TCanvas("Cumulative Hitmap 2D", " Cumulative Hitmap 2D", 900, 200, 700, 700);
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
  for(Int_t k=0;k<7;k++)PxFile >> DummyChar;
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
  fCumulativeHitmapCanvas2D->Update();
  PxFile.close();
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
  fCheck1->SetEnabled(false);
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
  fCheck1->SetEnabled(true);
  fHeadToFile->SetEnabled(true);
  fSradiusEntry->SetState(true);
  fWradiusEntry->SetState(true);
  fWeightEntry->SetState(true);
  return;
 }

ClassImp(TMainGUI)
