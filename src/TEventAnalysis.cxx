#include "TEventAnalysis.h"

bool sort_by_ph2 (TCluster* x, TCluster* y) { return (x->fPulseHeight > y->fPulseHeight); }


TEventAnalysis::TEventAnalysis(Int_t VLEVEL, char* _name, char* _dflist, Int_t NEVTS)
{
  Init(VLEVEL, _name, _dflist);
  DataAnalysis(VLEVEL, NEVTS);
}


void TEventAnalysis::Init(Int_t VLEVEL, char* _name, char* _dflist )
{
  _workingDir = gSystem->WorkingDirectory();
  SetProgName(_name);
  if (VLEVEL >= 0) 
    cout << "[" << _progName << " - MESSAGE] Working directory: " <<  _workingDir << endl;

  infofilename   = _workingDir + "/info.dat";      // store used configuration
  configfilename = _workingDir + "/config.dat";    // user's configuration
  dataflistname  = _workingDir + "/" + _dflist;    // data file list  

  W1 = 1.5;    // SmallCircleRadius
  W2 = 3.5;    // WideCircleRadius
  W3 = 0.05;   // WeightLengthScale
  W4 = 9;      // PixelThreshold  
  W6 = false;  // RawSignalSave
  W7 = false;  // FullFrameFlag

  //cout << "before: W... " << W1 << ' ' <<  W2 << ' ' <<  W3 << ' ' <<   W4 << ' ' << W6 << ' ' << W7 << endl;
  /* Read config file and overwrite defaults if any */
  if ((gSystem->AccessPathName(configfilename, kFileExists))) 
    if (VLEVEL >= 0) 
      cout << "[" << _progName << " - MESSAGE] No config file found: using defaults." << endl;
  ifstream _configfile;
  if ((!gSystem->AccessPathName(configfilename, kFileExists))) {
    if (VLEVEL >= 0) 
      cout << "[" << _progName << " - MESSAGE] Using config file  " << configfilename << endl;
    _configfile.open(configfilename, ios::in);
    string line;
    string w1("SmallCircleRadius");
    string w2("WideCircleRadius");
    string w3("WeightLengthScale");
    string w4("PixelThreshold");
    string w6("RawSignalSave");
    string w7("FullFrameFlag");
    /* Read pars from input file */
    if (VLEVEL >= 0) 
      cout << "[" << _progName << " - MESSAGE] Reading parameters: " << endl;
    while ( getline(_configfile,line) ) {
      stringstream ssline(line);
      string _param;
      Float_t _val;
      ssline >> _param >> _val;
      if (VLEVEL >= 0) 
	cout << "                     " << _param << ' ' << _val <<endl;
      if ( strstr(_param.c_str(), w1.c_str()) ) W1 = _val;
      if ( strstr(_param.c_str(), w2.c_str()) ) W2 = _val;
      if ( strstr(_param.c_str(), w3.c_str()) ) W3 = _val;
      if ( strstr(_param.c_str(), w4.c_str()) ) W4 = _val;
      if ( strstr(_param.c_str(), w6.c_str()) ) W6 = _val;
      if ( strstr(_param.c_str(), w7.c_str()) ) W7 = _val;
    }
    _configfile.close();
  }
  //cout << "after: W... " << W1 << ' ' <<  W2 << ' ' <<  W3 << ' ' <<   W4 << ' ' << W6 << ' ' << W7 << endl;

  /* Get input files */
  if (VLEVEL >= 0) 
    cout << "[" << _progName << " - MESSAGE] Input files list:  " << dataflistname << endl;
  ifstream _dataflist;
  if ((gSystem->AccessPathName(dataflistname, kFileExists))) {
    if (VLEVEL >= 0) 
      cout << "[" << _progName << " -   ERROR] File not found." << endl;
    exit(0);
  }

  dataflist = new TList();
  if ((!gSystem->AccessPathName(dataflistname, kFileExists))) {
    _dataflist.open(dataflistname, ios::in);
    char dfnam[80];
    if (VLEVEL >= 0) 
      cout << "[" << _progName << " - MESSAGE] Adding data files:  " << endl;
    while ( _dataflist >> dfnam ) {
      if (VLEVEL >= 0) 
	cout << "                     " << dfnam << endl;
      /* Create file chain */
      // datafchain.Add(dfnam);
      /* Create TList of files */
      dataflist->Add(new TObjString(dfnam));
    }
    // cout  << "[" << _progName << " - MESSAGE] Added " << datafchain.GetEntries() << " files" << endl;
    _dataflist.close();
  }
  if (VLEVEL >= 0) 
    cout <<  "[" << _progName << " - MESSAGE] Files inside " << dataflist->GetEntries() << endl;
}



void TEventAnalysis::DataAnalysis(Int_t VLEVEL, Int_t NEVTS)
{
  Int_t totnev = 0, nev;
  TObjString *el;

  TList *_fileNamesList = NULL;

  if(dataflist->GetEntries()!=0) {
    _fileNamesList = dataflist;
  }
  /*
  else if (fi.fFileNamesList->GetEntries()!=0) {
    cout << "***** 3a" << endl;
    _fileNamesList = fi.fFileNamesList;  // FS currently dummy
    cout << "***** 4a" << endl;
  }
  */
  else cerr << "[" << _progName << " -   ERROR] File list not found" << endl;

  TIter next(_fileNamesList);
  TString rootFile;
  TString rootExt = "_TH";     // FS Output file - change to "out" directory
  Int_t size;
  if (VLEVEL >= 0) 
    cout <<  "[" << _progName << " - MESSAGE] =====> START ANALYSIS!!!!! " << endl;
  //DataPanelDisable();                                  // FS !!BATCH
  eofdata = 0;
  longEv = 0;
  Rcounter = 0;
  RTFlag = 0; // O = No EventsTree for MC analisys <<<<<<<<<<<<<============ 
  FFflag = W7;

  if (RTFlag) InitializeEventsTree();
  SmallRadius  = W1; 
  WideRadius   = W2; 
  Weight       = W3;       
  fPixelThreshold = W4;
  
  rootExt += W4;   
  rootExt += "_outputfile";
  rootExt += ".root";

  Int_t startEv = 0, stopEv = 0;
  //startEv = atoi(fEvData->GetString());                // FS  - get from arguments

  RawFlag = W6;
  gSystem->ChangeDirectory(_workingDir);
  if ((!gSystem->AccessPathName("RawSignals.root", kFileExists)) && (!RawFlag)) 
    gSystem->Rename("RawSignals.root","RawSignals_old.root");
  

  /* start LOOP on Input files */
  while ((el = (TObjString *) next())){
    DataName = el->GetString(); 
    size = DataName.Length();
    rootFile = DataName;	
    rootFile.Replace(size-5,rootExt.Length(),rootExt);
    if (VLEVEL >= 0) { 
      cout << "[" << _progName << " - MESSAGE] Input file:        " << DataName << endl;
      cout << "[" << _progName << " - MESSAGE] Clusters saved in: " << rootFile << endl;
    }
    /* Need to extract Run Number and write in output */
    fRunId = GetRunId(DataName); 
      
    if (!gSystem->AccessPathName(DataName, kFileExists)) {
      MCflag = 0;
      NewDataFlag = 0; // <<<<<<=============== Flag for reading matrix of data from new chip3
 
      if (DataName.Contains(".root")) MCflag = 1;        // MonteCarlo data
      if (DataName.Contains(".mdat")) NewDataFlag = 1;   // ROI data from new chip3 (Still not ped subtracted)
      if (DataName.Contains("fullFrame_")) FFflag = 1;   // Full Frame data
 
      infofile.open(infofilename, ios::out);
      infofile << "SmallCircleRadius: " << SmallRadius << endl;
      infofile << "WideCircleRadius:  " << WideRadius << endl;
      infofile << "WeightLengthScale: " << Weight << endl;
      infofile << "PixelThreshold:    " << fPixelThreshold << endl;
      infofile << "RawSignalSave:     " << RawFlag << endl;
      infofile.close();

      InitializeClusterTree(rootFile);
      if ( RawFlag && NewDataFlag ) InitializeRawSignalTree();

      gSystem->ChangeDirectory(_workingDir);         
      if ( !gSystem->AccessPathName( "pixmap.dat", kFileExists )) {
	PixFileName = new TInputFile("pixmap.dat", kAscii);
	 
	if ( MCflag ) {
	  if (VLEVEL >= 0) 
	    cout <<  "[" << _progName << " - WARNING] MonteCarlo data " << endl;

	  TFile *f = new TFile(DataName);
	  TTree *t=(TTree*)f->Get("EventsTree");

	  if (NEVTS == -999) stopEv=(int)t->GetEntries();
	  else stopEv = (int)NEVTS;

	  cout << "[" << _progName << " - MESSAGE] Events to be read in MC tree:  " << stopEv-startEv+1 << endl;
	  Polarimeter = new TDetector(t,PixFileName);
	  Polarimeter->SetWeight(Weight);
	  Polarimeter->SetSmallRadius(SmallRadius);
	  Polarimeter->SetWideRadius(WideRadius);
	  Polarimeter->fPixelThresh = fPixelThreshold;
	  nev = 0;
	  for ( Int_t i=startEv; i<=stopEv; i++ ) {
	    f->cd();
	    nev++;
	    gSystem->ProcessEvents(); //Added 29-01-08
	    if ( !((i-startEv)%1000) ) 
	      if (VLEVEL >= 0) 
		cout << "[" << _progName << " - RUNNING] ========>>> Analized " << i-startEv << " MC events" << endl;
	    eofdata = Polarimeter->ReadMCFile(i);
	    Int_t NbClusters = Polarimeter->FindClusters();
	    if ( Polarimeter->GetRcounter() ) Rcounter++;
	    if ( !NbClusters ) nev--;
	    SaveClusters(NbClusters, nev);
	    totnev++;
	  }
	  totnev = totnev + startEv;
	}//MonteCarlo flag 

	else {
	  if (VLEVEL >= 0) 
	    cout <<  "[" << _progName << " - WARNING] Real data " << endl;
	  RawFileName = new TInputFile(DataName, kBinary);
	  Polarimeter = new TDetector(PixFileName, RawFileName);
	   
	  if(HeaderOn)Polarimeter->SetHeaderOn();
	  Polarimeter->SetWeight(Weight);
	  Polarimeter->SetSmallRadius(SmallRadius);
	  Polarimeter->SetWideRadius(WideRadius);
	  Polarimeter->fPixelThresh = fPixelThreshold;
	  nev = 0;
	  totnev = 0;
	   
	  if (NEVTS == -999) {
	     if (VLEVEL >= 0)
	      cout << "[" << _progName << " - MESSAGE] Reading ALL events in file" << endl;
	  }
	  else {
	    stopEv = (int)NEVTS;
	    if (VLEVEL >= 0)
	      cout << "[" << _progName << " - MESSAGE] " << stopEv-startEv+1 << " events to be read" << endl;
	  }
	  timer2.Start(kTRUE);

	  while(!Polarimeter->fRawFile->fStream.eof()){
	    if (stopEv && totnev == stopEv+1) break;
	    nev++;
	    totnev++;
	    // gSystem->ProcessEvents(); //Added 29-01-08 FS- //!! BATCH!
	     
	    eofdata =  Polarimeter->ReadROInew(totnev);
	     
	    if(eofdata == 1) {
	      if (VLEVEL >= 0)
		cout << "[" << _progName << " - MESSAGE] Events: " << totnev << "  end of file!!" << endl;
	      break;
	    }
	    if(eofdata==2){
	      cout << totnev << "[" << _progName << " -   ERROR] Bad window ==> event rejected " << endl;
	      longEv++;
	      nev--;  
	      continue;
	    }       
	    if(totnev < startEv){
	      nev = 0;
	      continue;
	    }
	     
	    if ( !((totnev-startEv)%100) ) 
	      cout << "[" << _progName << " - RUNNING] " << (totnev-startEv) << " events analized starting from ev: " << startEv << endl;
	     
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
	      if (RTFlag && NbClusters==1) SaveEventsTree(NbClusters,nev);
	      if (RawFlag && NbClusters>=1 && NewDataFlag) SaveRawSignal(nev);
	      if (NbClusters>=1) SaveClusters(NbClusters,nev);
	    }
	  }// end while
	  
	}//Real Data
	 
	cout << "[" << _progName << " - MESSAGE] End loop on events" << endl;
	timer2.Stop();
	if (VLEVEL >= 0) {
	  cout << "[" << _progName << " - MESSAGE] ";
	  timer2.Print("m");
	}
	//WriteRawSignalTree();
	if (VLEVEL >= 0) {
	  cout << "[" << _progName << " - MESSAGE] Rejected for numPixels Xeedoing MAX transf.: " << longEv << endl;
	  cout << "[" << _progName << " - MESSAGE] " << nev << " events found over a total of " 
	       << (totnev-startEv) << " (" << (Float_t)(nev*100)/(totnev-startEv) << "%)" << endl;	
	//if(nev ==0)break;
	  cout << "[" << _progName << " - MESSAGE] Impact points within selected circles: " << Rcounter << " -->> " << Float_t (Rcounter*100/nev) << " %" << endl;
	}

	if (RTFlag&&nev) WriteEventsTree();
	WriteClusterTree(rootFile);
	if (RawFlag&&NewDataFlag&&nev) WriteRawSignalTree();
	//if (nev) DrawCumulativeHitMap(nev);   // FS Batch!

	CloseFiles();	   
	 
      }//pixmap
      else {
	cerr << "[" << _progName << " -   ERROR] Pixmap file not existing!!!" << endl;
	return;
      } 
	   
    }//datafile
    else {
      cerr << "[" << _progName << " -   ERROR] Data File: " << DataName << " not existing!!!" << endl;
    }
   
  } //END LOOP ON FILES!!!!
  //DataPanelEnable();                            //FS !!BATCH
  return;
}



void TEventAnalysis::InitializeRawSignalTree() {
  RawSignalFile = new TFile("RawSignals.root", "RECREATE");
  cout << "[" << _progName << " - MESSAGE] Initializing RawSignals Tree... ONLY for events read in Window mode!!! ";
  nchans = GetNchans();
  fRawTree = new TTree("RawSignaltree","Raw Signal");
  fRawTree->Branch("fEventId", &fEventId, "fEventId/I");
  fRawTree->Branch("nchans",&nchans,"nchans/I");
  fRawTree->Branch("fRawSignal",fRawSignal, "fRawSignal[nchans]/F");
  cout << "... done! " << endl;
  return;
}



void TEventAnalysis::InitializeClusterTree(TString rootFile) {
  DataAnalizedFile = new TFile(rootFile, "RECREATE");
  cout << "[" << _progName << " - MESSAGE] Initializing Clusters Tree";
  fTree = new TTree("tree","Analized Data Tree");
  fTree->Branch("fRunId", &fRunId, "fRunId/i");
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
  cout << "... done! " << endl;
  return;
}


void TEventAnalysis::InitializeEventsTree(){
  EventsFile = new TFile("Eventstree.root", "RECREATE");
  cout << "[" << _progName << " - MESSAGE] Initializing EventsTree with real data for MC analysis";
  tree = new TTree("EventsTree","data");
  tree->Branch("Nevent",&Nevent,"Nevent/I");
  tree->Branch("Clusterdim",&Clusterdim,"Clusterdim/I");
  tree->Branch("Channel",Channel,"Channel[Clusterdim]/I");
  tree->Branch("Charge",Charge,"Charge[Clusterdim]/I");
  tree->Branch("DigiCharge",DigiCharge,"DigiCharge[Clusterdim]/I");
  tree->Branch("InitialPhi",&Phi,"Phi/D");
  //tree->Branch("XInitial",&XInitial,"XI/D");
  //tree->Branch("YInitial",&YInitial,"YI/D");
  cout << "... done! " << endl;
  return;
}


void TEventAnalysis::SaveRawSignal(Int_t nev) {
  fEventId   = nev+1;
  for (Int_t j=0; j<nchans; j++)fRawSignal[j] = Polarimeter->fROIRawData[j];
  fRawTree->Fill();
  return;
}


void TEventAnalysis::SaveClusters(Int_t NbClusters, Int_t nev){
  fEventId   = nev;
  fNClusters = NbClusters;
  fTrigWindow = Polarimeter->numPix;
  fTimeTick   = Polarimeter->timetick;
  fTimeStamp  = Polarimeter->timestamp;
  fbufferId   = Polarimeter->bufferID;
  // sort by fPulseHeight, highest first, as defined in  sort_by_ph2
  if (NbClusters>1)
    {
      sort(Polarimeter->fAllClusts, Polarimeter->fAllClusts + NbClusters, sort_by_ph2);
    }
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


void TEventAnalysis::SaveEventsTree(Int_t NbClusters, Int_t nev){
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


void TEventAnalysis::WriteRawSignalTree(){
  RawSignalFile->cd();
  fRawTree->Write();
  cout << "[" << _progName << " - MESSAGE] Writing Raw Signals in root file ==>> RawSignals.root" << endl;
  delete fRawTree;
  RawSignalFile->Close();
  delete RawSignalFile;
  return;
}


void TEventAnalysis::WriteClusterTree(TString rootFile){
  DataAnalizedFile->cd();
  fTree->Write();
  cout << "[" << _progName << " - MESSAGE] Writing all clusters found in root file ==>> " << rootFile << endl;
  delete fTree;
  DataAnalizedFile->Close();
  delete DataAnalizedFile;
  return;
}


void TEventAnalysis::WriteEventsTree(){
  EventsFile->cd();
  tree->Write();
  cout << "[" << _progName << " - MESSAGE] Writing real data clusters for MC analysis in root file ==>> Eventstree.root" << endl;
  delete tree;
  EventsFile->Close();
  delete EventsFile;
  return;
}

/*
void TEventAnalysis::DrawCumulativeHitMap(Int_t nev){  
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
*/


Int_t TEventAnalysis::GetRunId(TString RunIdName)
{
  cout << "[" << _progName << " - MESSAGE] GetRunId::RunId Name = " << RunIdName << " " << endl;
  RunIdName = RunIdName.ReplaceAll(".mdat", "");
  RunIdName = RunIdName.ReplaceAll("data", "");
  RunIdName = RunIdName.ReplaceAll("_", "");
  Ssiz_t lastMarker = RunIdName.Last('/'); // assume linux..
  if (lastMarker<0){ // must be windows
        lastMarker = RunIdName.Last('\\');  
  }
  RunIdName.Remove(0, lastMarker+1);
  cout << "[" << _progName << " - MESSAGE] GetRunId::RunId = " << RunIdName << " " << endl;
  //UInt_t RunId = (UInt_t) RunIdName.Data();
      
  return 0;// for now!!!
}


void TEventAnalysis::CloseFiles(){
  gSystem->ChangeDirectory(_workingDir);
  PixFileName->fStream.close();
  delete PixFileName;
  if (!MCflag) {
    RawFileName->fStream.close();
    delete RawFileName;
  }
  delete Polarimeter;
  return;
}



void TEventAnalysis::ClearArrays(Int_t NbClusters){
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

TEventAnalysis::~TEventAnalysis() {
  delete dataflist;
  dataflist = NULL;
}

//ClassImp(TEventAnalysis)
