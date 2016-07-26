#ifndef THEXAGONCOL_HH
#define THEXAGONCOL_HH

#include "TPixy.h"

struct PixelHit {
  Float_t X; 
  Float_t Y;
  Float_t Height;
};


class THexagonCol {


 private:


 public:

  Float_t HitX, HitY, Size;
  Float_t x[ 7 ];
  Float_t y[ 7 ];
  THexagonCol(PixelHit);
  //~THexagonCol();
  void Draw(Color_t);
  void DrawEmpty(Color_t);
};

#endif
