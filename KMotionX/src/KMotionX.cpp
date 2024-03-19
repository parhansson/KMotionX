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
#include <sys/stat.h>
#include <sys/syscall.h>
#include <locale>
#include <codecvt>
#include <string>
#include "KMotionX.h"

#define SECONDS_PER_MONTH 2629743

uint32_t GetTickCount()
{
	struct timeval tv;
	if (gettimeofday(&tv, NULL) != 0)
		return 0;

	// casting to smaller type should wrap around in 49.71 days
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
	// TODO Use uptime and the same in GetTickCount
	// http://stackoverflow.com/questions/3070278/uptime-under-linux-in-c

	// this is an ugly beast. cut from windows API doc

	// The timeGetTime function retrieves the system time, in milliseconds.
	// The system time is the time elapsed since Windows was started.

	// Note that the value returned by the timeGetTime function is a DWORD value.
	// The return value wraps around to 0 every 2^32 milliseconds, which is about 49.71 days.
	// This can cause problems in code that directly uses the timeGetTime return value in computations,
	// particularly where the value is used to control code execution.
	// You should always use the difference between two timeGetTime return values in computations.

	// We normalize to current month which is about 30 days and will fit in unsigned int.
	struct timeval now;
	gettimeofday(&now, NULL);
	uint64_t epoch = (now.tv_sec);
	uint32_t monthSinceEpoch = epoch / SECONDS_PER_MONTH;
	uint32_t sThisMonth = epoch - (SECONDS_PER_MONTH * monthSinceEpoch);
	uint32_t msThisMonth = sThisMonth * 1000 + now.tv_usec / 1000;
	// printf("Timestamp: monthSinceEpoch=%d sThisMonth=%d msThisMonth=%d\n", monthSinceEpoch,sThisMonth, msThisMonth);

	return msThisMonth;
}
namespace kmx
{
	char customCompiler[256] = KMX_COMPILER;
	char customOptions[256] = "-g";
	int tcc_vers = OLD_COMPILER ? 16 : 26;
	char installPath[MAX_PATH] = {0};
	char binPath[MAX_PATH] = {0};
	char localLanguageFilePath[MAX_PATH] = {0};

	int verifyInstallRoot(const char *rootpath);
	int verifySubpath(const char *path, const char *subpath, bool isdir);
	int testExecuteAccess(const char *file);

	// Convert std::wstring to std::string
	std::string wstrtostr(std::wstring wideString)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		std::string narrowString = converter.to_bytes(wideString);
		return narrowString;
	}
	// Convert std::string to std::wstring
	std::wstring strtowstr(std::string narrowString)
	{

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wideString = converter.from_bytes(narrowString);
		return wideString;
	}
	uint8_t randomInt()
	{
		uint8_t random_number;

		// Seed the random number generator withƒ the current time
		srand(time(NULL));

		// Generate a random number between 1 and 128
		random_number = rand() % 127 + 1;
		return random_number;
	}
	const char *getInstallPath()
	{
		char cwd[MAX_PATH];
		getcwd(cwd, MAX_PATH);

		if (!installPath[0])
		{
			// To run on git location for debugging use environment vars
			// check ENV KMOTIONX_HOME this overrides default install location
			char *envPath;
			if ((envPath = getenv("KMOTIONX_HOME")) != NULL)
			{
				strcpy(installPath, envPath);
			}
			else
			{
				// No KMOTIONX_HOME environment variable set.
				// Check in $HOME/.kmotionx
				if ((envPath = getenv("HOME")) != NULL)
				{
					snprintf(installPath, MAX_PATH, "%s/.kmotionx", envPath);
				}
				else
				{
					// Is HOME not ever set?
					// homedir = getpwuid(getuid())->pw_dir;
				}
			}

			if (verifyInstallRoot(installPath))
			{
				log_info("No valid KMOTIONX_HOME found in %s\n Please set KMOTIONX_HOME or run make install again", installPath);
				exit(1);
			}
			else
			{
				log_info("Using KMOTIONX_HOME=%s", installPath);
			}
		}
		return installPath;
	}

	const char *getLocalLanguageFilePath()
	{
		if (!localLanguageFilePath[0])
		{
			snprintf(localLanguageFilePath, MAX_PATH, "%s/Data/LocalLanguage.txt", getInstallPath());
		}
		return localLanguageFilePath;
	}

	const char *getBinPath()
	{
		if (!binPath[0])
		{
			snprintf(binPath, MAX_PATH, "%s/bin", getInstallPath());
		}
		return binPath;
	}

	int LaunchServer()
	{
		char command[1024];
#ifdef _DEAMON
		snprintf(command, MAX_PATH, "%s/%s", getBinPath(), "KMotionServer");
#else
		snprintf(command, MAX_PATH, "%s/%s", getBinPath(), "KMotionServer &");
#endif

#if defined(__APPLE__) && defined(_DEAMON)
		// The daemon is currently not supported on MacOs
		log_info("Launch KMotionServer first: %s", command);
		PipeMutex->Unlock();
		exit(1);
#endif
		log_info("Launching KMotionServer: '%s'", command);
		return system(command);
	}

	int getDspFile(char *OutFile, const int BoardType)
	{
		if (BoardType == BOARD_TYPE_KOGNA)
		{
			snprintf(OutFile, MAX_PATH, "%s%cDSPKOGNA%cDSPKOGNA.out", getInstallPath(), PATH_SEPARATOR, PATH_SEPARATOR);
		}
		else
		{
			snprintf(OutFile, MAX_PATH, "%s%cDSP_KFLOP%cDSPKFLOP.out", getInstallPath(), PATH_SEPARATOR, PATH_SEPARATOR);
		}
		return 0;
	}

	int getCompiler(char *Compiler, int MaxCompilerLen)
	{
		// char Compiler[MAX_PATH + 1];
		strncpy(Compiler, customCompiler, MaxCompilerLen);
		if (Compiler[0] == '/')
		{
			// try if compiler is accessible on absolute path
			if (testExecuteAccess(Compiler) == 0)
				return 0;
		}
		else
		{
			// try in the released directory next
			snprintf(Compiler, MAX_PATH, "%s/%s", getBinPath(), customCompiler);
			if (testExecuteAccess(Compiler) == 0)
				return 0;

			// this is for development only
			snprintf(Compiler, MAX_PATH, "%s/TCC67/%s", getInstallPath(), customCompiler);
			if (testExecuteAccess(Compiler) == 0)
				return 0;

			if (Compiler[0] != '.')
			{ // check if compiler is present in current dir
				snprintf(Compiler, MAX_PATH, "./%s", customCompiler);
				if (testExecuteAccess(Compiler) == 0)
					return 0;
			}
		}
		snprintf(Compiler, MaxCompilerLen, "Error Locating Compiler %s", customCompiler);
		return -1;
	}

	void SetCustomCompiler(const char *compiler, const char *options, int tcc_minor_version)
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

	int getCompileCommand(const char *Name, const char *OutFile, uint32_t LoadAddress, const int BoardType, char *command, int cmd_len)
	{
		char Compiler[MAX_PATH + 1];
		if (getCompiler(Compiler, sizeof(Compiler)))
		{
			strncpy(command, Compiler, cmd_len);
			return 1;
		}

		char IncSrcPath1[MAX_PATH + 1];
		char IncSrcPath2[MAX_PATH + 1];
		char BindTo[MAX_PATH + 1];

		// Get path to DSPKMotion.out or DSPKFLOP.out
		getDspFile(BindTo, BoardType);

		getPath(BindTo, IncSrcPath1);

		getPath(Name, IncSrcPath2);

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

		// Original TCC67 Windows version shipped with KMotion
		// tcc -text 80050000 -g -nostdinc -I./DSP_KFLOP -I./ -o Gecko3Axis.out Gecko3Axis.c ./DSP_KFLOP/DSP_KFLOP.out
		//  -text replaced by -Wl,-Ttext,address
		// http://manpages.ubuntu.com/manpages/lucid/man1/tcc.1.html

		// compile with debug flag is currently not supported -g
		// c67-tcc -Wl,-Ttext,80050000 -Wl,--oformat,coff -static -nostdinc -nostdlib -I./ -o ~/Desktop/Gecko3AxisOSX.out Gecko3Axis.c DSPKFLOP.out
		return 0;
	}

	int testExecuteAccess(const char *file)
	{
		debug("Testing file for existance execute permissions %s", file);
		// log_info("Checking access=%s",file);
		struct stat *sb;
		sb = (struct stat *)malloc(sizeof(struct stat));
		int result = -1;
		if (stat(file, sb) == 0)
		{
			// check that file is real file and is executable
			if (S_ISREG(sb->st_mode) && sb->st_mode & S_IXUSR)
			{
				result = 0;
			}
		}
		free(sb);
		return result;
	}

	int verifyInstallRoot(const char *root)
	{
		int error = verifySubpath(root, "bin/", true) |
					verifySubpath(root, "bin/tcc67", false) |
					verifySubpath(root, "bin/KMotionServer", false) |
					verifySubpath(root, "Data/", true) |
					verifySubpath(root, "Data/emc.var", false) |
					// verifySubpath(root, "GCode Programs/Default.set", false) |
					// verifySubpath(root, "GCode Programs/Default.tbl", false) |
					verifySubpath(root, "DSP_KFLOP/", true) |
					verifySubpath(root, "DSP_KFLOP/DSPKFLOP.out", false) |
					verifySubpath(root, "DSP_KFLOP/KMotionDef.h", false) |
					verifySubpath(root, "DSP_KFLOP/PC-DSP.h", false) |
					// verifySubpath(root, "DSP_KFLOP/PC2.c", false) |
					verifySubpath(root, "DSP_KOGNA/", true) |
					verifySubpath(root, "DSP_KOGNA/DSPKOGNA.out", false) |
					verifySubpath(root, "DSP_KOGNA/KMotionDef.h", false) |
					verifySubpath(root, "DSP_KOGNA/PC-DSP.h", false);
		// verifySubpath(root, "DSP_KOGNA/PC2.c", false) |
		return error;
	}

	int verifySubpath(const char *path, const char *subpath, bool isdir)
	{
		char actualpath[MAX_PATH];
		if (path[strlen(path) - 1] == '/')
		{
			snprintf(actualpath, MAX_PATH, "%s%s", path, subpath);
		}
		else
		{
			snprintf(actualpath, MAX_PATH, "%s/%s", path, subpath);
		}
		// file exists and read user has read permissions
		int result = access(actualpath, F_OK | R_OK);

		if (result)
		{
			if (isdir)
			{
				printf("Directory not found %s\n", actualpath);
			}
			else
			{
				printf("File not found %s\n", actualpath);
			}
		}
		return result;
	}

	void getPath(const char *file, char *path)
	{
		const char *pch;
		pch = strrchr(file, PATH_SEPARATOR);
		strcpy(path, file);
		// null terminate string at last slash position, unless no path,
		//  in which case make it a ".".
		if (!pch)
			strcpy(path, ".");
		else
			path[pch - file] = '\0';
	}

	// currently not used
	char *fix_slashes(char *path)
	{
		char *p;
		for (p = path; *p; ++p)
			if (*p == '\\')
				*p = '/';
		return path;
	}

	long int getThreadId(const char *callerId)
	{
		long int tid;
		// tid = syscall(SYS_gettid/*224*/);
#ifdef __APPLE__

		pthread_t t;
		t = pthread_self();
		// unsigned int
		mach_port_t mt;
		mt = pthread_mach_thread_np(t);

		// tid = t->__sig;
		tid = mt;
#else
		// assume linux. do syscall
		tid = syscall(SYS_gettid /*224*/);
		if (tid < 0)
		{
			// perror("syscall");
		}

		// pthread_id_np_t   tid;
		// tid = pthread_getthreadid_np();
		//---alternativeley--
		// pthread_t         self;
		// self = pthread_self();
		// pthread_getunique_np(&self, &tid);

#endif
		// if(callerId){
		// 		printf("%s wants to know thread id: %lu\n",callerId, tid);
		// } else {
		// 	printf("Thread id: %lu\n", tid);
		// }
		return tid;
	}

} // kmx namespace
