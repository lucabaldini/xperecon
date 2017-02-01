
#ifndef TMAINGUI_HH
#define TMAINGUI_HH

#include "TPixy.h"
#include "TEventDisplay.h"
#include "TDetector.h"
#include "TCluster_hex.h"
#include "TEditor.h"
#include "TStopwatch.h"
#include "TEventAnalysis.h"

class TMainGUI: public TGMainFrame{
 
 private:
  Int_t            DebugLevel;
  char*            progName;
  TString          workingdir;
  ofstream         infofile;
  TGFileInfo       fi;

  TEventAnalysis   *tEventAnalysis;

  TEventDisplay    *EventDisplay;
  Editor           *ed;

  const TGPicture  *fLogo;
  TGImageMap       *fLogoImageMap;

  TGLabel          *fTitle, *fTitleDataEv, *fTitleDataEv1;
  TGLabel          *fTitleWeight, *fTitleSradius, *fTitleWradius;
  TGLabel          *fDataThresholdLabel;
 
  TGTextEntry      *fDataEvEntry, *fDataEvEntry1;
  TGTextEntry      *fWeightEntry, *fSradiusEntry, *fWradiusEntry;
  TGTextEntry      *fDataThresholdEntry;

  TGGroupFrame     *fDataFrame;
  TGCompositeFrame *fMainFrameVert, *fMainFrameHor;
  TGCompositeFrame *fRightFrame, *fLeftFrame;
  TGCompositeFrame *fDataThresholdFrame;
  TGCompositeFrame *fDataEvFrame, *fDataEvFrame1, *fWeightFrame, *fSradiusFrame, *fWradiusFrame;
  TGLayoutHints    *fMenuBarLayout, *fMenuBarItemLayout, *fMenuBarHelpLayout;	
  TGLayoutHints    *fButtonLayout, *fAnalysisLayout;
  TGButton         *fDisplayEventButton, *fAnalizeDataButton;
  TGCheckButton    *fThfix, *fHeadToFile;

  TGTextBuffer     *fEvData, *fEvData1;
  TGTextBuffer     *fW1, *fW2, *fW3;
  TGTextBuffer     *fThresholdBuf;
  Int_t            ThfixFlag, RawFlag;
  TString          W1, W2, W3, W4, W5;

  TGPopupMenu      *fPopupMenu, *fPopupHelp;
  TGMenuBar        *fMenuBar;
  TGMsgBox         *box;
  EMsgBoxIcon      icontype;

  Int_t            buttons, retval;
  Bool_t           HeaderOn;

 public:

  TMainGUI(const TGWindow *p, UInt_t w, UInt_t h, Int_t, char*);
  virtual  ~TMainGUI();
  void Init();
  inline void  SetProgParameters(Int_t VLEVEL, char* _name) {DebugLevel = VLEVEL; progName = _name;}
  inline void  SetWorkingdir() {workingdir = gSystem->WorkingDirectory();}
  virtual Bool_t ProcessMessage(Long_t msg, Long_t par1, Long_t);
  virtual void CloseWindow();
  void DataDisplay();
  void DataAnalysis();
  void DataSelect();
  void DataPanelDisable();
  void DataPanelEnable();
  ClassDef(TMainGUI,0);
};

#endif
