// KMotionDLL_Direct   this class makes direct calls (using the current process)
//                     to the USB Driver
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/




enum 
{ 
	ENUM_WriteLineReadLine,
	ENUM_WriteLine,
	ENUM_WriteLineWithEcho,
	ENUM_ReadLineTimeOut,
	ENUM_ListLocations,
	ENUM_Failed,
	ENUM_Disconnect,
	ENUM_FirmwareVersion,
	ENUM_CheckForReady,
	ENUM_KMotionLock,
	ENUM_USBLocation,
	ENUM_KMotionLockRecovery,
	ENUM_ReleaseToken,
	ENUM_ServiceConsole,
	ENUM_SetConsole,
};

enum 
{ 
	DEST_NORMAL,
	DEST_CONSOLE,
	DEST_ERRMSG,
};


// This class is exported from the KMotionDLL_Direct.dll
class KMOTIONDLL_API CKMotionDLL_Direct {
public:
	CKMotionDLL_Direct(void);

	int MapBoardToIndex(int board);
	int WriteLineReadLine(int board, const char *s, char *response);
	int WriteLine(int board, const char *s);
	int WriteLineWithEcho(int board, const char *s);
	int ReadLineTimeOut(int board,char *buf, int TimeOutms);
	int ListLocations(int *nlocations, int *list);
	void WaitToken(int board);
	int KMotionLock(int board);
	int USBLocation(int board);
	int KMotionLockRecovery(int board);
	void ReleaseToken(int board);
	int Failed(int board);
	int Disconnect(int board);
	int FirmwareVersion(int board);
	int CheckForReady(int board);
	int ServiceConsole(int board);
	const char *GetErrMsg(int board);
	void ClearErrMsg(int board);

	int SetConsoleCallback(int board, SERVER_CONSOLE_HANDLER *ch);

	int nInstances();

};

