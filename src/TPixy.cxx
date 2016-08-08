#include "anyoption.h"
#include "TPixy.h"
#include "TEventAnalysis.h"
#include "TMainGUI.h"

enum STATUS {
  HLP_STAT = 0x02,
  OK_STAT  = 0x01,
  BAD_STAT = 0x00
};

int    VLEVEL;
int    NEVTS;
char*  DFNAME;
bool   ISBATCH;

TMainGUI *theGUI;

int GetUserOpt(int, char**);



int main(int argc, char **argv) 
{


  VLEVEL  = 0;
  ISBATCH = false;

  /* Get runtime options and args */
  int _status = 0; 
  _status = GetUserOpt(argc, argv);              
  if( _status != OK_STAT ) { 
    exit(EXIT_FAILURE);
  }

  if (ISBATCH) {
    /* Run Pixy in batch mode */ 
    gROOT->SetBatch();      
    TEventAnalysis eventAnalysis(VLEVEL, argv[0], DFNAME, NEVTS);
  }

  else {
    /* Interactive Pixy */
    TRint theApp( "Pixy", &argc, argv );
    theGUI = new TMainGUI( gClient->GetRoot(), 520, 320 );
    theApp.Run();
    gSystem->ProcessEvents();
    return (0);
  }
}



int GetUserOpt( int argc, char* argv[] )
{
  
  AnyOption *opt = new AnyOption();
  
  //opt->setVerbose();                          // print warnings about unknown options
  //opt->autoUsagePrint(false);                 // print usage for bad options
  
  opt->addUsage( "USAGE: ./Pixy [Options] [Arguments]" );
  opt->addUsage( "" );
  opt->addUsage( "Options: " );
  opt->addUsage( "  -h, --help              Print this help " );
  opt->addUsage( "  -v, --verbose <vlevel>  Change verbosity level" );
  opt->addUsage( "  -b, --batch-mode        Run in batch mode" );
  opt->addUsage( "" );
  opt->addUsage( "Arguments: " );
  opt->addUsage( "  dataFileList            Data files list" );
  opt->addUsage( "  nEvents [default: ALL]  Max n. of event to be processed" );
  opt->addUsage( "" );
  opt->addUsage( "EXAMPLE: ./Pixy -b filelist.txt" );

  opt->setFlag( "help", 'h' );  
  opt->setOption( "verbose", 'v' );
  opt->setFlag( "batch-mode", 'b' );

  opt->processCommandArgs( argc, argv );      // go through the command line and get the options 
  
  if ( ! opt->hasOptions() ) {                // default action if no options 
    cerr << "[" << argv[0] << " - WARNING] No options specified. Using defaults ... Type '" 
	   << argv[0] << " --help' for usage." << endl;
  }
  
  if ( opt->getFlag( "help" ) ){              // get the options
    opt->printUsage();
    return HLP_STAT;
  }
  
  if ( opt->getFlag( "batch-mode" ) ) {
    ISBATCH = true;
    cerr << "[" << argv[0] << " - MESSAGE] Running in batch mode" << endl;
  }

  if ( opt->getValue( "verbose" ) != NULL ) {
    VLEVEL = atoi( opt->getValue( "verbose" ) ); 
  }
   
  switch ( opt->getArgc() ) {                 // get the actual arguments after the options
    
  case 2: 
    NEVTS   = atoi(opt->getArgv(1));          // get n. of events
    DFNAME  = opt->getArgv(0);                // get data files list name
    break;
    
  case 1: 
    NEVTS  = -999;
    DFNAME  = opt->getArgv(0);                // get data file name
    break;
    
  default:
    delete opt;
    if (ISBATCH) {
      cerr << "[" << argv[0] << " -   ERROR] No arguments specified. Type '" 
	   << argv[0] << " --help' for usage." << endl;
      return BAD_STAT;
    }
    else {
      cerr << "[" << argv[0] << " - WARNING] No arguments specified. Using defaults." << endl;
      return OK_STAT;
    }

    break;
  }

  delete opt;
  return OK_STAT;

}
