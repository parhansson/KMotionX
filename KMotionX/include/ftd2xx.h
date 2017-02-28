#ifndef FTD2XX_OS_INDIPENDENT_H
#define FTD2XX_OS_INDIPENDENT_H

#ifndef __cplusplus
    typedef enum {false = 0, true = 1} bool;
#endif  

    #ifdef _WINDOWS
        #include <windows.h>
        #include "ftdi/windows/FTD2XX.H"
	#elif __APPLE__
		#include "ftdi/macosx/x86_64/ftd2xx.h"
    #else
		#include "ftdi/linux/armhf/ftd2xx.h"
    #endif
#endif
