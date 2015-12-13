"C:\CCStudio_v3.1\C6000\cgtools\bin\cl6x" -k -q -al -as -i"..\..\DSP_KFLOP" -mu -ml3 -mv6710 -o3 "BlinkFast.c"
"C:\CCStudio_v3.1\C6000\cgtools\bin\cl6x" -@"Thread2.lkf"
del "BlinkFast(2).out"
rename BlinkFast.out BlinkFast(2).out
