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
int    STARTEV, STOPEV;
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
    TEventAnalysis eventAnalysis(VLEVEL, argv[0], DFNAME, STARTEV, STOPEV);
  }

  else {
    /* Interactive Pixy */
    TRint theApp( "Pixy", &argc, argv );
    theGUI = new TMainGUI( gClient->GetRoot(), 520, 320, VLEVEL, argv[0] );
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
  opt->addUsage( "  firstEv [default: 0]    Start from this event in file" );
  opt->addUsage( "  lastEv  [default: ALL]  Max n. of event to be processed" );
  opt->addUsage( "" );
  opt->addUsage( "EXAMPLE: ./Pixy -b filelist.txt           Process all events in file list" );
  opt->addUsage( "EXAMPLE: ./Pixy -b filelist.txt 0 100     Process the first 100 events in file list" );
  opt->addUsage( "EXAMPLE: ./Pixy -b filelist.txt 100 100   Process only the 100th event in file list" );

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
    
  case 3: 
    STOPEV  = atoi(opt->getArgv(2));          // get n. of last event
    STARTEV = atoi(opt->getArgv(1));          // get n. of first event
    DFNAME  = opt->getArgv(0);                // get data files list name
    break;

  case 2: 
    STOPEV  = -999;
    STARTEV = atoi(opt->getArgv(1));          // get n. of first event
    DFNAME  = opt->getArgv(0);                // get data files list name
    break;
    
  case 1: 
    STOPEV  = -999;
    STARTEV = 0;
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
      cerr << "[" << argv[0] << " - WARNING] No arguments specified. Check settings in config files." << endl;
      return OK_STAT;
    }

    break;
  }

  delete opt;
  return OK_STAT;

}
