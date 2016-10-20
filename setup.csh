#!/bin/csh
setenv CFITSIO /data1/glast/ground/GLAST_EXT/redhat6-x86_64-64bit-gcc44/cfitsio/v3370
if($?LD_LIBRARY_PATH) then
  setenv LD_LIBRARY_PATH $CFITSIO/lib:$LD_LIBRARY_PATH
else
  setenv LD_LIBRARY_PATH $CFITSIO/lib
endif
