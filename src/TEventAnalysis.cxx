#include "TEventAnalysis.h"

bool sort_by_ph2 (TCluster* x, TCluster* y) { return (x->fPulseHeight > y->fPulseHeight); }

TEventAnalysis::TEventAnalysis()
{}

TEventAnalysis::TEventAnalysis(Int_t VLEVEL, char* _name, char* _dflist, Int_t startEv, Int_t stopEv)
{
  Init(VLEVEL, _name, _dflist);
  if (VLEVEL > 1) cout << "[" << progName << " -   DEBUG] startev stopev " << startEv << ' ' << stopEv << endl;
  DataAnalysis(VLEVEL, startEv, stopEv);
}


void TEventAnalysis::Init(Int_t VLEVEL, char* _name, char* _dflist )
{
  SetProgName(_name);

  bool is_batch = ( (_dflist == NULL) ? 0 : 1 );
  if (VLEVEL >= 1) cout << "[" << progName << " -   DEBUG] is_batch " << is_batch << endl;

  /* Files and extensions */
  if (is_batch) {  // Batch: set working dir now. Interactive: already got from GUI
    workingDir = gSystem->WorkingDirectory();
  }
  outputDir  = workingDir + "/out";
  if (VLEVEL >= 0) {
    cout << "[" << progName << " - MESSAGE] Working directory: " <<  workingDir << endl;
    cout << "[" << progName << " - MESSAGE] Output will be in: " <<  outputDir << endl;
  }

  TString     infofilename, configfilename, dataflistname;
  infofilename   = workingDir + "/info.dat";      // store used configuration
  configfilename = workingDir + "/config.dat";    // user's configuration
  dataflistname  = workingDir + "/" + _dflist;    // data file list  

  if (VLEVEL > 1)
    cout << "[" << progName << " -   DEBUG] before: W... " << W1 << ' ' <<  W2 << ' ' <<  W3 << ' ' <<   W4 << ' ' << W5 << endl;

  /* Default parameters */
  if (is_batch) {  // Batch: set parameters now. Interactive: already got from GUI
    W1 = 1.5;    // SmallCircleRadius
    W2 = 3.5;    // WideCircleRadius
    W3 = 0.05;   // WeightLengthScale
    W4 = 11;     // PixelThreshold  
    W5 = 1;      // FixThrFlag
    
    /* Read parameters from config file and overwrite defaults if any */
    if ((gSystem->AccessPathName(configfilename, kFileExists))) 
      if (VLEVEL >= 0) 
	cout << "[" << progName << " - MESSAGE] No config file found: using defaults." << endl;
    
    ifstream _configfile;
    if ((!gSystem->AccessPathName(configfilename, kFileExists))) {
      if (VLEVEL >= 0) 
	cout << "[" << progName << " - MESSAGE] Using config file: " << configfilename << endl;
      _configfile.open(configfilename, ios::in);
      string line;
      string w1("SmallCircleRadius");
      string w2("WideCircleRadius");
      string w3("WeightLengthScale");
      string w4("PixelThreshold");
      string w5("FixThrFlag");
       
      if (VLEVEL >= 0) 
	cout << "[" << progName << " - MESSAGE] Reading parameters: " << endl;
      while ( getline(_configfile,line) ) {
	stringstream ssline(line);
	string _param;
	Float_t _val;
	ssline >> _param >> _val;
	if (VLEVEL >= 0) 
	  cout << "[" << progName << " - MESSAGE]             " << _param << ' ' << _val <<endl;
	if ( strstr(_param.c_str(), w1.c_str()) ) W1 = _val;
	if ( strstr(_param.c_str(), w2.c_str()) ) W2 = _val;
	if ( strstr(_param.c_str(), w3.c_str()) ) W3 = _val;
	if ( strstr(_param.c_str(), w4.c_str()) ) W4 = _val;
	if ( strstr(_param.c_str(), w5.c_str()) ) W5 = _val;
      }
      _configfile.close();
    }

  }

  if (VLEVEL > 1)
    cout << "[" << progName << " -   DEBUG] after: W... " << W1 << ' ' <<  W2 << ' ' <<  W3 << ' ' <<   W4 << ' ' << W5 << endl;

  /* Set parameters */
  SmallRadius     = W1; 
  WideRadius      = W2; 
  Weight          = W3;       
  fPixelThreshold = W4;
  ThfixFlag       = W5;

  /* Manage output files */
  rootExt = "_TH";
  rootExt += W4;   
  rootExt += "_outputfile.root";

  ofstream _infofile;
  _infofile.open(infofilename, ios::out);
  _infofile << "SmallRadius:       " << SmallRadius << endl;
  _infofile << "WideCircleRadius:  " << WideRadius << endl;
  _infofile << "WeightLengthScale: " << Weight << endl;
  _infofile << "PixelThreshold:    " << fPixelThreshold << endl;
  _infofile << "FixThrFlag:        " << ThfixFlag << endl;
  _infofile.close();
  if (VLEVEL >= 0) 
    cout << "[" << progName << " - MESSAGE] Current config written in file " << infofilename << endl;

  /* Get input data files list */
  if (VLEVEL > 1) cout << "[" << progName << " -   DEBUG] dataflist " << dataflist << endl;

  if (is_batch) {  // Batch: build file list from datafile list. Interactive: already got from GUI
    if (VLEVEL >= 0) 
      cout << "[" << progName << " - MESSAGE] Input files list:  " << dataflistname << endl;
    if ((gSystem->AccessPathName(dataflistname, kFileExists))) {
      if (VLEVEL >= 0) 
	cout << "[" << progName << " -   ERROR] File not found." << endl;
      exit(0);
    }

    ifstream _dataflist;
    dataflist = new TList();
    if ((!gSystem->AccessPathName(dataflistname, kFileExists))) {
      _dataflist.open(dataflistname, ios::in);
      char dfnam[80];
      if (VLEVEL >= 0) 
	cout << "[" << progName << " - MESSAGE] Adding data files:  " << endl;
      while ( _dataflist >> dfnam ) {
	if (VLEVEL >= 0) 
	  cout << "                     " << dfnam << endl;
	/* Create TList of files */
	dataflist->Add(new TObjString(dfnam));
      }
      _dataflist.close();
    }
  }
  if (VLEVEL >= 0) 
    cout <<  "[" << progName << " - MESSAGE] Files loaded " << dataflist->GetEntries() << endl;

}


TEventAnalysis::~TEventAnalysis() 
{
  delete dataflist;
  dataflist = NULL;
  delete fTree;
  delete DataAnalizedFile;
}



void TEventAnalysis::DataAnalysis(Int_t VLEVEL, Int_t startEv, Int_t stopEv)
{
  Int_t nTotEvents = 0, nClusEvents;
  TObjString *el;
  int err;
  TList *_fileNamesList = NULL;

  if(dataflist->GetEntries()!=0) {
    _fileNamesList = dataflist;
  }
  else {
    cerr << "[" << progName << " -   ERROR] File list not found. EXIT" << endl;
    exit(0);
  }

  TIter next(_fileNamesList);
  TString _dataFile;
  TString _rootFile;
  Int_t size, pathsize;
  //char separator[1] = "/";
  if (VLEVEL >= 0) 
    cout <<  "[" << progName << " - MESSAGE] =====> START ANALYSIS!!!!! " << endl;
  eofdata = 0;
  longEv = 0;
  Rcounter = 0;
  RTFlag = 0; // O = No EventsTree for MC analisys <<<<<<<<<<<<<============ 

  if (RTFlag) InitializeEventsTree();

  gSystem->ChangeDirectory(workingDir);

  /* start LOOP on Input files */
  while ((el = (TObjString *) next())){
    _dataFile = el->GetString(); 
    _rootFile = _dataFile;	
    pathsize = _rootFile.Last('/');
    _rootFile.Remove(0,pathsize);
    size = _rootFile.Length();
    if (VLEVEL > 1) cout << " pathsize, size " << pathsize <<  ' ' << size << endl;
    _rootFile.Replace(size-5,rootExt.Length(),rootExt);
    _rootFile = outputDir + _rootFile;
    if (VLEVEL >= 0) { 
      cout << "[" << progName << " - MESSAGE] Input file:  " << _dataFile << endl;
      cout << "[" << progName << " - MESSAGE] Clusters in: " << _rootFile << endl;
    }
    /* Need to extract Run Number and write in output */
    fRunId = GetRunId(_dataFile); 
      
    if (!gSystem->AccessPathName(_dataFile, kFileExists)) {
      MCflag = 0;
      NewDataFlag = 0; // <<<<<<=============== Flag for reading matrix of data from new chip3
 
      if (_dataFile.Contains(".root")) MCflag = 1;        // MonteCarlo data
      if (_dataFile.Contains(".mdat")) NewDataFlag = 1;   // ROI data from new chip3 (Still not ped subtracted)

      InitializeClusterTree(_rootFile);
  
      gSystem->ChangeDirectory(workingDir);   
      string mapName;
      mapName = "pixmap_xpe.fits";

      if ( !gSystem->AccessPathName(mapName.c_str(), kFileExists )) {
	if ( MCflag ) {
	  if (VLEVEL >= 0) 
	    cout <<  "[" << progName << " - WARNING] MonteCarlo data " << endl;

	  TFile *f = new TFile(_dataFile);
	  TTree *t=(TTree*)f->Get("EventsTree");

	  if (stopEv == -999) stopEv=(int)t->GetEntries();
	  // else stopEv = (int)NEVTS;

	  cout << "[" << progName << " - MESSAGE] Events to be read in MC tree:  " << stopEv-startEv+1 << endl;
	  Polarimeter = new TDetector(mapName,t);
	  cout << " TEventAnalysis:: progName " << progName << endl;
	  Polarimeter->SetProgParameters(VLEVEL, progName);

	  Polarimeter->SetWeight(Weight);
	  Polarimeter->SetSmallRadius(SmallRadius);
	  Polarimeter->SetWideRadius(WideRadius);
	  Polarimeter->fPixelThresh = fPixelThreshold;
	  Polarimeter->fThreshFlag = 1;
	  nClusEvents = 0;
	  for ( Int_t i=startEv; i<=stopEv; i++ ) {
	    f->cd();
	    nClusEvents++;
	    if ( !((i-startEv)%1000) ) 
	      if (VLEVEL >= 0) 
		cout << "[" << progName << " - RUNNING] ========>>> Analized " << i-startEv << " MC events" << endl;
	    eofdata = Polarimeter->ReadMCFile(i);
	    Int_t NbClusters = Polarimeter->FindClusters();
	    if ( Polarimeter->GetRcounter() ) Rcounter++;
	    if ( !NbClusters ) nClusEvents--;
	    SaveClusters(NbClusters, nClusEvents);
	    nTotEvents++;
	  }
	  nTotEvents = nTotEvents + startEv;
	}//MonteCarlo flag 

	else {
	  if (VLEVEL >= 0) 
	    cout <<  "[" << progName << " - WARNING] Real data " << endl;
	  RawFileName = new TInputFile(_dataFile, kBinary);
	  Polarimeter = new TDetector(mapName, RawFileName);
	  Polarimeter->SetProgParameters(VLEVEL, progName);

	  if (HeaderOn) Polarimeter->SetHeaderOn();
	  Polarimeter->SetWeight(Weight);
	  Polarimeter->SetSmallRadius(SmallRadius);
	  Polarimeter->SetWideRadius(WideRadius);
	  Polarimeter->fPixelThresh = fPixelThreshold;
	  Polarimeter->fThreshFlag = ThfixFlag;
	  if(!ThfixFlag){
	    err = Polarimeter->ReadRMS();
	    if(err) {
	      cout << "[" << progName << " -   ERROR] in reading PEDFITS file!!!! - EXIT! " << endl;
	      return;
	    }
	  }
	  nClusEvents = 0;
	  nTotEvents = 0;
	   
	  if (stopEv == -999) {
	     if (VLEVEL >= 0)
	      cout << "[" << progName << " - MESSAGE] Reading ALL events in file" << endl;
	  }
	  else {
	    // stopEv = (int)NEVTS;
	    if (VLEVEL >= 0)
	      cout << "[" << progName << " - MESSAGE] Reading " << stopEv-startEv+1 << " events" << endl;
	  }
	  timer2.Start(kTRUE);

	  while(!Polarimeter->fRawFile->fStream.eof()){
	    if (stopEv && nTotEvents == stopEv+1) break;
	    nClusEvents++;
	    nTotEvents++;
	    eofdata =  Polarimeter->ReadROInew(nTotEvents);
	     
	    if(eofdata == 1) {
	      if (VLEVEL >= 0)
		cout << "[" << progName << " - MESSAGE] Events: " << nTotEvents << "  end of file!!" << endl;
	      break;
	    }
	    if(eofdata==2){
	      cout << "[" << progName << " -   ERROR] Ev. " << nTotEvents << " - Bad window ==> event rejected " << endl;
	      longEv++;
	      nClusEvents--;  
	      continue;
	    }       
	    if(nTotEvents < startEv){
	      nClusEvents = 0;
	      continue;
	    }
	     
	    if ( !((nTotEvents-startEv) % 5000) ) 
	      cout << "[" << progName << " - RUNNING] " << (nTotEvents-startEv) << " events analized starting from ev: " << startEv << endl;
	     
	    Int_t NbClusters = Polarimeter->FindClusters();
	    if (Polarimeter->GetRcounter()) Rcounter++;
	    if (!NbClusters)
	      {
		//cout << "nClusEvents:  " << nClusEvents << "   NClusters:  " << NbClusters << " ---> NO CLUSTER FOUND in tot ev " << nTotEvents <<  endl;
		nClusEvents--;  
		continue;
	      }
	    else {
	      //cout << " OK nClusEvents: " << nClusEvents << " toteV " << nTotEvents << endl;
	      if (RTFlag && NbClusters==1) SaveEventsTree(NbClusters,nClusEvents);
	      if (NbClusters>=1) SaveClusters(NbClusters,nClusEvents);
	    }
	  }// end while
	  
	}//Real Data
	 
	cout << "[" << progName << " - MESSAGE] ====>>> END loop on events" << endl;
	timer2.Stop();
	if (VLEVEL >= 0) {
	  cout << "[" << progName << " - MESSAGE] ";
	  timer2.Print("m");
	}
	//WriteRawSignalTree();
	if (VLEVEL >= 0) {
	  cout << "[" << progName << " - MESSAGE] Rejected for numPixels Xeedoing MAX transf.: " << longEv << endl;
	  cout << "[" << progName << " - MESSAGE] " << nClusEvents << " events found over a total of " 
	       << (nTotEvents-startEv) << " (" << (Float_t)(nClusEvents*100)/(nTotEvents-startEv) << "%)" << endl;	
	//if(nClusEvents ==0)break;
	  cout << "[" << progName << " - MESSAGE] Impact points within selected circles: " << Rcounter << " -->> " << Float_t (Rcounter*100/nClusEvents) << " %" << endl;
	}

	if (RTFlag&&nClusEvents) WriteEventsTree();
	WriteClusterTree(_rootFile);
	//if (nClusEvents) DrawCumulativeHitMap(nClusEvents);   // FS Batch!

	CloseFiles();	   
	 
      }//pixmap
      else {
	cerr << "[" << progName << " -   ERROR] Pixmap file not existing!!!" << endl;
	return;
      } 
	   
    }//datafile
    else {
      cerr << "[" << progName << " -   ERROR] Data File: " << _dataFile << " not existing!!!" << endl;
    }
   
  } //END LOOP ON FILES!!!!
  //DataPanelEnable();                            //FS !!BATCH
  return;
}

void TEventAnalysis::InitializeClusterTree(TString rootFile) {
  DataAnalizedFile = new TFile(rootFile, "RECREATE");
  cout << "[" << progName << " - MESSAGE] Initializing Clusters Tree";
  fTree = new TTree("tree","Analized Data Tree");
  fTree->Branch("fRunId", &fRunId, "fRunId/i");
  fTree->Branch("fEventId", &fEventId, "fEventId/I");
  fTree->Branch("fNClusters", &fNClusters, "fNClusters/I");
  fTree->Branch("fTrigWindow", &fTrigWindow, "fTrigWindow/I");
  fTree->Branch("fTimeTick",&fTimeTick,"fTimeTick/l");//https://root.cern.ch/root/html534/TBranch.html
  fTree->Branch("fTimeStamp",&fTimeStamp,"fTimeStamp/D");
  fTree->Branch("fbufferId",&fbufferId,"fbufferId/I");

  fTree->Branch("ROI", ROI,"ROI[4]/I");
  fTree->Branch("fPHBXpixels", &fPHBXpixels, "fPHBXpixels[fNClusters]/F"); //sum of PHs of border pixels
  fTree->Branch("fNBXpixels", &fNBXpixels, "fNBXpixels[fNClusters]/I");   //number of border pixels
  fTree->Branch("fPHBYpixels", &fPHBYpixels, "fPHBYpixels[fNClusters]/F"); //sum of PHs of border pixels
  fTree->Branch("fNBYpixels", &fNBYpixels, "fNBYpixels[fNClusters]/I");   //number of border pixels

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
  cout << "[" << progName << " - MESSAGE] Initializing EventsTree with real data for MC analysis";
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


void TEventAnalysis::SaveClusters(Int_t NbClusters, Int_t nClusEvents){
  fEventId   = nClusEvents;
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
    for(int k=0;k<4;k++) ROI[k] = Polarimeter->Roi[k]; //read window bounder 
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

      if(fUpixel[i][j]==ROI[2] || fUpixel[i][j]==ROI[3]){
	fNBXpixels[i]++;
	fPHBXpixels[i]+=fPHpixel[i][j];
      }
   
      if(fVpixel[i][j]==ROI[0]){
	fNBYpixels[i]++;
	fPHBYpixels[i]+=fPHpixel[i][j];
	if(ROI[0]<299 && fVpixel[i][j]==ROI[1]+1){
	  fNBYpixels[i]++;
	  fPHBYpixels[i]+=fPHpixel[i][j];
	}
      }
    
      if(fVpixel[i][j]==ROI[1]){
	fNBYpixels[i]++;
	fPHBYpixels[i]+=fPHpixel[i][j];
	if(ROI[1]>0 && fVpixel[i][j]==ROI[1]-1){ 
	  fNBYpixels[i]++;
	  fPHBYpixels[i]+=fPHpixel[i][j];
	}
      }
    }

    if(i==0 && (fNBYpixels[i]>0||fNBXpixels[i]>0)) {
      //cout << "ev: " << nClusEvents-1 << " -- ROI: " << ROI[0] << " -- " << ROI[1] << " -- " << ROI[2] << " -- " << ROI[3] << endl;
      //cout << "[" << progName << " - MESSAGE] NeV: " << nClusEvents-1 << "  --> NBY: " << fNBYpixels[i] << " -- NBX: " << fNBXpixels[i] << endl;
    }
    delete Polarimeter->fAllClusts[i];  //free memory
    Polarimeter->fAllClusts[i] = 0;
  }//close loop on clusters!!!
  DataAnalizedFile->cd();
  fTree->Fill();
  ClearArrays(NbClusters);
  return;
}


void TEventAnalysis::SaveEventsTree(Int_t NbClusters, Int_t nClusEvents){
  Int_t u,v;
  Nevent   = nClusEvents;
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

void TEventAnalysis::WriteClusterTree(TString rootFile){
  DataAnalizedFile->cd();
  fTree->Write();
  cout << "[" << progName << " - MESSAGE] Writing clusters in root file." << endl;
  delete fTree;
  DataAnalizedFile->Close();
  delete DataAnalizedFile;
  return;
}


void TEventAnalysis::WriteEventsTree(){
  EventsFile->cd();
  tree->Write();
  cout << "[" << progName << " - MESSAGE] Writing real data clusters for MC analysis in root file ==>> Eventstree.root" << endl;
  delete tree;
  EventsFile->Close();
  delete EventsFile;
  return;
}


Int_t TEventAnalysis::GetRunId(TString RunIdName)
{
  cout << "[" << progName << " - MESSAGE] GetRunId::RunId Name = " << RunIdName << " " << endl;
  RunIdName = RunIdName.ReplaceAll(".mdat", "");
  RunIdName = RunIdName.ReplaceAll("data", "");
  RunIdName = RunIdName.ReplaceAll("_", "");
  Ssiz_t lastMarker = RunIdName.Last('/'); // assume linux..
  if (lastMarker<0){ // must be windows
        lastMarker = RunIdName.Last('\\');  
  }
  RunIdName.Remove(0, lastMarker+1);
  cout << "[" << progName << " - MESSAGE] GetRunId::RunId = " << RunIdName << " " << endl;
  //UInt_t RunId = (UInt_t) RunIdName.Data();     
  return 0;// for now!!!
}


void TEventAnalysis::CloseFiles(){
  gSystem->ChangeDirectory(workingDir);
  if (!MCflag) {
    RawFileName->fStream.close();
    delete RawFileName;
  }
  delete Polarimeter;
  return;
}


void TEventAnalysis::ClearArrays(Int_t NbClusters){
  for (Int_t i=0; i<NbClusters; i++){
    fNBXpixels[i] = fPHBXpixels[i] = 0;
    fNBYpixels[i] = fPHBYpixels[i] = 0;
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


//ClassImp(TEventAnalysis)
