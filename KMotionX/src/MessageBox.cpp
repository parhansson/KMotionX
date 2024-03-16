#include <iostream>
#include "MessageBox.h"
#include "KMotionX.h"

int MessageBoxConsoleHandler(const wchar_t *title, const wchar_t *msg, int options);

MB_USER_CALLBACK *mb_callback = MessageBoxConsoleHandler;

int AfxMessageBox(const char* value, int type){

	return mb_callback(L"AfxMessageBox",kmx::strtowstr(value).c_str(),type);
}

int MessageBox(long hwnd,const char* value,const char* title, int type){
	return mb_callback(kmx::strtowstr(title).c_str(),kmx::strtowstr(value).c_str(),type);
}
int MessageBoxW(long hwnd,const wchar_t* value,const wchar_t* title, int type){
	return mb_callback(title,value,type);
	//return MessageBox(hwnd, kmx::wstrtostr(value).c_str(), kmx::wstrtostr(title).c_str(), type);
}
int MessageBoxW(long hwnd, std::wstring value,const wchar_t* title, int type){
	return mb_callback(title,value.c_str(),type);
	//return MessageBox(hwnd, kmx::wstrtostr(value).c_str(), kmx::wstrtostr(title).c_str(), type); 
}

//Default Console handler for messageboxes
int MessageBoxConsoleHandler(const wchar_t *title, const wchar_t *msg, int options){
		char str[100];
		printf("---------%ls:  (0x%.8X)  ---------\n"
				"%ls\n"
				"-----------------------------------------------\n",
		    title,options,msg);
		if((options & MB_OK) == MB_OK){
			printf(">OK?");
			fgets(str, 100, stdin);
		}else if((options & MB_YESNO) == MB_YESNO){
			printf(">(Yes/No)?");
			fgets(str, 100, stdin);
		}else if((options & MB_OKCANCEL) == MB_OKCANCEL){
			printf(">(Ok/Cancel)?");
			fgets(str, 100, stdin);
		}
	// testing for multiple flags
	// as above, OR the bitmasks
	//if ((flags & (LOG_INCOMING | LOG_OUTGOING))
	//         == (LOG_INCOMING | LOG_OUTGOING))

	return 0;
}