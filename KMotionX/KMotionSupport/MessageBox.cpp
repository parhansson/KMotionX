#include <iostream>
#include "MessageBox.h"

int MessageBoxConsoleHandler(const char *title, const char *msg, int options);

MB_USER_CALLBACK *mb_callback = MessageBoxConsoleHandler;

int AfxMessageBox(const char* value, int type){

	return mb_callback("AfxMessageBox",value,type);
}

int MessageBox(int whatisthis,const char* value,const char* title, int type){
	return mb_callback("AfxMessageBox",value,type);
}

//Default Console handler for messageboxes
int MessageBoxConsoleHandler(const char *title, const char *msg, int options){
		char str[100];
		printf("---------%s:  OPTION:  %s---------\n%d\n-----------------------------------------------\n",
		    title,msg,options);
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