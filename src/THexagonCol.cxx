#include "THexagonCol.h"
#include "TPolyLine.h"

THexagonCol::THexagonCol(PixelHit fHits ) {
  
  HitX = fHits.X;
  HitY = fHits.Y;
  Size = fHits.Height;

  // Find Hexagon vertex points
  x[0] = ( HitX );
  y[0] = ( HitY + Size);
  x[1] = ( HitX + Size*0.8660254 );
  y[1] = ( HitY + Size*0.5 );
  x[2] = ( HitX + Size*0.8660254 );
  y[2] = ( HitY - Size*0.5);
  x[3] = ( HitX );
  y[3] = ( HitY - Size);
  x[4] = ( HitX - Size*0.8660254 );
  y[4] = ( HitY - Size*0.5);
  x[5] = ( HitX - Size*0.8660254 );
  y[5] = ( HitY + Size*0.5 );
  x[6] = ( HitX );
  y[6] = ( HitY + Size );

}

void THexagonCol::Draw(Color_t color) {
  TPolyLine *hexagon = new TPolyLine (7,x,y);
  hexagon->SetFillColor(color);
  //hexagon->SetLineWidth(.1);
  hexagon->SetLineColor(color);
  hexagon-> Draw("f");
  hexagon-> Draw();
}

void THexagonCol::DrawEmpty(Color_t color) {
  TLine *line1, *line2, *line3, *line4, *line5, *line6;
  line1 = new TLine ( x[ 0 ], y[ 0 ], x[ 1 ], y[ 1 ] );
  line1->SetLineColor(color);
  line1 -> Draw();
  line2 = new TLine ( x[ 1 ], y[ 1 ], x[ 2 ], y[ 2 ] );
  line2->SetLineColor(color);
  line2 -> Draw();
  line3 = new TLine ( x[ 2 ], y[ 2 ], x[ 3 ], y[ 3 ] );
  line3->SetLineColor(color);
  line3 -> Draw();
  line4 = new TLine ( x[ 3 ], y[ 3 ], x[ 4 ], y[ 4 ] );
  line4->SetLineColor(color); 
  line4 -> Draw();
  line5 = new TLine ( x[ 4 ], y[ 4 ], x[ 5 ], y[ 5 ] );
  line5->SetLineColor(color);
  line5 -> Draw();
  line6 = new TLine ( x[ 5 ], y[ 5 ], x[ 0 ], y[ 0 ] );
  line6->SetLineColor(color);
  line6 -> Draw();
}
