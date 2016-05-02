#include "TDetector.h"
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


Int_t TDetector::ReadMCFile(Int_t entry){
  Int_t C;
  for (Int_t j=0; j<NCHANS; ++j) fPedSubtrSignal[j] = 0;
  C=Evtree->GetEntry(entry);  
   for(Int_t ch=0;ch<Clusterdim;ch++)
    {
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


Int_t TDetector::ReadROInew(Int_t numEv) {
  // Roi[0]=xmin; Roi[1]=xmax, Roi[2]=ymin, Roi[3]=ymax.
  Int_t chan;                      
  Char_t tmp1, tmp2;
  numPix = 0;
  for (Int_t jj=0; jj<NCHANS; jj++){
    fRawChannelData[jj] = 0;
    fPedSubtrSignal[jj]= 0;
   }

  for (Int_t jj=0; jj<maxPixTrasm; jj++)fROIRawData[jj] = 0;
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
	 chan = fPixMap[ix][iy];
	 fRawChannelData[chan] = COMB(tmp1,tmp2);  
	 fPedSubtrSignal[chan] = fRawChannelData[chan];
	 fROIRawData[wch] = -1.*fPedSubtrSignal[chan];
	 if (fROIRawData[wch] > fPixelThresh) pixel_counter++;  // count pixel over threshold for cumulative map!!!
       }
     }
   }
   
   if(jump)return(2); // <-------------- 3/07/2013

   for (Int_t i=0; i<NCHANS; ++i) fPedSubtrSignal[i] = (fPedSubtrSignal[i] > 4000 ? 0 :fPedSubtrSignal[i]);

  // Create signal matrix.
  for (Int_t i=0; i<PIX_X_DIM; ++i) {
    for (Int_t j=0; j<PIX_Y_DIM; ++j) {
      chan = fPixMap[i][j]; 
      fSignalMatrix[i][j] = fPixMask[i][j]*fPedSubtrSignal[chan]; // No inverted signal with new DAQ!!!!
      if (fSignalMatrix[i][j] > fPixelThresh  && pixel_counter < MAXCLUSIZE){
	fCumulativeHitmap[chan] += fSignalMatrix[i][j];
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
	if (fSignalMatrix[i][j] > fPixelThresh)  OverThresh[i][j] = 1;
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
 
  // Loop over all clusters to evaluate the pixel with the highest charge IN THE EVENT
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

  // Second loop over the clusters; the highest charge for each cluster is set to the highest
  // charge IN THE EVENT. Necessary for the event display normalization.
  for (i=0; i<ClusterCounter; i++){
    fAllClusts[i]->fHighestCharge = HighestEvtCharge;
  }
  return (ClusterCounter);
}

