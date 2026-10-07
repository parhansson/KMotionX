#include "KMotionDef.h"

void memcpy_from_user_flash(void *dest, int byte_offset_in_user_flash, int num_bytes);

void main()
{
	int nBytes;

	iOpenBuf();					// initialize Coordinated Motion Buffer

	memcpy_from_user_flash(&nBytes, 0, 4);	// read the number of bytes of data from 1st word of Flash
	memcpy_from_user_flash(&CS0_TimeDownloaded, 4, 8);	// read the Time in buffer
	memcpy_from_user_flash(ParametricCoeffs, 12, nBytes);	// read the data
	printf("Reading Coord Motion Buffer from Flash %d bytes, Time %.3f\n", nBytes,
		   CS0_TimeDownloaded);
	
	iExecBuf();					// Launch execution
	CS0_Flushed = TRUE;			// allow to finish
}

// copies memory and sets Flash Bank as required
void memcpy_from_user_flash(void *dest, int byte_offset_in_user_flash, int num_bytes)
{
	const char *f = (const char *)FLASH_USER + byte_offset_in_user_flash;
	char *m = (char *)dest;
	int BytesToDo;

	while (num_bytes > 0)
	{
		SetFlashBank(f);
		BytesToDo = 0x4000 - (((int)f) & 0x3FFF);  // how much left before bank switch required?
		if (BytesToDo > num_bytes) BytesToDo = num_bytes;  // limit to how many left
		memcpy(m,f,BytesToDo);  // move it
		m+=BytesToDo;
		f+=BytesToDo;
		num_bytes-=BytesToDo;
	}
}
