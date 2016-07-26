#include "anyoption.h"
#include "TPixy.h"
#include "TMainGUI.h"

enum STATUS {
  OK_STAT=0x01,
  BAD_STAT=0x00
};


int VLEVEL;
string DFNAME;
string CFNAME;
bool ISBATCH;

TMainGUI *theGUI;

int GetUserOpt(int, char**);


int main(int argc, char **argv) 
{

  VLEVEL = 0;
  ISBATCH = false;

  int _status=0; 
  _status=GetUserOpt(argc, argv);              
  if(_status!=OK_STAT){ 
    // if(_status == 0) 
    //   cerr<<"[e3Viewer.exe - ERROR] GetUserOpt returned error code "
    // 	  <<_status<<" - too few arguments. "<<endl;
    exit(EXIT_FAILURE);
  }

  if (ISBATCH) {
    gROOT->SetBatch();
  }

  else {

    TRint theApp("Pixy", &argc, argv);
    theGUI = new TMainGUI(gClient->GetRoot(), 520, 320);
    theApp.Run();
    gSystem->ProcessEvents();
    return (0);
  }
}



int GetUserOpt(int argc, char* argv[])
{
  
  AnyOption *opt = new AnyOption();
  
  //opt->setVerbose();                          // print warnings about unknown options
  //opt->autoUsagePrint(false);                 // print usage for bad options
  
  opt->addUsage( "Usage: Pixy [options] [arguments]" );
  opt->addUsage( "" );
  opt->addUsage( "Options: " );
  opt->addUsage( "  -h, --help              Print this help " );
  opt->addUsage( "  -v, --verbose <vlevel>  Change verbosity level" );
  opt->addUsage( "  -b, --batch-mode        Run in batch mode" );
  opt->addUsage( "" );
  opt->addUsage( "Arguments: " );
  opt->addUsage( "  fileName                Data file absolute path" );
  opt->addUsage( "  nEvents                 Number of events to be processed" );
  opt->addUsage( "" );
  //opt->addUsage( "Notes: ");
  //opt->addUsage( "" );

  opt->setFlag( "help", 'h' );  
  opt->setOption( "verbose", 'v' );
  opt->setFlag( "batch-mode", 'b' );

  opt->processCommandArgs( argc, argv );      // go through the command line and get the options 
  
  if( ! opt->hasOptions()) {                  // default action if no options 
    cerr<<"["<<argv[0]<<" - WARNING] No options specified. Using default values ..."<<endl;
    // cout<<"["<<argv[0]<<" - WARNING] No options specified. Type 'Pixy --help' for usage. Exiting ..."<<endl;
    // delete opt;
    // return BAD_STAT;
  }
  
  if( opt->getFlag( "help" ) ){               // get the options
    opt->printUsage();
    return 2;
  }
  
  //  if (gROOT->IsBatch()) {
  if( opt->getFlag( "batch-mode" ) ) {
    ISBATCH = true;
    cerr<<"["<<argv[0]<<" - ERROR] Cannot run in batch mode"<<endl; // this needs to be cured! :-)
    return 1;
  }

  if( opt->getValue( "verbose" ) != NULL ) {
    VLEVEL=atoi(opt->getValue( "verbose" )); 
  }
   
  switch (opt->getArgc()) {                   // get the actual arguments after the options
    
  case 2: 
    CFNAME  = opt->getArgv(1);                // get configuration file name
    DFNAME  = opt->getArgv(0);                // get data file name
    break;
    
  case 1: 
    DFNAME  = opt->getArgv(0);                // get data file name
    break;
    
  default:
    delete opt;
    cerr<<"["<<argv[0]<<" - WARNING] No arguments specified. Using default settings. Type '"<<argv[0]<<" --help' for usage."<<endl;
    return OK_STAT;
    break;
  }

  delete opt;
  return OK_STAT;

}
