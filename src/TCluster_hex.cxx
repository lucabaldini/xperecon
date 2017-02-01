#include "TCluster_hex.h"

/************************************************************************
 * TCluster::TCluster - Constructor.                                    *
 *                      Initialise all physics parameters and create    *
 *                      array of pixels associated with this cluster.   *
 ************************************************************************/

TCluster::TCluster(MatrixFloat_t SignalMatrix, MatrixInt_t OverThresh, MatrixInt_t AssociatedCluster, 
		    Int_t ClusterNum, Int_t SeedPixelX, Int_t SeedPixelY, Int_t PixScanRange) {
  Int_t ExploredPixels;
  Int_t StartPixelX, StartPixelY;
  Int_t i, j;
  //--------------------------------------------------
  fClusterNumber = ClusterNum;
  fPulseHeight = 0.0;
  fSignalToNoise = 0.0;
  fTotalNoise = 0.0;
  fCenterOfGravityX = fCenterOfGravityY = 0.0;
  fClusterSize = 0;
  fTheta = 0.0;
  fTheta2 = 0.0;
  fMomentumX = 0.0;
  fMomentumY = 0.0;
  //---------------------------------------------------
  // Determine which of the pixels above threshold belong to this cluster.
  fClusterSize = 1;   // Since one pixel already found over thresh, initialise cluster size to 1.
  ExploredPixels = 0;   // Zero the number of explored pixels other than the one already found.
  // Set the X and Y coords of the first pixel in the cluster (pixel 0) to the values for the
  // seed pixel passed from TDetector.cxx...
  fPixelsInCluster[0][0] = SeedPixelX;
  fPixelsInCluster[0][1] = SeedPixelY;
  AssociatedCluster[SeedPixelX][SeedPixelY] = fClusterNumber;  // Set associated cluster for pixel 0.
  // Initialise variables for tracking pixel with highest signal.
  HighestSig = SignalMatrix[SeedPixelX][SeedPixelY];
  fHighestPix = 0;

  i = 0;
  j = 0;

   while(ExploredPixels < fClusterSize) {
    // Look in a hexagonal region around the seed pixel for other hits above threshold.  Update cluster size
    // counter if found.  Then do the same for all pixels adjacent to the seed, each time updating the ExploredPixels
    // counter.  Continue in this way and exit loop when all pixels in cluster have been explored.  The ExploredPixels
    // counter will always be lower than the cluster size until the end of this process.

    // Set the coordinates of the pixel being explored.
    StartPixelX = fPixelsInCluster[ExploredPixels][0];
    StartPixelY = fPixelsInCluster[ExploredPixels][1];	

    // For the moment PixScanRange not used   <<<<============
    /// i,j of neighbour pixels change if StartPixelX (& StartPixelY) are even or odd 

    ///Pixel low left respect to seed ///1
    if(StartPixelX%2==0) { // i pari  j pari o dispari
      i = StartPixelX - 1;
      if(i < 0) i = 0;
      j = StartPixelY;
    }
    else {                /// i dispari j pari o dispari
      i = StartPixelX - 1;
      if(i < 0) i = 0;
      j = StartPixelY - 1;
      if(j < 0) j = 0;
    }
    if (OverThresh[i][j] && AssociatedCluster[i][j] < 0 ) {
      fPixelsInCluster[fClusterSize][0] = i;
      fPixelsInCluster[fClusterSize][1] = j;
      AssociatedCluster[i][j] = fClusterNumber;
      // Keep track of the pixel with highest signal.
      if (SignalMatrix[i][j] > HighestSig) {
	HighestSig = SignalMatrix[i][j];
	fHighestPix = fClusterSize;
      }
      ++fClusterSize;
      if (fClusterSize == MAXCLUSIZE) return;
    }
    
    //Pixel high left respect to seed  ///2
    if(StartPixelX%2==0) {
      i = StartPixelX - 1;
      if(i < 0) i = 0;
      j = StartPixelY + 1;
      if(j >= 300) j = 299; 
    }
    else {
      i = StartPixelX - 1;
      if(i < 0) i = 0;
      j = StartPixelY;
    }
     if (OverThresh[i][j] && AssociatedCluster[i][j] < 0 ) {
      fPixelsInCluster[fClusterSize][0] = i;
      fPixelsInCluster[fClusterSize][1] = j;
      AssociatedCluster[i][j] = fClusterNumber;
      // Keep track of the pixel with highest signal.
      if (SignalMatrix[i][j] > HighestSig) {
	HighestSig = SignalMatrix[i][j];
	fHighestPix = fClusterSize;
      }
      ++fClusterSize;
      if (fClusterSize == MAXCLUSIZE) return;
    }
    
    //Pixel top respect to seed  ///3
    i = StartPixelX;
    j = StartPixelY + 1;
    if (j >= 300) j = 299; 
    if (OverThresh[i][j] && AssociatedCluster[i][j] < 0 ) {
      fPixelsInCluster[fClusterSize][0] = i;
      fPixelsInCluster[fClusterSize][1] = j;
      AssociatedCluster[i][j] = fClusterNumber;
      // Keep track of the pixel with highest signal.
      if (SignalMatrix[i][j] > HighestSig) {
	HighestSig = SignalMatrix[i][j];
	fHighestPix = fClusterSize;
      }
      ++fClusterSize;
      if (fClusterSize == MAXCLUSIZE) return;
    }
    
    //Pixel high right respect to seed  ///4
    if(StartPixelX%2==0) {
      i = StartPixelX + 1;
      if(i >= 352) i = 351;
      j = StartPixelY + 1;
      if(j >= 300) j = 299; 
    }
    else {
      i = StartPixelX + 1;
      if(i >= 352) i = 351;
      j = StartPixelY;
    }
    if (OverThresh[i][j] && AssociatedCluster[i][j] < 0 ) {
      fPixelsInCluster[fClusterSize][0] = i;
      fPixelsInCluster[fClusterSize][1] = j;
      AssociatedCluster[i][j] = fClusterNumber;
      // Keep track of the pixel with highest signal.
      if (SignalMatrix[i][j] > HighestSig) {
	HighestSig = SignalMatrix[i][j];
	fHighestPix = fClusterSize;
      }
      ++fClusterSize;
      if (fClusterSize == MAXCLUSIZE) return;
    }
    
    //Pixel low right respect to seed  ///5
    if(StartPixelX%2==0) {
      i = StartPixelX + 1;
      if (i >= 352) i = 351;
      j = StartPixelY;
    }
    else {
      i = StartPixelX + 1;
      if (i >= 352) i = 351;
      j = StartPixelY - 1;
      if(j < 0) j = 0;
    }
    if (OverThresh[i][j] && AssociatedCluster[i][j] < 0 ) {
      fPixelsInCluster[fClusterSize][0] = i;
      fPixelsInCluster[fClusterSize][1] = j;
      AssociatedCluster[i][j] = fClusterNumber;
      // Keep track of the pixel with highest signal.
      if (SignalMatrix[i][j] > HighestSig) {
	HighestSig = SignalMatrix[i][j];
	fHighestPix = fClusterSize;
      }
      ++fClusterSize;
      if (fClusterSize == MAXCLUSIZE) return;
    }
    //////////////////////////////////////////////
    
    //Pixel bottom respect to seed  ///6
    i = StartPixelX;
    j = StartPixelY-1;
    if(j < 0) j = 0;
    if (OverThresh[i][j] && AssociatedCluster[i][j] < 0 ) {
      fPixelsInCluster[fClusterSize][0] = i;
      fPixelsInCluster[fClusterSize][1] = j;
      AssociatedCluster[i][j] = fClusterNumber;
      // Keep track of the pixel with highest signal.
      if (SignalMatrix[i][j] > HighestSig) {
	HighestSig = SignalMatrix[i][j];
	fHighestPix = fClusterSize;
      }
      ++fClusterSize;
      if (fClusterSize == MAXCLUSIZE) return;
    }
    ++ExploredPixels;
  }
}

/************************************************************************
 * TCluster::CalcProperties - Determine the cluster properties, such as *
 *                            pulse height, centers of gravity, etc.    *
 ************************************************************************/

void TCluster::CalcProperties(MatrixFloat_t SignalMatrix, MatrixFloat_t NoiseMatrix, UShort_t BorderPixel[PIX_R_DIM][PIX_C_DIM],
			      MatrixFloat_t PixToCartX, MatrixFloat_t PixToCartY, Int_t MCflag, Float_t Weight,  
			      Float_t SmallCircleRadius, Float_t WideCircleRadius) {
  Float_t XResid, YResid; 
  Float_t A = 0.0, B = 0.0;
  Float_t CosTh, SinTh;
  Float_t MomX;
  Int_t u, v;
  Float_t x, y;       ;
  Int_t i;
  fHighestCharge = 0;
  fTotalNoise = 0;
  fSignalToNoise = 0;
  impactFlag = 0;
  for (i=0; i<fClusterSize; ++i) {
    u = fPixelsInCluster[i][0];
    v = fPixelsInCluster[i][1];
    x = PixToCartX[u][v];
    y = PixToCartY[u][v];
    fPixelsCoordInCluster[i][0] = x;
    fPixelsCoordInCluster[i][1] = y;
    fPulseHeight += SignalMatrix[u][v];
    if(SignalMatrix[u][v] > fHighestCharge) fHighestCharge = SignalMatrix[u][v];
    if (BorderPixel[u][v]) fBorderCharge += SignalMatrix[u][v];  // Total cluster charge on border pixels.
    fCenterOfGravityX += x*SignalMatrix[u][v];
    fCenterOfGravityY += y*SignalMatrix[u][v];
    if (!MCflag) fTotalNoise += NoiseMatrix[u][v]*NoiseMatrix[u][v];
    else fTotalNoise +=2*2;
  }
  fTotalNoise = sqrt(fTotalNoise);
  if (fTotalNoise) fSignalToNoise = fPulseHeight/fTotalNoise;
  if (fPulseHeight) {
    fCenterOfGravityX /= fPulseHeight;
    fCenterOfGravityY /= fPulseHeight;
  }    
  
  // Smearing...
  // Cluster has all hits aligned in X:
  if (!(fCenterOfGravityX - int(fCenterOfGravityX))) {
    fCenterOfGravityX += (rand()/float(RAND_MAX)*kSmearingFact);
  }
  // Cluster has all hits aligned in Y:
  if (!(fCenterOfGravityY - int( fCenterOfGravityY))) {
    fCenterOfGravityY += (rand()/float(RAND_MAX)*kSmearingFact);
  }
  // Single pixel cluster:
  if (fClusterSize == 1) {
    fCenterOfGravityX += (rand()/float(RAND_MAX)*kSmearingFact);
    fCenterOfGravityY += (rand()/float(RAND_MAX)*kSmearingFact);
  }
  

  // *************** POLARISATION STUDIES ****************
  // This stuff needs large clusters!
  if (fClusterSize < 3) return;
  // Angle of main axis of cluster.
  // Calculate the angle (between -kPI/4 and kPI/4 ) that minimize fMomentumX for rotations around the baricenter.
  for (i=0; i<fClusterSize; ++i) {
    x = fPixelsCoordInCluster[i][0];
    y = fPixelsCoordInCluster[i][1];
    u = fPixelsInCluster[i][0];
    v = fPixelsInCluster[i][1];
    XResid = x - fCenterOfGravityX;
    YResid = y - fCenterOfGravityY;
    A += SignalMatrix[u][v]*XResid*YResid;
    B += SignalMatrix[u][v]*(-XResid*XResid + YResid*YResid);
  }

  fTheta = -0.5*TMath::ATan2((double)2.0*A,(double)B);
  CosTh = cos(fTheta);
  SinTh = sin(fTheta);

  // Now calculate the principal momenta of the cluster.
  // Calculate Second and Third momenta along principal axes.
  fThirdMomentumX = 0;
  fThirdMomentumY = 0;
  for ( i=0; i<fClusterSize; ++i ) {
    u = fPixelsInCluster[i][0];
    v = fPixelsInCluster[i][1];
    x = fPixelsCoordInCluster[i][0];
    y = fPixelsCoordInCluster[i][1];
    XResid = x - fCenterOfGravityX;
    YResid = y - fCenterOfGravityY;
    fMomentumX += Float_t(SignalMatrix[u][v]*TMath::Power((double)(CosTh*XResid + SinTh*YResid), 2));
    fMomentumY += Float_t(SignalMatrix[u][v]*TMath::Power((double)(-SinTh*XResid + CosTh*YResid), 2));
    fThirdMomentumX += Float_t(SignalMatrix[u][v]*TMath::Power((double)(CosTh*XResid + SinTh*YResid), 3));
    fThirdMomentumY += Float_t(SignalMatrix[u][v]*TMath::Power((double)(-SinTh*XResid + CosTh*YResid), 3));
  }
  if (fPulseHeight > 0.0) {
    fMomentumX /= fPulseHeight;
    fMomentumY /= fPulseHeight;
    fThirdMomentumX /= fPulseHeight;
    fThirdMomentumY /= fPulseHeight;
  }
  // At the moment theta represents the angle of the cluster axis ( major OR minor ) which lies in the range
  // -kPI/4 -> kPI/4.  Now that we can distinguish major and minor axes ( from the momenta ), we can translate
  // theta to represent the angle of just the MAJOR axis ( ranging from -kPI/2 to kPI/2 ).
  Float_t TMomX;


  if (fMomentumX < fMomentumY) {
    MomX = fMomentumX;
    fMomentumX = fMomentumY;
    fMomentumY = MomX;
    TMomX = fThirdMomentumX;
    fThirdMomentumX = fThirdMomentumY;
    fThirdMomentumY = TMomX; 
    if (fTheta > 0.0) fTheta -= 0.5*kPI;
    else fTheta += 0.5*kPI;
    if (fTheta < 0.0)
      fThirdMomentumX = -fThirdMomentumX;
  }
  CosTh = cos(fTheta);
  SinTh = sin(fTheta);
  Shape = fMomentumX/fMomentumY;


  Float_t projMax = -999999;
  Float_t projMin = 999999;
  for ( i=0; i<fClusterSize; ++i ) {
    Float_t projection = CosTh*fPixelsCoordInCluster[i][0]+SinTh*fPixelsCoordInCluster[i][1];
    if(projection>projMax) projMax = projection;
    if(projection<projMin) projMin = projection;
  }
  cluLenght = projMax - projMin;


  // Algorithm 2: evaluate baricenter of all the pixels ~2 stdv far from
  // the baricenter, checking that they lie on the "right side".

  Bool_t DEBUG_ALGO = 0;
  Float_t fPartialPHeight = 0;
  Float_t fDistanceFromBaricenter = 0; 
  Int_t   pixelcounter = 0;
  fImpactPosX = 0;
  fImpactPosY = 0;
  
  if (SmallCircleRadius && WideCircleRadius){
    for ( i=0; i<fClusterSize; ++i ) {
      u = fPixelsInCluster[i][0];
      v = fPixelsInCluster[i][1];
      x = fPixelsCoordInCluster[i][0];
      y = fPixelsCoordInCluster[i][1];
      XResid = x - fCenterOfGravityX;
      YResid = y - fCenterOfGravityY;
      if (fMomentumX != 0.0 && fThirdMomentumX != 0.0)
	{
	  fDistanceFromBaricenter = sqrt(XResid*XResid + YResid*YResid)/sqrt(fMomentumX);
	  if (fDistanceFromBaricenter > SmallCircleRadius && fDistanceFromBaricenter < WideCircleRadius)
	    {
	      if ((CosTh*XResid + SinTh*YResid)/fThirdMomentumX > 0.0)
		{
		  fImpactPosX += Float_t(SignalMatrix[u][v]*x);
		  fImpactPosY += Float_t(SignalMatrix[u][v]*y);
		  fPartialPHeight += Float_t(SignalMatrix[u][v]);
		  impactFlag = 1;
		}
	    }
	}
    }
  }
  else {
  
    // Conditions for SmallRadius and WideRadius from MC (Francesco C.- 10 Oct 2005)
    Float_t PH[14] = {500.,900.,1100.,1350.,1450.,1550.,1650.,1750.,1900.,2150.,2350.,2650.,3100.,999999.};
    Float_t RMIN[13] ={0.5,0.5,1.3,2.5,1.9,1.9,1.3,2.3,2.7,1.7,1.7,2.5,2.5};
    Float_t RMAX[13] = {1.,2.5,3.3,3.,2.4,3.9,3.3,4.3,3.7,2.7,2.7,4.5,3.5};
   
  // ==========>>>>> NEW Conditions for SmallRadius and WideRadius in accordance with parameters in MC /29 Nov. 2006 
  //  Float_t PH[10] = {540.,670.,930.,1190.,1440.,1700.,1960.,2220.,2990.,99999.};
  //  Float_t RMIN[10] ={0.4, 0.9, 1.1, 1.3, 1.5, 1.55, 1.65, 1.7, 2., 2.5};
  //  Float_t RMAX[10] = {1., 1.5, 2.0, 2.3, 2.5, 2.7, 2.8, 3.0, 4.0, 4.};
  // ==========>>>>>===============================================================================================
 
    for (Int_t index =0; index<13; index++) {
      if(fPulseHeight<=PH[index+1] && fPulseHeight>=PH[index]){
	index0 = index;
	break;
      }
    }
    //Starting point
    SmallCircleRadius = RMIN[index0];
    WideCircleRadius = RMAX[index0];                     // Implementazione di Francesco nell'analisi MC parte se SmallRadiuse WideRadius sono entrambi zero
    while(pixelcounter <= 4 && SmallCircleRadius > 0.2 ){// partendo da un SmallRadius fissato e dipendente dal PH, finche' non ho un numero di pixel 
      pixelcounter = 0;                                  // sufficiente (>=4) riduco SmallRadius (ogni volta di 0.2) fino ad un minimo di 0.2.
      fImpactPosX = 0;
      fImpactPosY = 0;
      fPartialPHeight = 0;
      impactFlag = 0;
      
      for ( i=0; i<fClusterSize; ++i ) {
	u = fPixelsInCluster[i][0];
	v = fPixelsInCluster[i][1];
	x = fPixelsCoordInCluster[i][0];
	y = fPixelsCoordInCluster[i][1];
	XResid = x - fCenterOfGravityX;
	YResid = y - fCenterOfGravityY;
	if (fMomentumX != 0.0 && fThirdMomentumX != 0.0)
	  {
	    fDistanceFromBaricenter = sqrt(XResid*XResid + YResid*YResid)/sqrt(fMomentumX);
	    if (fDistanceFromBaricenter > SmallCircleRadius && fDistanceFromBaricenter < WideCircleRadius 
		&& ((CosTh*XResid + SinTh*YResid)/fThirdMomentumX) > 0.)
	    {	    
	      pixelcounter++;
	      fImpactPosX += Float_t(SignalMatrix[u][v]*x);
	      fImpactPosY += Float_t(SignalMatrix[u][v]*y);
	      fPartialPHeight += Float_t(SignalMatrix[u][v]);
	      impactFlag = 1;
	    }
	    
	  }
	else {
		fImpactPosX = fCenterOfGravityX;
		fImpactPosY = fCenterOfGravityY;
		fPartialPHeight = 1;
	}
      }// end loop on cluster!!
      
      SmallCircleRadius -= 0.2;
    }// while loop.
  }
  
  if(fPartialPHeight)
    {
      fImpactPosX /= fPartialPHeight;
      fImpactPosY /= fPartialPHeight;
    }
  else {
    fImpactPosX = fCenterOfGravityX;
    fImpactPosY = fCenterOfGravityY;
  }
  
  if (DEBUG_ALGO)
    {
      cout << "Partial pulse height: " << fPartialPHeight << endl;
      cout << "Reconstructed conversion point (x, y): " << fImpactPosX << " " << fImpactPosY << endl;
    }
  // Again on Theta!
  // Now that we know the sign of the third momentum, we can even
  // distinguish the direction of the photoelectron (ranging from -kPI to kPI).
  if (fThirdMomentumX > 0){
    if (fTheta > 0) fTheta -= kPI;
    else fTheta += kPI;
  }
  // Try to use the reconstructed conversion point as to have a better estimate for Theta
  // To be implemented!
  fTheta2 = fTheta;
  // Evaluate new baricenter and new Pulse Height.
  Float_t fBaricenter2X = 0.0;
  Float_t fBaricenter2Y = 0.0;
  Float_t fPulseHeight2 = 0.0;
  for (i=0; i<fClusterSize; ++i) {
    u = fPixelsInCluster[i][0];
    v = fPixelsInCluster[i][1];
    x = PixToCartX[u][v];
    y = PixToCartY[u][v];
    Float_t DistanceFromStart = sqrt((x-fImpactPosX)*(x-fImpactPosX) + (y-fImpactPosY)*(y-fImpactPosY));
    fPixelsCoordInCluster[i][0] = x;
    fPixelsCoordInCluster[i][1] = y;
    fPulseHeight2 += SignalMatrix[u][v]*PixelWeight(DistanceFromStart, Shape, fThirdMomentumX,Weight);
    fBaricenter2X += x*SignalMatrix[u][v]*PixelWeight(DistanceFromStart, Shape, fThirdMomentumX,Weight);
    fBaricenter2Y += y*SignalMatrix[u][v]*PixelWeight(DistanceFromStart, Shape, fThirdMomentumX,Weight);
  }
  if (fPulseHeight2) {
    fBaricenter2X /= fPulseHeight2;
    fBaricenter2Y /= fPulseHeight2;
  }
  // Angle of main axis of cluster (between -kPI/4 and kPI/4 ).
  Float_t A2 = 0.0;
  Float_t B2 = 0.0;
  Float_t CosTh2, SinTh2;
  for (i=0; i<fClusterSize; ++i) {
    x = fPixelsCoordInCluster[i][0];
    y = fPixelsCoordInCluster[i][1];
    XResid = x - fBaricenter2X;
    YResid = y - fBaricenter2Y;
    u = fPixelsInCluster[i][0];
    v = fPixelsInCluster[i][1];
    Float_t DistanceFromStart = sqrt((x-fImpactPosX)*(x-fImpactPosX) + (y-fImpactPosY)*(y-fImpactPosY));
    A2 += SignalMatrix[u][v]*XResid*YResid*PixelWeight(DistanceFromStart, Shape, fThirdMomentumX,Weight);
    B2 += SignalMatrix[u][v]*(-XResid*XResid + YResid*YResid)*PixelWeight(DistanceFromStart, Shape, fThirdMomentumX,Weight);
  }

  fTheta2 = -0.5*TMath::ATan2((double)2.0*A2,(double)B2);
  CosTh2 = cos(fTheta2);
  SinTh2 = sin(fTheta2);
  // Now calculate the principal momenta of the cluster.
  Float_t fMomentum2X = 0;
  Float_t fMomentum2Y = 0;
  for ( i=0; i<fClusterSize; ++i ) {
    u = fPixelsInCluster[i][0];
    v = fPixelsInCluster[i][1];
    x = fPixelsCoordInCluster[i][0];
    y = fPixelsCoordInCluster[i][1];
    XResid = x - fBaricenter2X;
    YResid = y - fBaricenter2Y;
    fMomentum2X += Float_t(SignalMatrix[u][v]*TMath::Power((double)(CosTh2*XResid + SinTh2*YResid), 2));
    fMomentum2Y += Float_t(SignalMatrix[u][v]*TMath::Power((double)(-SinTh2*XResid + CosTh2*YResid), 2));
  }
  if (fPulseHeight2 > 0.0) {
    fMomentum2X /= fPulseHeight2;
    fMomentum2Y /= fPulseHeight2;
  }
  // At the moment theta represents the angle of the cluster axis ( major OR minor ) which lies in the range
  // -kPI/4 -> kPI/4.  Now that we can distinguish major and minor axes ( from the momenta ), we can translate
  // theta to represent the angle of just the MAJOR axis ( ranging from -kPI/2 to kPI/2 ).
  Float_t Mom2X;
  if (fMomentum2X < fMomentum2Y) {
    Mom2X = fMomentum2X;
    fMomentum2X = fMomentum2Y;
    fMomentum2Y = Mom2X;
    if (fTheta2 > 0.0) fTheta2 -= 0.5*kPI;
    else fTheta2 += 0.5*kPI;
  }
  if (TMath::Abs(fTheta2 - fTheta) > kPI/2.0)
    {
      if (fTheta2 > 0) fTheta2 -= kPI;
      else fTheta2 += kPI;
    }
}

Float_t TCluster::PixelWeight(Float_t Distance, Float_t Shape, Float_t fThirdMomentumX, Float_t WeightLengthScale){
  Float_t Weight;
  Float_t ds = 2;
  Float_t dm = 0.001;
  Float_t kWeight2 = 0.05;//0.070;
  //Weight = pow((1/(kWeightLengthScale + Distance)), kWeightIndex);
  //Set the Weight according to the shape and charge asimmetry of the cluster
  if (Shape <(1+ds) || (fThirdMomentumX > -dm && fThirdMomentumX < dm)) Weight = exp(-Distance/kWeight2);
  else Weight = exp(-Distance/WeightLengthScale);
  return Weight; 
}










