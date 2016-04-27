#include "TDetector_new_v2.h"
#define PAR 1

TDetector::TDetector(TTree *tree, TInputFile *fPix) {
 
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
 
TDetector::TDetector(TInputFile *fPix, TInputFile *fPed, TInputFile *fRaw){

  // Make the member variable pointers fRawFile, etc. point to the various
  // file objects.
  fPixFile = fPix;
  fPedFile = fPed;
  fRawFile = fRaw;
  ifstream gainfile;
  // Read in and create the pixel lookup table.
  CreatePixLookup();
  // Read in pedestal and noise values.
  ReadPeds();
  // Reset the cumulative Hitmap
  for (Int_t i=0; i<NCHANS; i++) fCumulativeHitmap[i] = 0;
  MCflag = 0;
  }


void TDetector::CreatePixLookup() {

  Text_t DummyString[ 8 ];
  Int_t DummyInt;
  Int_t i, j;
  Float_t x,y;
  // Loop over all pixels, reading in the corresponding ADC channel number
  // and mask state from the file.  The first 2 entries on each line of the
  // file are the pixel coordinates - skip these.
  for (i=0; i< PIX_X_DIM; i++)
    {
      for (j=0; j< PIX_Y_DIM; j++)
	{
	  fPixMap[i][j] = -1;
	  fPixMask[i][j] = 0;
	  fBorderPixel[i][j] = 0; 
	} 
    }
  for (i=0; i<2; i++) {
    fPixFile->fStream >> DummyInt;  // Skip file header.
  }

  for (i=0; i<7; ++i) {
    fPixFile->fStream >> DummyString;  // Skip file header.
  }
  while(!fPixFile->fStream.eof()) {
    fPixFile->fStream >> x >> y >> i >> j;
    fPixFile->fStream >> fPixMap[i][j] >> fPixMask[i][j] >> fBorderPixel[i][j];
    PixToCartX[i][j] = x;
    PixToCartY[i][j] = y;
  }
}

void TDetector::ReadPeds() {

  Int_t i, j;
  Int_t Dummy;
  // Read average pedestal values.
  fPedFile->fStream.read((Char_t *)&fPeds, NCHANS*sizeof(Double_t));
   fPedFile->fStream.read((Char_t *)&Dummy, sizeof(Int_t ));
  // Read rms noise values.
  fPedFile->fStream.read((Char_t *)&fSigma, NCHANS*sizeof(Double_t));

  // Channel 0 always has zero noise - put it to same as channel 1.
  fSigma[0] = fSigma[1];

  #ifdef DEBUG
  cout << "\n\n----------------- Pedestals ----------------->>>\n\n";
  for (i=0; i<NCHANS; ++i) if (i<20) cout << i << " ------- " << fPeds[i] << endl;
  #endif

  //cout << "Creating pixel matrix" << endl;
  Int_t SirChan;
  // Put the rms noise data and pedestal data into a pixel matrix.
  for (i=0; i<PIX_X_DIM; ++i) {
    for (j=0; j<PIX_Y_DIM; ++j) {
      fNoiseMatrix[i][j] = 0;
      if (fPixMap[i][j] > -1){
	SirChan = fPixMap[i][j];               // Sirocco channel for pixel ( i, j ).
	fNoiseMatrix[i][j] = fSigma[SirChan];  // Add value to matrix.
      }
    }
  }
}

Int_t TDetector::ReadMCFile(Int_t entry){

  Int_t C;
  for (Int_t j=0; j<NCHANS; ++j) fPedSubtrSignal[j] = 0;
  C=Evtree->GetEntry(entry);  
   for(Int_t ch=0;ch<Clusterdim;ch++)
    {
      //cout << "event: ==>> " << entry << " Channel:" <<Channel[ch]<<" digi charge " << DigiCharge[ch] << endl;
      fPedSubtrSignal[Channel[ch]] = (Double_t)DigiCharge[ch] - 1400;
    }
   // Create signal matrix.
   for (Int_t i=0; i<PIX_X_DIM; ++i) {
     for (Int_t j=0; j<PIX_Y_DIM; ++j) {
       Int_t SirChan = fPixMap[i][j]; 
         fSignalMatrix[i][j] = -1*fPixMask[i][j]*fPedSubtrSignal[SirChan];
	 fCumulativeHitmap[SirChan] += fSignalMatrix[i][j];
     }
   }
   return(1);
}
Int_t TDetector::ReadFullFrame(Int_t numEv) {
  Int_t pixel_counter = 0;
  Char_t tmp1, tmp2;
  Int_t NCLU = 8;
  Int_t CHCLU = NCHANS/NCLU;
  numPix = NCHANS;
 
  for (Int_t jj=0; jj<NCHANS; jj++){
    fRawChannelData[jj] = 0;
    fRawData[jj] = 0;
  }

  for (int ch=0; ch<CHCLU ;ch++){
    for (int clu=0; clu<NCLU ;clu++){      
      fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));
      fRawFile->fStream.read((Char_t*)&tmp2, sizeof(Char_t));
      fRawChannelData[ch+clu*CHCLU] = COMB(tmp1,tmp2);   
      if (fRawChannelData[ch+clu*CHCLU] > fPixelThresh*fSigma[ch+clu*CHCLU]) pixel_counter++;  // count pixel over threshold for cumulative map!!!
    }
  }
  // Subtract pedestal from signal. 
    for (Int_t i=0; i<NCHANS; ++i) {
      if(fRawChannelData[i]){
	fPedSubtrSignal[i] = Float_t(fRawChannelData[i]) - fPeds[i];       
	fRawData[i]= -1.*fPedSubtrSignal[i];
      }
      else fPedSubtrSignal[i] = 0;  
    }

  // Create signal matrix.
  for (Int_t i=0; i<PIX_X_DIM; ++i) {
    for (Int_t j=0; j<PIX_Y_DIM; ++j) {
       Int_t chan = fPixMap[i][j]; 
       // a hit is a POSITIVE signal
       fSignalMatrix[i][j] = fPixMask[i][j]*fPedSubtrSignal[chan]; // No inverted signal with new DAQ!!!!
       fPedSubtrSignalMask[chan] = -1 * fSignalMatrix[i][j]; // Mask to channels NOT APPLIED!!!!
       if (fSignalMatrix[i][j] > fPixelThresh*fSigma[chan]  && pixel_counter <MAXCLUSIZE){
	 fCumulativeHitmap[chan] += fSignalMatrix[i][j];
       }
    }
  }
  return(1);
}


Int_t TDetector::ReadROInew(Int_t numEv) {

  Int_t chan;                      // Roi[0]=xmin; Roi[1]=xmax, Roi[2]=ymin, Roi[3]=ymax.
  Char_t tmp1, tmp2;
  numPix = 0;
  for (Int_t jj=0; jj<NCHANS; jj++){
    fRawChannelData[jj] = 0;
    fPedSubtrSignal[jj]= 0;
  }
  for (Int_t jj=0; jj<maxPixTrasm; jj++)fROIRawData[jj] = 0;
  if(numEv==1){
    if(fThreshFlag) cout << "====>>> FIXED THRESHOLD!  " << fPixelThresh  << "  ADC counts" << endl;
    else  cout << "====>>>  VARIABLE THR!  " << fPixelThresh  << "  sigmas" << endl;
    if(fSubPedFlag)cout <<  "====>>> subtract PEDESTALS!!! " << endl;
   }
   
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
    //gSystem->ProcessEvents(); 
    //read TAG
    fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));
    fRawFile->fStream.read((Char_t*)&tmp2, sizeof(Char_t));
    tag = COMB(tmp1,tmp2);
  }
 
  // read Ymin, Ymax, Xmin, Xmax 
  for(Int_t j=0; j<4; j++){
    fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));
    fRawFile->fStream.read((Char_t*)&tmp2, sizeof(Char_t));
    Roi[j] = COMB(tmp1,tmp2);
  }
  //cout << numEv << "   Ymin: " << Roi[0] << "  Ymax: " << Roi[1] << "  Xmin: " << Roi[2] << "  Xmax: " << Roi[3]<< endl;  
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
  timestamp = (Double_t)(time1 + time2*65534)*0.8; //in microsec
 
  // 4 Dummy read 
  for (int j=0; j<4; j++)fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));

  Int_t pixel_counter = 0;
  // ==>> !!! Swap ix with iy respect to previous versions
  // explanations in readme.txt.- Swap has been done just for coherence with the coord. system BUT nothing has changed 
  // in practice in the results.
   for (Int_t ix=Roi[2]; ix<=Roi[3]; ix++){
     for (Int_t iy=Roi[0]; iy<=Roi[1]; iy++){
       Int_t wch = (ix-Roi[2])+(iy-Roi[0])*(Roi[1]-Roi[0]+1);     
       fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));
       fRawFile->fStream.read((Char_t*)&tmp2, sizeof(Char_t));

       if(jump==0){ // <-------------- 3/07/2013
	 //chan = PIX_Y_DIM * ix + iy;
	 chan = fPixMap[ix][iy];
	 fRawChannelData[chan] = COMB(tmp1,tmp2);  
	 if (fThreshFlag) thr = fPixelThresh;
	 else thr = fPixelThresh*fSigma[chan];
	 if(fSubPedFlag) fPedSubtrSignal[chan] = fRawChannelData[chan]  - fPeds[chan];
	 else fPedSubtrSignal[chan] = fRawChannelData[chan];
	 fROIRawData[wch] = -1.*fPedSubtrSignal[chan];
	 if (fROIRawData[wch] > thr) pixel_counter++;  // count pixel over threshold for cumulative map!!!
       }
     }
   }
   
   if(jump)return(2); // <-------------- 3/07/2013

   for (Int_t i=0; i<NCHANS; ++i) fPedSubtrSignal[i] = (fPedSubtrSignal[i] > 4000 ? 0 :fPedSubtrSignal[i]);
   
  // Create signal matrix.
  for (Int_t i=0; i<PIX_X_DIM; ++i) {
    for (Int_t j=0; j<PIX_Y_DIM; ++j) {
      chan = fPixMap[i][j]; 
      // Invert signal polarity if initially negative ( now a hit means POSITIVE signal ).
      fSignalMatrix[i][j] = fPixMask[i][j]*fPedSubtrSignal[chan]; // No inverted signal with new DAQ!!!!
      fPedSubtrSignalMask[chan] = -1 * fSignalMatrix[i][j]; // Mask to channels NOT APPLIED!!!!
      if (fThreshFlag) thr = fPixelThresh;
      else thr = fPixelThresh*fSigma[chan];
      if (fSignalMatrix[i][j] > thr  && pixel_counter < MAXCLUSIZE){
	fCumulativeHitmap[chan] += fSignalMatrix[i][j];
       }
    }
  }
  return(0);
}


Int_t TDetector::ReadROIFile(Int_t numEv) {

  Int_t chan;                      // Roi[0]=xmin; Roi[1]=xmax, Roi[2]=ymin, Roi[3]=ymax.
  Int_t beginEv;
  Char_t tmp1, tmp2;
  Int_t maxPixTrasm = 1195; //1195; //2035; // At the moment a max of 2035 pixels data is trasmitted in self trigger mode (March 2006)
  Int_t flagret = 1;
  numPix = 0;
  for (Int_t jj=0; jj<NCHANS; jj++)fRawChannelData[jj] = 0;
  beginEv = 0;
     
  while(beginEv != 32767){
    fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));
    fRawFile->fStream.read((Char_t *)&tmp2, sizeof(Char_t));
    beginEv = COMB(tmp1,tmp2);
    if(fRawFile->fStream.eof()) { 
      flagret = 0;
      break;
    }
  }
  if(!flagret) return(flagret);

  for(Int_t j=0; j<4; j++){
    fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));
    fRawFile->fStream.read((Char_t *)&tmp2, sizeof(Char_t));
    Roi[j] = COMB(tmp1,tmp2);
  }


  numPix = (Roi[1]+1- Roi[0])*(Roi[3]+1- Roi[2]);
  
  //if (numPix>=maxPixTrasm && numPix<2000){
  if (numPix>=maxPixTrasm){
    for (Int_t i=0; i<maxPixTrasm; i++){ 
      fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));
      fRawFile->fStream.read((Char_t *)&tmp2, sizeof(Char_t));
    }
    //return(1); // li accetto
    return(2);  //li butto via
  }

   Int_t pixel_counter = 0;
   if(numEv==1){
     if(fThreshFlag) cout << "====>>> FIXED THRESHOLD!  " << fPixelThresh  << "  ADC counts" << endl;
     else  cout << "====>>>  VARIABLE THR!  " << fPixelThresh  << "  sigmas" << endl;
   }
   for (Int_t iy=Roi[2]; iy<=Roi[3]; iy++)
     for (Int_t ix=Roi[0]; ix<=Roi[1]; ix++){
       chan = PIX_Y_DIM * ix + iy;
       fRawFile->fStream.read((Char_t*)&tmp1, sizeof(Char_t));
       fRawFile->fStream.read((Char_t *)&tmp2, sizeof(Char_t));
       fRawChannelData[chan] = COMB(tmp1,tmp2);
       if (fThreshFlag) thr = fPixelThresh;
       else thr = fPixelThresh*fSigma[chan];
       if (fRawChannelData[chan] > thr) pixel_counter++;  // count pixel over threshold for cumulative map!!!
       if(COMB(tmp1,tmp2)==32767) {
	 cout << "Found header new event reading the matrix --- numPixels  " << numPix << endl;
       }
     }

   //if(numEv<202000) return(2);

  for (Int_t i=0; i<NCHANS; ++i) fRawChannelData[i] = (fRawChannelData[i] > 4000 ? 0 : fRawChannelData[i]);

  //////////////////////////////////////////////////////////////

    // Subtract pedestal from signal.  //// For the moment all channels are considered, set fRawChannelData = 0 out of the ROI
    for (Int_t i=0; i<NCHANS; ++i) {
      if(fRawChannelData[i])
	//fPedSubtrSignal[i] = Float_t(fRawChannelData[i])  - fPeds[i];
	fPedSubtrSignal[i] = Float_t(fRawChannelData[i]); /// <<<======= for data pedestal subtracted (17-03-2006)
      else fPedSubtrSignal[i] = 0;           // if inserted for the case of input filr with ROI matrix data (all zeros except in matrix)
    }
  
  // Create signal matrix.
  for (Int_t i=0; i<PIX_X_DIM; ++i) {
    for (Int_t j=0; j<PIX_Y_DIM; ++j) {
       chan = fPixMap[i][j]; 
       // Invert signal polarity if initially negative ( now a hit means POSITIVE signal ).
       fSignalMatrix[i][j] = fPixMask[i][j]*fPedSubtrSignal[chan]; // No inverted signal with new DAQ!!!!
       fPedSubtrSignalMask[chan] = -1 * fSignalMatrix[i][j]; // Mask to channels NOT APPLIED!!!!
       if (fThreshFlag) thr = fPixelThresh;
       else thr = fPixelThresh*fSigma[chan];
       if (fSignalMatrix[i][j] > thr  && pixel_counter < 200){
	 fCumulativeHitmap[chan] += fSignalMatrix[i][j];
       }
    }
  }
  
  return(1);
}


Int_t TDetector::ReadRawFile() {
  
  Adcword_t Dummy[2];
  Int_t Dummy2;
  Int_t GlobId;
  Int_t i, j;
  Int_t SirChan;
  Clusterdim = 0;
  // There are 4 bytes of superfluous data at the start and end of each
  // data block for each detector.
  fRawFile->fStream.read((Char_t *)&Dummy, 2*sizeof(Adcword_t));
  // Get global ID (but do nothing with it for now!)
  fRawFile->fStream.read((Char_t *)&GlobId, sizeof(Int_t));
  // Read the ADC data.
  fRawFile->fStream.read((Char_t *)&fRawChannelData, NCHANS*sizeof(Adcword_t));
  //fRawFile->fStream.read((Char_t *)&Dummy, 2*sizeof(Int_t));
  //Event num in now an integer
  fRawFile->fStream.read((Char_t *)&Dummy2, sizeof(Int_t));
  //////////////////////////////////////////////////
  for (i=0; i<NCHANS; ++i) fRawChannelData[i] = (fRawChannelData[i] > 4000 ? 0 : fRawChannelData[i]);
  //////////////////////////////////////////////////

#ifdef DEBUG
  cout << "\n\n----------------- Raw data ----------------->>>\n\n";
  cout << "GlobId = " << GlobId << "\n";
  for (i=0; i<NCHANS; ++i) {
    cout << fRawChannelData[i] << "\t";
  }
#endif

  /////////////////////////////////////////////////////// Modified for ROI Matrix data Feb 24, 2006 by Gloria
  Int_t flagMatrix = 1;

    if (flagMatrix) {
  // Subtract pedestal from signal.
  for ( i=0; i<NCHANS; ++i ) {
    if(fRawChannelData[i]){
      fPedSubtrSignal[i] = Float_t(fRawChannelData[i])  - fPeds[i];
      //cout << i << " ---- " << fRawChannelData[i] << "  ------  " << fPeds[i] << endl;
    }
    else fPedSubtrSignal[i] = 0;           // if inserted for the case of input filr with ROI matrix data (all zeros except in matrix)

  }
    }
    else 
      for (i=0; i<NCHANS; ++i) fPedSubtrSignal[i] = Float_t(fRawChannelData[i])  - fPeds[i];

  //////////////////////////////////////////////////////////////

  //CheckBaselineJump();
   
   // Create signal matrix.
   for (i=0; i<PIX_X_DIM; ++i) {
     for (j=0; j<PIX_Y_DIM; ++j) {
       SirChan = fPixMap[i][j]; 
       // Invert signal polarity if initially negative ( now a hit means POSITIVE signal ).
       fSignalMatrix[i][j] = fPixMask[i][j]*fPedSubtrSignal[SirChan]; // No inverted signal with new DAQ!!!!
       fPedSubtrSignalMask[SirChan] = -1 * fSignalMatrix[i][j]; // Mask to channels NOT APPLIED!!!!
       if (fThreshFlag) thr = fPixelThresh;
       else thr = fPixelThresh*fSigma[SirChan];
       if (fSignalMatrix[i][j] > thr){
      fCumulativeHitmap[SirChan] += fSignalMatrix[i][j];
    }
     }
   }
   
#ifdef DEBUG
   cout << "\n\n----------------- Signal matrix ----------------->>>\n\n";
   for (i=0; i<PIX_X_DIM; ++i) {
     for (j=0; j<PIX_Y_DIM; ++j) { 
       cout << fSignalMatrix[i][j] << endl;
     }
   }
#endif
   return(1);
}



void TDetector::CheckBaselineJump() {

  Float_t FiredChannelThreshold = 25.0;
  Int_t nJumpingChannels = 0;
  Float_t Jump = 0.0;

  // Check for coherent jumps in the baseline.
  for ( int i=0; i<NCHANS; ++i ) {
    if (fPedSubtrSignal[i] < FiredChannelThreshold){
       Jump += fPedSubtrSignal[i];
      nJumpingChannels += 1;
    }
  }
  Jump /= nJumpingChannels;
   
  // Correct the signal for the jump. 
  for ( int i=0; i<NCHANS; ++i ) fPedSubtrSignal[i] -= Jump; 
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
  Int_t i, j;
  Bool_t OverMaxClusts;

  MatrixInt_t OverThresh;          // Matrix of flags indicating which pixels are over threshold.
  MatrixInt_t AssociatedCluster;   // Matrix indicating to which cluster each pixel is associated 
  counter = 0;

  // Select pixels over threshold.

  for (i=0; i<PIX_X_DIM; ++i) {
    for (j=0; j<PIX_Y_DIM; ++j) {
      
      AssociatedCluster[i][j] = -1;
      if (MCflag) {
	if (fSignalMatrix[i][j] > fPixelThresh*2) OverThresh[i][j] = 1; // set the noise for MC data to 2 ADC counts 
	else OverThresh[i][j] = 0;
      }
      else {
	if (fThreshFlag) thr = fPixelThresh;
	else thr = fPixelThresh*fNoiseMatrix[i][j];
	if (fSignalMatrix[i][j] > thr)  OverThresh[i][j] = 1;
	else OverThresh[i][j] = 0;
      }
    }
  }
  
  // Find clusters algorithm...
  
  ClusterCounter = 0;
  OverMaxClusts = kFALSE;
  
  for (i=0; i<PIX_X_DIM; ++i) {

    for (j=0; j<PIX_Y_DIM; ++j) {
      
      if (OverThresh[i][j] && AssociatedCluster[i][j] < 0 ) {
	  
	// NEW CLUSTER!
	
	if (ClusterCounter == MAXNUMCLUSTS) {
	  cout << "\nNumber of clusters in event exceeded maximum ( " << MAXNUMCLUSTS
	       << " ).\n";
	  OverMaxClusts = kTRUE;
	  break;
	}
	
	fAllClusts[ClusterCounter] = new TCluster(fSignalMatrix, OverThresh, AssociatedCluster, ClusterCounter, i, j, fPixScanRange);
  

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
  


  ///////////// write out info on cluster for reconstruction algo study 
  /*  
  if(ClusterCounter==1){ 
    cout << fAllClusts[0]->fClusterSize <<  endl;
    cout  << Phi << " " << XI << " " << YI << endl;
    for (Int_t j =0; j<fAllClusts[0]->fClusterSize; j++){
      Int_t up = (Int_t)fAllClusts[0]->fPixelsInCluster[j][0];
      Int_t vp = (Int_t)fAllClusts[0]->fPixelsInCluster[j][1];
      cout << up << " " << vp << " " << fSignalMatrix[up][vp] << " " << PixToCartX[up][vp] << " " << PixToCartY[up][vp] << endl;
    }
  }
  */
  /////////////


  // Loop over all clusters as to evaluate the pixel with the highest charge IN THE EVENT
  // (and not in the clusters themselves).
  Float_t HighestEvtCharge = 0.0;
  Float_t HighestEvtSize = 0.0;
  Int_t HighestCluIndex = 0;
  for (i=0; i<ClusterCounter; i++){
    if (fAllClusts[i]->fHighestCharge > HighestEvtCharge)  {
      //HighestEvtCharge = fAllClusts[i]->fPulseHeight;
      HighestEvtCharge = fAllClusts[i]->fHighestCharge;
      HighestEvtSize  = fAllClusts[i]->fClusterSize;
      HighestCluIndex = i;
    }
  }
  /*

  //For offset study within ROI
  //cout  << "  " << Roi[0] << "  " << Roi[1] << "  " << Roi[2] << "  " << Roi[3]  << "  " << HighestEvtCharge << "  " << HighestEvtSize << "  " << ClusterCounter << endl;
  //For offset variations study in the event region (taken cluster with the highest charge)  
  cout  << "  " <<  HighestEvtSize << endl;
  for (Int_t j =0; j<HighestEvtSize; j++){
    Int_t up = (Int_t)fAllClusts[HighestCluIndex]->fPixelsInCluster[j][0];
    Int_t vp = (Int_t)fAllClusts[HighestCluIndex]->fPixelsInCluster[j][1];
    //cout << up <<  " " << vp << "  --- " << (Int_t)fPixMap[up][vp] << endl;
    cout << " " << (Int_t)fPixMap[up][vp];
}
  cout << endl;

  */

  // Second loop over the clusters; the highest charge for each cluster is set to the highest
  // charge IN THE EVENT. Necessary for the event display normalization.
  for (i=0; i<ClusterCounter; i++){
    fAllClusts[i]->fHighestCharge = HighestEvtCharge;
  }
  return (ClusterCounter);
}

