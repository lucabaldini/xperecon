#ifndef TCLUSTER_HH
#define TCLUSTER_HH

#include "TPixy.h"

class TCluster {

 private:

  Int_t fClusterNumber;

 public:
  Float_t cluLenght;
  MatrixFloat_t fPGVMatrix;     // Matrix for "average cluster" plot.
  Int_t fHighestPix;            // Number of pixel with highest signal.

  Int_t fPixelsInCluster[MAXCLUSIZE][2];
  Float_t fPixelsCoordInCluster[MAXCLUSIZE][2];
  
  // Pulse height, noise and S/N of the cluster.
  Float_t fPulseHeight;
  Float_t fBorderCharge;        // Total charge on border pixels.
  Float_t fSignalToNoise;
  Float_t fTotalNoise;
  Float_t fHighestCharge;
  // Position of the baricenter of the cluster.
  Float_t fCenterOfGravityX;
  Float_t fCenterOfGravityY;
  Float_t fTheta;              // Angle of cluster main axis, using beam spot or barycentre method.
  Float_t fTheta2;             // Angle calculated by another method, e.g. using furthest pix.
  Float_t fThetaDiff;
  Float_t fMomentumX, fMomentumY;   // Principal momenta of cluster ( comments for fTheta apply ).
  Float_t fMom2X, fMom2Y;
  Float_t fThirdMomentumX, fThirdMomentumY;
  Float_t fFurthestPosX, fFurthestPosY;
  Float_t fImpactPosX, fImpactPosY;
  Int_t   fClusterSize;
  Float_t Shape;
  Int_t   impactFlag;
  Int_t   index0;
  Float_t HighestSig;   // Highest signal in cluster.

  TCluster(MatrixFloat_t, MatrixInt_t, MatrixInt_t, Int_t, Int_t, Int_t, Int_t);
  //~TCluster();   // Default destructor.

  void CalcProperties(MatrixFloat_t, MatrixFloat_t, UShort_t[PIX_R_DIM][PIX_C_DIM], MatrixFloat_t, MatrixFloat_t, Int_t, 
		      Float_t, Float_t, Float_t);
  void DrawProperties();
  Float_t PixelWeight(Float_t, Float_t, Float_t, Float_t);
  
};


#endif
























