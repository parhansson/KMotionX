/*
 * Functions.cpp
 *
 *  Created on: 19 sep 2014
 *      Author: parhansson
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
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
#include <stdexcept>
#include "KMotionX.h"
#include <cstdio>	 // For vsnprintf
#include <cstdlib>	 // For malloc, free
#include <stdexcept> // For runtime_error
#include <cstdarg>	 // For va_list
#include <vector>
#include <sstream>
#include <cerrno>
#include "../../config.h"

#define SECONDS_PER_MONTH 2629743

FILE *kmx_stderr = stderr;
FILE *kmx_stdout = stdout;

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
	char kmotionXHomePath[MAX_PATH] = {0};
	char machineDataPath[MAX_PATH] = {0};
	char localLanguageFilePath[MAX_PATH] = {0};

	int testExecuteAccess(const char *file);
	static std::string shellQuote(const std::string &value)
	{
		std::string quoted = "'";
		for (char c : value)
			quoted += c == '\'' ? "'\\''" : std::string(1, c);
		return quoted + "'";
	}

	// variadic function for formatting strings with multiple arguments

	std::wstring format(const wchar_t *wformat, ...)
	{
		std::string format = wstrtostr(wformat);
		va_list args;
		va_start(args, wformat);
		// Calculate the size of the formatted string
		int size = std::vsnprintf(nullptr, 0, format.c_str(), args);
		va_end(args);

		if (size < 0)
			throw std::runtime_error("Formatting error");

		std::vector<char> buf(size + 1);
		va_start(args, wformat);
		// Format the string with the provided arguments
		std::vsnprintf(buf.data(), buf.size(), format.c_str(), args);
		va_end(args);

		return strtowstr(buf.data());
	}

	// Convert std::wstring to UTF-8 encoded std::string
	std::string wstrtostr(const std::wstring &wideString)
	{
		std::string result;
		result.reserve(wideString.size());

		for (wchar_t wc : wideString)
		{
			if (wc <= 0x7F)
			{
				result.push_back(static_cast<char>(wc)); // 1-byte sequence
			}
			else if (wc <= 0x7FF)
			{
				result.push_back(static_cast<char>(0xC0 | ((wc >> 6) & 0x1F))); // 2-byte sequence
				result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
			}
			else if (wc <= 0xFFFF)
			{
				result.push_back(static_cast<char>(0xE0 | ((wc >> 12) & 0x0F))); // 3-byte sequence
				result.push_back(static_cast<char>(0x80 | ((wc >> 6) & 0x3F)));
				result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
			}
			else if (wc <= 0x10FFFF)
			{
				result.push_back(static_cast<char>(0xF0 | ((wc >> 18) & 0x07))); // 4-byte sequence
				result.push_back(static_cast<char>(0x80 | ((wc >> 12) & 0x3F)));
				result.push_back(static_cast<char>(0x80 | ((wc >> 6) & 0x3F)));
				result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
			}
		}

		return result;
	}
	// Convert UTF-8 encoded char* to std::wstring
	std::wstring strtowstr(const char *narrowString)
	{
		std::wstring result;
		size_t length = std::strlen(narrowString);
		result.reserve(length); // Reserve space

		for (size_t i = 0; i < length;)
		{
			wchar_t wc = 0;
			unsigned char c = narrowString[i];

			if (c <= 0x7F)
			{ // 1-byte sequence
				wc = c;
				i += 1;
			}
			else if ((c & 0xE0) == 0xC0)
			{ // 2-byte sequence
				wc = (c & 0x1F) << 6;
				wc |= (narrowString[++i] & 0x3F);
				i += 1;
			}
			else if ((c & 0xF0) == 0xE0)
			{ // 3-byte sequence
				wc = (c & 0x0F) << 12;
				wc |= (narrowString[++i] & 0x3F) << 6;
				wc |= (narrowString[++i] & 0x3F);
				i += 1;
			}
			else if ((c & 0xF8) == 0xF0)
			{ // 4-byte sequence
				wc = (c & 0x07) << 18;
				wc |= (narrowString[++i] & 0x3F) << 12;
				wc |= (narrowString[++i] & 0x3F) << 6;
				wc |= (narrowString[++i] & 0x3F);
				i += 1;
			}

			result.push_back(wc);
		}

		return result;
	}

	// Convert UTF-8 encoded std::string to std::wstring
	std::wstring strtowstr(const std::string &narrowString)
	{
		return strtowstr(narrowString.c_str());
	}

	// string inputString = "This docment uses 3 other docments to docment the docmentation";
	// string outputString = replaceAll(errString, "docment", "document");
	std::string replaceAll(std::string str, const std::string &from, const std::string &to)
	{
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos)
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
		}
		return str;
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

	// ~/.kmxrc may select a machine root containing data/ and c-programs/.
	const char *getMachineDataPath()
	{
		if (machineDataPath[0])
		{
			return machineDataPath;
		}
		char selected[MAX_PATH] = {0};
		int rc = getResourceValue("machineDataPath", selected, sizeof(selected));
		if (rc == -2)
			throw std::runtime_error("Invalid or overlong machineDataPath in ~/.kmxrc");
		if (rc)
		{
			const char *home = getenv("HOME");
			if (!home || snprintf(selected, sizeof(selected), "%s/.kmotionx/default-machine", home) >= sizeof(selected))
				throw std::runtime_error("HOME is missing or default machine path is too long");
		}
		for (const char *file : {"data/emc.var", "data/Default.tbl", "data/Kinematics.txt"})
		{
			std::string path = std::string(selected) + "/" + file;
			if (access(path.c_str(), R_OK))
				throw std::runtime_error("Required machine file missing or unreadable: " + path);
		}
		memcpy(machineDataPath, selected, strlen(selected) + 1);
		log_info("Using Machine configuration path=%s", machineDataPath);
		return machineDataPath;
	}

	int getResourceValue(const char *searchKey, char * result, size_t maxLen)
	{
		if (!result || !maxLen) return -1;
		result[0] = '\0';
		char rc_file[MAX_PATH];
		const char *home = getenv("HOME");
		if (!home || snprintf(rc_file, sizeof(rc_file), "%s/.kmxrc", home) >= sizeof(rc_file)) return -1;
		FILE *file = fopen(rc_file, "r");
		if (file == NULL)
		{
			if (errno != ENOENT) perror(rc_file);
			return -1;
		}
		char line[MAX_PATH * 2];
		while (fgets(line, sizeof(line), file))
		{
			if (!strchr(line, '\n') && !feof(file))
			{
				int c;
				while ((c = fgetc(file)) != '\n' && c != EOF) {}
				fclose(file);
				return -2;
			}
			char *equalSign = strchr(line, '=');
			if (equalSign)
			{
				*equalSign++ = '\0';
				equalSign[strcspn(equalSign, "\r\n")] = '\0';
				if (!strcmp(line, searchKey))
				{
					if (!*equalSign || strlen(equalSign) >= maxLen)
					{
						fclose(file);
						return -2;
					}
					memcpy(result, equalSign, strlen(equalSign) + 1);
					fclose(file);
					return 0;
				}
			}
		}
		fclose(file);
		return -1;
	}
	const char *getKMotionXHomePath()
	{
		if (!kmotionXHomePath[0])
		{
			// KMOTIONX_HOME overrides the user-wide home path, not the install prefix.
			char *envPath;
			if ((envPath = getenv("KMOTIONX_HOME")) != NULL)
			{
				snprintf(kmotionXHomePath, sizeof(kmotionXHomePath), "%s", envPath);
			}
			else
			{
				if ((envPath = getenv("HOME")) != NULL)
				{
					snprintf(kmotionXHomePath, MAX_PATH, "%s/.kmotionx", envPath);
				}
				else
				{
					snprintf(kmotionXHomePath, MAX_PATH, "~/.kmotionx");
					// homedir = getpwuid(getuid())->pw_dir;
				}
			}

			log_info("Using user-wide KMotionX path=%s", kmotionXHomePath);
		}
		return kmotionXHomePath;
	}

	const char *getInstallPath()
	{
		return getKMotionXHomePath();
	}

	const char *getLocalLanguageFilePath()
	{
		if (!localLanguageFilePath[0])
		{
			const char *home = getenv("HOME");
			if (!home || snprintf(localLanguageFilePath, sizeof(localLanguageFilePath), "%s/.kmotionx/data/LocalLanguage.txt", home) >= sizeof(localLanguageFilePath))
				throw std::runtime_error("HOME is missing or user-wide language path is too long");
		}
		return localLanguageFilePath;
	}

	const char *getLibexecPath()
	{
		return KMX_LIBEXECDIR;
	}

	const char *getResourcePath()
	{
		return KMX_DATADIR;
	}

	const char *getBinPath() // legacy helper lookup API
	{
		return getLibexecPath();
	}

	int LaunchServer()
	{
		std::string server = std::string(getLibexecPath()) + "/KMotionServer";
		std::string command = shellQuote(server);
#ifdef _DEAMON
#else
		command += " -redirect_streams &";
#endif

#if defined(__APPLE__) && defined(_DEAMON)
		// The daemon is currently not supported on MacOs
		log_info("Launch KMotionServer first: %s", command.c_str());
		PipeMutex->Unlock();
		exit(1);
#endif
		log_info("Launching KMotionServer: %s", command.c_str());
		return system(command.c_str());
	}

	int getDspFile(char *OutFile, const int BoardType)
	{
		if (BoardType == BOARD_TYPE_KOGNA)
		{
			snprintf(OutFile, MAX_PATH, "%s/DSP_KOGNA/DSPKOGNA.out", getResourcePath());
		}
		else
		{
			snprintf(OutFile, MAX_PATH, "%s/DSP_KFLOP/DSPKFLOP.out", getResourcePath());
		}
		return 0;
	}

	int getCompiler(char *Compiler, int MaxCompilerLen)
	{
		// char Compiler[MAX_PATH + 1];
		if (!Compiler || MaxCompilerLen <= 0) return -1;
		Compiler[0] = '\0';
		if (customCompiler[0] == '/')
		{
			snprintf(Compiler, MaxCompilerLen, "%s", customCompiler);
			// try if compiler is accessible on absolute path
			if (testExecuteAccess(Compiler) == 0)
				return 0;
		}
		else
		{
			// try in the released directory next
			snprintf(Compiler, MaxCompilerLen, "%s/%s", getLibexecPath(), customCompiler);
			if (testExecuteAccess(Compiler) == 0)
				return 0;

			// this is for development only
			snprintf(Compiler, MaxCompilerLen, "%s/TCC67/%s", getKMotionXHomePath(), customCompiler);
			if (testExecuteAccess(Compiler) == 0)
				return 0;

			if (Compiler[0] != '.')
			{ // check if compiler is present in current dir
				snprintf(Compiler, MaxCompilerLen, "./%s", customCompiler);
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
			snprintf(customCompiler, sizeof(customCompiler), "%s", compiler);
		else
			strcpy(customCompiler, KMX_COMPILER);
		if (options)
			snprintf(customOptions, sizeof(customOptions), "%s", options);
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
			if (cmd_len > 0) snprintf(command, cmd_len, "%s", Compiler);
			return 1;
		}

		char IncSrcPath1[MAX_PATH + 1];
		char IncSrcPath2[MAX_PATH + 1];
		char BindTo[MAX_PATH + 1];

		// Get path to DSPKMotion.out or DSPKFLOP.out
		getDspFile(BindTo, BoardType);

		getPath(BindTo, IncSrcPath1);

		getPath(Name, IncSrcPath2);
		std::istringstream options(customOptions);
		std::string option, quotedOptions;
		while (options >> option) quotedOptions += " " + shellQuote(option);
		std::string quotedCompiler = shellQuote(Compiler);
		std::string quotedInc1 = shellQuote(IncSrcPath1);
		std::string quotedInc2 = shellQuote(IncSrcPath2);
		std::string quotedOutput = shellQuote(OutFile);
		std::string quotedSource = shellQuote(Name);
		std::string quotedDsp = shellQuote(BindTo);

		int written;
		if (tcc_vers < 26)
			written = snprintf(command, cmd_len, "%s -text %08X%s -nostdinc -I%s -I%s -o %s %s %s 2>&1",
					 quotedCompiler.c_str(),
					 LoadAddress,
					 quotedOptions.c_str(),
					 quotedInc1.c_str(), quotedInc2.c_str(), quotedOutput.c_str(), quotedSource.c_str(), quotedDsp.c_str());
		else
			written = snprintf(command, cmd_len, "%s -Wl,-Ttext,%08X%s -Wl,--oformat,coff -static -nostdinc -nostdlib -I%s -I%s -o %s %s %s 2>&1",
					 quotedCompiler.c_str(),
					 LoadAddress,
					 quotedOptions.c_str(),
					 quotedInc1.c_str(), quotedInc2.c_str(), quotedOutput.c_str(), quotedSource.c_str(), quotedDsp.c_str());

		// Original TCC67 Windows version shipped with KMotion
		// tcc -text 80050000 -g -nostdinc -I./DSP_KFLOP -I./ -o Gecko3Axis.out Gecko3Axis.c ./DSP_KFLOP/DSP_KFLOP.out
		//  -text replaced by -Wl,-Ttext,address
		// http://manpages.ubuntu.com/manpages/lucid/man1/tcc.1.html

		// compile with debug flag is currently not supported -g
		// c67-tcc -Wl,-Ttext,80050000 -Wl,--oformat,coff -static -nostdinc -nostdlib -I./ -o ~/Desktop/Gecko3AxisOSX.out Gecko3Axis.c DSPKFLOP.out
		if (written < 0 || written >= cmd_len)
		{
			snprintf(command, cmd_len, "Compiler command is too long");
			return 1;
		}
		return 0;
	}

	int testExecuteAccess(const char *file)
	{
		debug("Testing file for existance execute permissions %s", file);
		// log_info("Checking access=%s",file);
		struct stat sb;
		return stat(file, &sb) == 0 && S_ISREG(sb.st_mode) && access(file, X_OK) == 0 ? 0 : -1;
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
