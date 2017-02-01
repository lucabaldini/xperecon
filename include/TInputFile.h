#ifndef TINPUTFILE_HH
#define TINPUTFILE_HH

#include "TPixy.h"

class TInputFile {
    
private:
    TString fFileName;   // Name of file.
    Int_t fErr;          // Error code.
 
public:
    ifstream fStream;    // Input stream to which file is linked.
    TInputFile(TString, UShort_t);
    ~TInputFile();
    void CheckRead(ULong_t);
};
#endif
