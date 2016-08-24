#include "TDetector.h"
#include <fitsio.h>
#define PAR 1

TDetector::TDetector(TInputFile *fPix, TTree *tree) { 
  Evtree = tree;
  Evtree->SetBranchAddress("Clusterdim",&Clusterdim);
  Evtree->SetBranchAddress("Channel",Channel);
  Evtree->SetBranchAddress("DigiCharge",DigiCharge);
  Evtree->SetBranchAddress("InitialPhi",&Phi);
  Evtree->SetBranchAddress("XInitial",&XI);
  Evtree->SetBranchAddress("YInitial",&YI);
  fPixFile = fPix;
  CreatePixLookup();
  MCflag = 1;
}
 
TDetector::TDetector(string pixmap,TTree *tree) { 
  Evtree = tree;
  Evtree->SetBranchAddress("Clusterdim",&Clusterdim);
  Evtree->SetBranchAddress("Channel",Channel);
  Evtree->SetBranchAddress("DigiCharge",DigiCharge);
  Evtree->SetBranchAddress("InitialPhi",&Phi);
  Evtree->SetBranchAddress("XInitial",&XI);
  Evtree->SetBranchAddress("YInitial",&YI);
  FitsToPixmap(pixmap);
  MCflag = 1;
}


TDetector::TDetector(TInputFile *fPix, TInputFile *fRaw){
  // Make the member variable pointers fRawFile, etc. point to the various
  // file objects.
  fPixFile = fPix;
  fRawFile = fRaw;
  ifstream gainfile;
  // Read in and create the pixel lookup table.
  CreatePixLookup();
  // Reset the cumulative Hitmap
  for (Int_t i=0; i<NCHANS; i++) fCumulativeHitmap[i] = 0;
  MCflag = 0;
  roll = 0;
  tm.clear();
  }

TDetector::TDetector(string pixmap, TInputFile *fRaw){
  fRawFile = fRaw;
  ifstream gainfile;
  // Read in and create the pixel lookup table.
  FitsToPixmap(pixmap);
    // Reset the cumulative Hitmap
  for (Int_t i=0; i<NCHANS; i++) fCumulativeHitmap[i] = 0;
  MCflag = 0;
  roll = 0;
  tm.clear();
  }

void TDetector::CreatePixLookup() {
  Text_t DummyString[ 8 ];
  Int_t DummyInt;
  Int_t nc, nr;
  Float_t x,y;
  // Loop over all pixels, reading in the corresponding ADC channel number
  // and mask state from the file.  The first 2 entries on each line of the
  // file are the pixel coordinates - skip these.
  for (nr=0; nr< PIX_R_DIM; nr++)
    {
      for (nc=0; nc< PIX_C_DIM; nc++)
	{
	  fPixMap[nr][nc] = -1;
	  fPixMask[nr][nc] = 0;
	  fBorderPixel[nr][nc] = 0; 
	} 
    }
  for (int i=0; i<2; i++) {
    fPixFile->fStream >> DummyInt;  // Skip file header.
  }

  for (int i=0; i<7; ++i) {
    fPixFile->fStream >> DummyString;  // Skip file header.
  }
  while(!fPixFile->fStream.eof()) {
    fPixFile->fStream >> x >> y >> nr >> nc;
    fPixFile->fStream >> fPixMap[nr][nc] >> fPixMask[nr][nc] >> fBorderPixel[nr][nc];
    PixToCartX[nr][nc] = x; 
    PixToCartY[nr][nc] = y; 
  }
}

int TDetector::FitsToPixmap(string fitsname) {
  Int_t nr, nc;
  fitsfile *fptr;         /* FITS file pointer, defined in fitsio.h */
  int status = 0;   /* CFITSIO status value MUST be initialized to zero! */
  long ntrows;
  //int ntcols;
  char TABNAME[] = "PIXMAP";
  
  for (nr=0; nr< PIX_R_DIM; nr++)
    {
      for (nc=0; nc< PIX_C_DIM; nc++)
	{
	  fPixMap[nr][nc] = -1;
	  fPixMask[nr][nc] = 1;
	  fBorderPixel[nr][nc] = 0; 
	} 
    }
  
  if (!fits_open_file(&fptr, fitsname.c_str(), READONLY, &status)) {
    fits_movnam_hdu(fptr, BINARY_TBL, TABNAME,0, &status);
    fits_get_num_rows(fptr, &ntrows, &status);
  
    int I[ntrows], J[ntrows];
    //float X[ntrows], Y[ntrows];

    fits_read_col(fptr, TINT, 1, 1, 1, ntrows, NULL, J, NULL, &status);
    fits_read_col(fptr, TINT, 2, 1, 1, ntrows, NULL, I, NULL, &status);
    fits_read_col(fptr, TFLOAT, 3, 1, 1, ntrows, NULL, X, NULL, &status);
    fits_read_col(fptr, TFLOAT, 4, 1, 1, ntrows, NULL, Y, NULL, &status);  

    for (int ch = 0; ch < ntrows; ch++){
      nr = I[ch];
      nc = J[ch];
      if(nr==0||nr==351)fBorderPixel[nr][nc] = 1;
      fPixMap[nr][nc] = ch;
      PixToCartX[nr][nc] = X[ch]; 
      PixToCartY[nr][nc] = Y[ch]; 
    } 
    if (status == END_OF_FILE)  status = 0; /* Reset after normal error */
    fits_close_file(fptr, &status);
  }
  return (status);
}


Int_t TDetector::ReadMCFile(Int_t entry){
  for (int j=0; j<NCHANS; ++j) fPedSubtrSignal[j] = 0;
  Evtree->GetEntry(entry);  
   for(int ch=0;ch<Clusterdim;ch++)
    {
      if (Channel[ch]<0 || Channel[ch]>NCHANS) {
	cout << "ERROR: non existing channel "<< Channel[ch] << " in evt " 
	     << entry << " ch id "<< ch << "! END OF LOOP"<< endl;
	break;
      }
      fPedSubtrSignal[Channel[ch]] = (Double_t)DigiCharge[ch] - 1400;
    }
   // Create signal matrix.
   for (int nr=0; nr<PIX_R_DIM; ++nr) {
     for (int nc=0; nc<PIX_C_DIM; ++nc) {
       int SirChan = fPixMap[nr][nc]; 
         fSignalMatrix[nr][nc] = -1*fPixMask[nr][nc]*fPedSubtrSignal[SirChan];
	 fCumulativeHitmap[SirChan] += fSignalMatrix[nr][nc];
     }
   }
   return(1);
}


Int_t TDetector::ReadROInew(Int_t numEv) {
  // Roi[0]=xmin; Roi[1]=xmax, Roi[2]=ymin, Roi[3]=ymax.
  Int_t chan;                      
  Char_t tmp1, tmp2;
  numPix = 0;
  for (int jj=0; jj<NCHANS; jj++){
    fRawChannelData[jj] = 0;
    fPedSubtrSignal[jj]= 0;
   }

  for (int jj=0; jj<maxPixTrasm; jj++)fROIRawData[jj] = 0;
  if(numEv==1) cout << "====>>> PIXEL THRESHOLD!  " << fPixelThresh  << "  ADC counts" << endl;
    
  if(HeaderInFile && numEv==1){
    cout << "-------> read Header in rawdata file..." << endl;
    fRawFile->fStream.read((Char_t*)&VersNum, sizeof(Int_t));
    fRawFile->fStream.read((Char_t*)&Counts, sizeof(Int_t));
    fRawFile->fStream.read((Char_t*)&Energy,sizeof(Float_t));
    fRawFile->fStream.read((Char_t*)&PolDegree, sizeof(Float_t));
    fRawFile->fStream.read((Char_t*)&Delta,sizeof(Float_t));   
    fRawFile->fStream.read((Char_t*)&Epsilon, sizeof(Float_t));
    fRawFile->fStream.read((Char_t*)&X1,sizeof(Float_t));
    fRawFile->fStream.read((Char_t*)&Y1, sizeof(Float_t));
    fRawFile->fStream.read((Char_t*)&Z1, sizeof(Float_t));
    fRawFile->fStream.read((Char_t*)&Mixture,sizeof(Mixture));
    
    cout << "=================>>> Read header in rawdata file ............" << endl;
    cout << "Energy: " << Energy<< endl;
    cout << "Polarization degree: " << PolDegree << endl;
    cout << "Delta (deg.)" << Delta << endl;
    cout << "Epsilon (deg.)" << Epsilon << endl;
    cout << "X1: " << X1 << " Y1: " << Y1 <<  " Z1: " << Z1 << endl;
    cout << "Counts: " << Counts << endl;
    cout << "Gas Mixture: " << Mixture << endl;
  }

  int tag;
  int jump = 0; // <-------------- 3/07/2013
  fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));
  fRawFile->fStream.read((Char_t*)&tmp2, sizeof(Char_t));
  tag = COMB(tmp1,tmp2);

  while(tag!=65535){
    if(fRawFile->fStream.eof())return(1);
    //read TAG
    fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));
    fRawFile->fStream.read((Char_t*)&tmp2, sizeof(Char_t));
    tag = COMB(tmp1,tmp2);
  }
 
  // read colMIN, colMAX, rowMIN, rowMAX 
  for(Int_t j=0; j<4; j++){
    fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));
    fRawFile->fStream.read((Char_t*)&tmp2, sizeof(Char_t));
    Roi[j] = COMB(tmp1,tmp2);
  }
  numPix = (Roi[1]+1- Roi[0])*(Roi[3]+1- Roi[2]);

  if(Roi[0]<0 || Roi[1]>299 || Roi[2]<0 || Roi[3]>351){ // <-------------- 3/07/2013
    cout << "---> ERROR!! Found wrong ROI values. Jump the event " << numEv << endl;
    jump = 1;
  }
  if(numPix<=0) {
     cout << "---> ERROR!! Found wrong ROI values. Jump the event " << numEv << endl;
    return(2);
  }
  
  // read BufferID
  fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));
  fRawFile->fStream.read((Char_t*)&tmp2, sizeof(Char_t));
  bufferID = COMB(tmp1,tmp2);
  
  fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));
  fRawFile->fStream.read((Char_t*)&tmp2, sizeof(Char_t));
  time1 = COMB(tmp1,tmp2);
  fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));
  fRawFile->fStream.read((Char_t*)&tmp2, sizeof(Char_t));
  time2 = COMB(tmp1,tmp2);

  tm.push_back(time2);

  if(numEv>=2){
    if((tm[numEv-1]-tm[numEv-2])<0){
      roll++;
    }
  }
  
  timetick = (ULong64_t)time1 + (ULong64_t)time2*65534 + (ULong64_t)roll*TMath::Power(2.,32);
  //timestamp = (Double_t)timetick*0.8; //in microsec  
    
  // next 4 reads now contains the run start time in seconds (given by DAQ gui)
  fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));
  fRawFile->fStream.read((Char_t*)&tmp2, sizeof(Char_t));
  run1 = COMB(tmp1,tmp2);
  fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));
  fRawFile->fStream.read((Char_t*)&tmp2, sizeof(Char_t));
  run2 = COMB(tmp1,tmp2);

  timestamp = (Double_t)(run2 +run1*65536)+ (Double_t)timetick*0.8/1e6; //in seconds

    
  Int_t pixel_counter = 0;
  
  for (int nr=Roi[2]; nr<=Roi[3]; nr++){
    for (int nc=Roi[0]; nc<=Roi[1]; nc++){
      int wch = (nr-Roi[2])+(nc-Roi[0])*(Roi[1]-Roi[0]+1);   
 
       fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));
       fRawFile->fStream.read((Char_t*)&tmp2, sizeof(Char_t));

       if(jump==0){ // <-------------- 3/07/2013
	 chan = fPixMap[nr][nc];
	 fRawChannelData[chan] = COMB(tmp1,tmp2);
	 fPedSubtrSignal[chan] = fRawChannelData[chan];
	 fROIRawData[wch] = fPedSubtrSignal[chan];
	 if (fROIRawData[wch] > fPixelThresh) pixel_counter++;  // count pixel over threshold for cumulative map!!!
       }
     }
   }
   
   if(jump)return(2); // <-------------- 3/07/2013

   // Not sure why we have this check here, let's comment this line for now. see issue
   // https://github.com/lucabaldini/xperecon/issues/2
   //for (Int_t i=0; i<NCHANS; ++i) fPedSubtrSignal[i] = (fPedSubtrSignal[i] > 4000 ? 0 :fPedSubtrSignal[i]); 

  // Create signal matrix.
  for (int nr=0; nr<PIX_R_DIM; ++nr) {
    for (int nc=0; nc<PIX_C_DIM; ++nc) {
      chan = fPixMap[nr][nc]; 
      fSignalMatrix[nr][nc] = fPixMask[nr][nc]*fPedSubtrSignal[chan]; // No inverted signal with new DAQ!!!!
      if (fSignalMatrix[nr][nc] > fPixelThresh  && pixel_counter < MAXCLUSIZE){
	fCumulativeHitmap[chan] += fSignalMatrix[nr][nc];
       }
    }
  }
  return(0);
}


/************************************************************************
 * TDetector::FindClusters - Locate clusters in 2-d and create an array *
 *   of instances of the TCluster class.                                *
 *                                                                      *
 ************************************************************************/
Int_t TDetector::FindClusters() {
  // Float_t BaryRadius;
  //Int_t NewClCounter;
  Int_t ClusterCounter;
  Bool_t OverMaxClusts;
  int nc, nr;
  MatrixInt_t OverThresh;          // Matrix of flags indicating which pixels are over threshold.
  MatrixInt_t AssociatedCluster;   // Matrix indicating to which cluster each pixel is associated 
  counter = 0;

  // Select pixels over threshold.
  for (nr=0; nr<PIX_R_DIM; ++nr) {
    for (nc=0; nc<PIX_C_DIM; ++nc) {
      
      AssociatedCluster[nr][nc] = -1;
      if (MCflag) {
	if (fSignalMatrix[nr][nc] > fPixelThresh*2) OverThresh[nr][nc] = 1; // set the noise for MC data to 2 ADC counts 
	else OverThresh[nr][nc] = 0;
      }
      else {
	if (fSignalMatrix[nr][nc] > fPixelThresh)  OverThresh[nr][nc] = 1;
	else OverThresh[nr][nc] = 0;
      }
    }
  }
  
  // Find clusters algorithm... 
  ClusterCounter = 0;
  OverMaxClusts = kFALSE;
  for (nr=0; nr<PIX_R_DIM; ++nr) {
    for (nc=0; nc<PIX_C_DIM; ++nc) {
      if (OverThresh[nr][nc] && AssociatedCluster[nr][nc] < 0 ) {	  
	// NEW CLUSTER!	
	if (ClusterCounter == MAXNUMCLUSTS) {
	  cout << "\nNumber of clusters in event exceeded maximum ( " << MAXNUMCLUSTS
	       << " ).\n";
	  OverMaxClusts = kTRUE;
	  break;
	}	
	fAllClusts[ClusterCounter] = new TCluster(fSignalMatrix, OverThresh, AssociatedCluster, ClusterCounter, nr, nc, fPixScanRange);  
	if(fAllClusts[ClusterCounter]->fClusterSize <MINCLUSIZE) {
	//if(fAllClusts[ClusterCounter]->fClusterSize <1) {
	  delete fAllClusts[ClusterCounter];
	  fAllClusts[ClusterCounter] = 0;
	  continue;
       	}
	if(fAllClusts[ClusterCounter]->fClusterSize >=MAXCLUSIZE) {
	  delete fAllClusts[ClusterCounter];
	  fAllClusts[ClusterCounter] = 0;
	  return(0);
	  break;
       	}
      	fAllClusts[ClusterCounter]->CalcProperties(fSignalMatrix, fNoiseMatrix, fBorderPixel, PixToCartX, PixToCartY, MCflag, 
						   WeightLengthScale, SmallCircleRadius, WideCircleRadius);
	if(fAllClusts[ClusterCounter]->impactFlag) counter++;
	++ClusterCounter;
      }      
    }   
    if (OverMaxClusts == kTRUE) {
      return(0);
      break;
    }
  }
 
  // Loop over all clusters to evaluate the pixel with the highest charge IN THE EVENT
  // (and not in the clusters themselves).
  Float_t HighestEvtCharge = 0.0;
  // Float_t HighestEvtSize = 0.0;
  //Int_t HighestCluIndex = 0;
  for (int i=0; i<ClusterCounter; i++){
    if (fAllClusts[i]->fHighestCharge > HighestEvtCharge)  {
      //HighestEvtCharge = fAllClusts[i]->fPulseHeight;
      HighestEvtCharge = fAllClusts[i]->fHighestCharge;
      //HighestEvtSize  = fAllClusts[i]->fClusterSize;
      //HighestCluIndex = i;
    }
  }

  // Second loop over the clusters; the highest charge for each cluster is set to the highest
  // charge IN THE EVENT. Necessary for the event display normalization.
  for (int i=0; i<ClusterCounter; i++){
    fAllClusts[i]->fHighestCharge = HighestEvtCharge;
  }
  return (ClusterCounter);
}

