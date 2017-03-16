/*
 * Functions.cpp
 *
 *  Created on: 19 sep 2014
 *      Author: parhansson
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "KMotionX.h"

#define SECONDS_PER_MONTH 2629743



uint32_t GetTickCount()
{
  struct timeval tv;
  if (gettimeofday(&tv, NULL) != 0)
    return 0;

  //casting to smaller type should wrap around in 49.71 days
  return (uint32_t)(tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

char *_strupr(char *s)
{
  if (s == (void *)0)
    return s;
  char *p = s;
  while ((*p = toupper(*p)))
    p++;
  return s;
}

char *_strlwr(char *s)
{
  if (s == (void *)0)
    return s;
  char *p = s;
  while ((*p = tolower(*p)))
    p++;
  return s;
}

uint32_t timeGetTime()
{
  //TODO Use uptime and the same in GetTickCount
  //http://stackoverflow.com/questions/3070278/uptime-under-linux-in-c

  //this is an ugly beast. cut from windows API doc

  //The timeGetTime function retrieves the system time, in milliseconds.
  //The system time is the time elapsed since Windows was started.

  //Note that the value returned by the timeGetTime function is a DWORD value.
  //The return value wraps around to 0 every 2^32 milliseconds, which is about 49.71 days.
  //This can cause problems in code that directly uses the timeGetTime return value in computations,
  //particularly where the value is used to control code execution.
  //You should always use the difference between two timeGetTime return values in computations.

  //We normalize to current month which is about 30 days and will fit in unsigned int.
  struct timeval now;
  gettimeofday(&now, NULL);
  uint64_t epoch = (now.tv_sec);
  uint32_t monthSinceEpoch = epoch / SECONDS_PER_MONTH;
  uint32_t sThisMonth = epoch - (SECONDS_PER_MONTH * monthSinceEpoch);
  uint32_t msThisMonth = sThisMonth * 1000 + now.tv_usec / 1000;
  //printf("Timestamp: monthSinceEpoch=%d sThisMonth=%d msThisMonth=%d\n", monthSinceEpoch,sThisMonth, msThisMonth);

  return msThisMonth;
}

namespace kmx
{
	char customCompiler[256] = KMX_COMPILER;
	char customOptions[256] ="-g";
	int tcc_vers = OLD_COMPILER ? 16 : 26;
	char installPath[MAX_PATH] = {0};
	char binPath[MAX_PATH] = {0};

	int verifyInstallRoot(const char * rootpath);
	int verifySubpath(const char * path, const char * subpath, bool isdir);

	const char * getInstallPath(){
				char cwd[MAX_PATH];
		getcwd(cwd,MAX_PATH);
		
		if(!installPath[0]){
			//To run on git location for debugging use environment vars
			//check ENV KMOTIONX_HOME this overrides default install location			
			char * envPath;
			if((envPath = getenv("KMOTIONX_HOME")) != NULL){
				strcpy(installPath, envPath);
			}
			else {
				//No KMOTIONX_HOME environment variable set.
				//Check in $HOME/.kmotionx
				if((envPath = getenv("HOME")) != NULL){
					sprintf(installPath,"%s/.kmotionx",envPath);
				} else {
					//Is HOME not ever set?
					//homedir = getpwuid(getuid())->pw_dir;
				}
			} 
			
			if(verifyInstallRoot(installPath)){
				log_info("No valid KMOTIONX_HOME found in %s\n Please set KMOTIONX_HOME or run make install again", installPath);
				exit(1);
			} else {
				log_info("Using KMOTIONX_HOME=%s",installPath);
			}


		}
		return installPath;
	}

	const char * getBinPath(){
		if(!binPath[0]){
			sprintf(binPath, "%s/bin", getInstallPath());
		}
		return binPath;
	}

	int LaunchServer(){
		char command[1024];
#ifdef _DEAMON
    sprintf(command, "%s/%s", getBinPath(),"KMotionServer");
#else
    sprintf(command, "%s/%s", getBinPath(),"KMotionServer &");
#endif

#if defined(__APPLE__) && defined(_DEAMON)
		//The daemon is currently not supported on MacOs
		log_info("Launch KMotionServer first: %s",command);
		PipeMutex->Unlock();
		exit(1);
#endif
    log_info("Launching KMotionServer: '%s'", command);
    return system(command);

	}

	int getDspFile(char * OutFile, bool KFLOP_board){
		if(KFLOP_board){
			sprintf(OutFile,"%s%cDSP_KFLOP%cDSPKFLOP.out",getInstallPath(),PATH_SEPARATOR,PATH_SEPARATOR);
		} else {
			sprintf(OutFile,"%s%cDSP_KMotion%cDSPKMotion.out",getInstallPath(),PATH_SEPARATOR,PATH_SEPARATOR);
		}
		return 0;
	}

	int getCompiler(char * Compiler, int MaxCompilerLen){
		//char Compiler[MAX_PATH + 1];
		strncpy(Compiler, customCompiler,MaxCompilerLen);
		if (Compiler[0] == '/')
		{
			FILE *f = fopen(Compiler, "r"); // try if compiler is on path
			if (f == NULL)
			{
				snprintf(Compiler,MaxCompilerLen,"Error Locating custom compiler (given absolute path) %s",customCompiler);
				return 1;
			}
		}
		else
		{
			FILE *f = fopen(Compiler, "r"); // try if compiler is on path

			if (f == NULL)
			{
				sprintf(Compiler, "%s/%s", getBinPath(), customCompiler);
				f = fopen(Compiler, "r"); // try in the released directory next
				if (f == NULL)
				{
					if (tcc_vers < 26)
						sprintf(Compiler, "%s/TCC67/%s", getInstallPath(), customCompiler);
					else
						sprintf(Compiler, "%s/KMotionX/tcc-0.9.26/%s", getInstallPath(), customCompiler);
					f = fopen(Compiler, "r"); // try in the released directory next
					if (f == NULL)
					{
						snprintf(Compiler,MaxCompilerLen,"Error Locating c67-tcc Compiler %s",customCompiler);
						return 1;
					}
				}
			}
			fclose(f);
		}
		
		return 0;
	}

	void SetCustomCompiler(const char * compiler, const char * options, int tcc_minor_version)
	{
			if (compiler)
					strncpy(customCompiler, compiler, sizeof(customCompiler));
			else
					strcpy(customCompiler, KMX_COMPILER);
			if (options)
					strncpy(customOptions, options, sizeof(customOptions));
			else
					customOptions[0] = 0;
			if (tcc_minor_version)
					tcc_vers = tcc_minor_version;
	}

	int getCompileCommand(const char * Name, const char * OutFile, uint32_t LoadAddress, bool KFLOP_board, char * command, int cmd_len){
		char Compiler[MAX_PATH +1];
		if(getCompiler(Compiler, sizeof(Compiler))){
			strncpy(command,Compiler,cmd_len);
			return 1;
		}

		char IncSrcPath1[MAX_PATH +1];
		char IncSrcPath2[MAX_PATH +1];
		char BindTo[MAX_PATH +1];

		//Get path to DSPKMotion.out or DSPKFLOP.out
		getDspFile(BindTo, KFLOP_board);

		getPath(BindTo,IncSrcPath1);

		getPath(Name,IncSrcPath2);

    if (tcc_vers < 26)
 	    snprintf(command, cmd_len, "%s -text %08X %s -nostdinc -I\"%s\" -I\"%s\" -o \"%s\" \"%s\" \"%s\" 2>&1",
			Compiler,
			LoadAddress,
			customOptions,
			IncSrcPath1,
			IncSrcPath2,
			OutFile,
			Name,
			BindTo);
    else
	    snprintf(command, cmd_len, "%s -Wl,-Ttext,%08X %s -Wl,--oformat,coff -static -nostdinc -nostdlib -I\"%s\" -I\"%s\" -o \"%s\" \"%s\" \"%s\" 2>&1",
			Compiler,
			LoadAddress,
			customOptions,
			IncSrcPath1,
			IncSrcPath2,
			OutFile,
			Name,
			BindTo);

	//Original TCC67 Windows version shipped with KMotion
	//tcc -text 80050000 -g -nostdinc -I./DSP_KFLOP -I./ -o Gecko3Axis.out Gecko3Axis.c ./DSP_KFLOP/DSP_KFLOP.out
	// -text replaced by -Wl,-Ttext,address
	//http://manpages.ubuntu.com/manpages/lucid/man1/tcc.1.html

	//compile with debug flag is currently not supported -g
	//c67-tcc -Wl,-Ttext,80050000 -Wl,--oformat,coff -static -nostdinc -nostdlib -I./ -o ~/Desktop/Gecko3AxisOSX.out Gecko3Axis.c DSPKFLOP.out
		return 0;
	}


	int verifyInstallRoot(const char * root){
		int error = verifySubpath(root, "bin/", true) |
		verifySubpath(root, "bin/tcc67", false) |
		verifySubpath(root, "bin/KMotionServer", false) |
		verifySubpath(root, "GCode Programs/", true) |
		verifySubpath(root, "GCode Programs/emc.var", false) |
		//verifySubpath(root, "GCode Programs/Default.set", false) |
		//verifySubpath(root, "GCode Programs/Default.tbl", false) |
		verifySubpath(root, "DSP_KFLOP/", true) |
		verifySubpath(root, "DSP_KFLOP/DSPKFLOP.out", false) |
		verifySubpath(root, "DSP_KFLOP/KMotionDef.h", false) |
		verifySubpath(root, "DSP_KFLOP/PC-DSP.h", false) |
		//verifySubpath(root, "DSP_KFLOP/PC2.c", false) |
		verifySubpath(root, "DSP_KMotion/", true) |
		verifySubpath(root, "DSP_KMotion/DSPKMotion.out", false) |
		verifySubpath(root, "DSP_KMotion/KMotionDef.h", false) |
		verifySubpath(root, "DSP_KMotion/PC-DSP.h", false);
		//verifySubpath(root, "DSP_KMotion/PC2.c", false) |
		return error;
	}

	int verifySubpath(const char * path, const char * subpath, bool isdir){
		char actualpath[MAX_PATH];
		if(path[strlen(path) - 1] == '/'){
			sprintf(actualpath,"%s%s", path,subpath);
		} else {
			sprintf(actualpath,"%s/%s", path,subpath);
		}
    // file exists and read user has read permissions
		int result = access( actualpath, F_OK | R_OK);
		
		if(result){
			if(isdir){
				printf("Directory not found %s\n", actualpath);
			} else {
				printf("File not found %s\n", actualpath);
			}
		}
		return result;
	}

	void getPath(const char * file, char * path){
		const char *pch;
		pch=strrchr(file,PATH_SEPARATOR);
		strcpy(path,file);
		//null terminate string at last slash position, unless no path,
		// in which case make it a ".".
		if (!pch)
				strcpy(path, ".");
		else
				path[pch-file] ='\0';
	}

	//currently not used
	char *fix_slashes(char *path)
	{
		char *p;
		for (p = path; *p; ++p)
			if (*p == '\\')
				*p = '/';
		return path;
	}
}