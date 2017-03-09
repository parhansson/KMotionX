/*
 * Functions.cpp
 *
 *  Created on: 19 sep 2014
 *      Author: parhansson
 */
#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "KMotionX.h"

#define SECONDS_PER_MONTH 2629743

//currently not used
char *fix_slashes(char *path)
{
  char *p;
  for (p = path; *p; ++p)
    if (*p == '\\')
      *p = '/';
  return path;
}

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

	const char * getInstallPath(){
		if(!installPath[0]){

			char* kmotionRoot;
			char rootPath[MAX_PATH];
			char resolved_root[MAX_PATH];

			if(!(kmotionRoot = getenv("KMOTION_ROOT"))){
					//No KMOTION_ROOT environment variable set.
					//Assume user is in KMotion/bin directory
						sprintf(rootPath,"%s/..",getenv("PWD"));
						kmotionRoot = realpath(rootPath, resolved_root);
						//printf("%s:%d Environment KMOTION_ROOT resolved to %s\n",__FILE__,__LINE__,kmotionRoot);

						//Save resolved root, used by CoordMotion
						//This results in Segmentation faults when calling getenv later on
						//setenv("KMOTION_ROOT", kmotionRoot,0);
			}
			sprintf(installPath, "%s", kmotionRoot);

		}
		return installPath;
	}

	const char * getBinPath(){
		if(!binPath[0]){
			char* kmotionBin;
			char rootPath[MAX_PATH];
			char resolved_root[MAX_PATH];
			if(!(kmotionBin = getenv("KMOTION_BIN"))){
					//No KMOTION_ROOT environment variable set.
					//Assume user is in KMotion/bin directory
						sprintf(rootPath,"%s",getenv("PWD"));
						kmotionBin = realpath(rootPath, resolved_root);
						//printf("%s:%d Environment KMOTION_BIN resolved to %s\n",__FILE__,__LINE__,kmotionBin);
			}
			sprintf(binPath, "%s", kmotionBin);
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
		log_info("Launch KMotionServer first: %s\n",command);
		PipeMutex->Unlock();
		exit(1);
#endif
    log_info("Launching KMotionServer: '%s'\n", command);
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
}