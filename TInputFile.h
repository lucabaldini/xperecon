#ifndef TINPUTFILE_HH
#define TINPUTFILE_HH

#include "TPixy.h"

class TInputFile {
    
private:
    Text_t *fFileName;   // Name of file.
    Int_t fErr;          // Error code.
 
public:
    ifstream fStream;    // Input stream to which file is linked.
    TInputFile(Text_t*, UShort_t);
    ~TInputFile();
    void CheckRead(ULong_t);

    //ClassDef (TInputFile, 1)
};

#endif
