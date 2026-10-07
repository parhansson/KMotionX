"..\..\DSP_KFLOP\c6000_7.4.24\bin\cl6x" -k -q -al -as -i"..\..\DSP_KFLOP" -mu -ml3 -mv6710 -o0 "MainFile1.c"
"..\..\DSP_KFLOP\c6000_7.4.24\bin\cl6x" -k -q -al -as -i"..\..\DSP_KFLOP" -mu -ml3 -mv6710 -o0 "File2.c"
"..\..\DSP_KFLOP\c6000_7.4.24\bin\cl6x" --diag_suppress=10063 -@"LinkMainFile1File2.lkf" --output_file=MainFile1(2).out
pause

