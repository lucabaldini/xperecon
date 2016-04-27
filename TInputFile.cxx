#include "TInputFile.h"

//ClassImp(TTInputFile)

/************************************************************************
 * TInputFile::TInputFile - Constructor: open file.                     *
 ************************************************************************/
TInputFile::TInputFile( Text_t *fName, UShort_t fType) {
 
    fFileName = fName;  // Initilise class data member from parameter.
    // Method of opening file depends on file type (Ascii or binary).
    if ( fType == kAscii ) fStream.open( fFileName, ios::in );
    else fStream.open( fFileName, ios::in | ios::binary );
    // Check if there were problems with the open.
    fErr = fStream.bad();
    if ( fErr ) {
	cout << "Could not open file " << fFileName << " Exit " << fErr << endl;
	exit ( fErr );
    }
    //    cout << "\nOpened file " << fFileName << endl;
}

/************************************************************************
 * TInputFile::~TInputFile - Destructor: close file.                    *
 ************************************************************************/
TInputFile::~TInputFile() {
    
    fStream.close();
    // Check if there were problems with the close.
    fErr = fStream.bad();
    if ( fErr ) {
	cout << "Problem closing file " << fFileName << " Exit " << fErr << endl;
	exit ( fErr );
    }
    cout << "\nClosed file " << fFileName << endl;
}

/************************************************************************
 * TInputFile::CheckRead - Check all data read correctly.               *
 ************************************************************************/
void TInputFile::CheckRead( ULong_t fExpected ) 
{
  // fExpected is the number of bytes we expected to read.  This is compared with
  // the actual number read, given by the gcount() method.
  if ( (ULong_t)fStream.gcount() != fExpected ) 
    {
      cout << "Could not read all of data from file " << fFileName <<  " Exit" << endl;
      exit ( 1 );
    }
}

