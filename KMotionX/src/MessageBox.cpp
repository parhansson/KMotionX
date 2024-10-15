#include <iostream>
#include "MessageBox.h"
#include "KMotionX.h"

int MessageBoxConsoleHandler(const wchar_t *title, const wchar_t *msg, uint32_t uType);

MB_USER_CALLBACK *mb_callback = MessageBoxConsoleHandler;

int AfxMessageBox(const char *value, int type)
{
	return mb_callback(L"AfxMessageBox", kmx::strtowstr(value).c_str(), type);
}

int MessageBox(long hwnd, const char *value, const char *title, int type)
{
	return mb_callback(kmx::strtowstr(title).c_str(), kmx::strtowstr(value).c_str(), type);
}
int MessageBoxW(long hwnd, const wchar_t *value, const wchar_t *title, int type)
{
	return mb_callback(title, value, type);
}
int MessageBoxW(long hwnd, std::wstring value, const wchar_t *title, int type)
{
	return mb_callback(title, value.c_str(), type);
}

// Default Console handler for messageboxes
int MessageBoxConsoleHandler(const wchar_t *title, const wchar_t *msg, uint32_t uType)
{
	uint32_t options = uType & 0x0000000F;
	uint32_t iconType = uType & 0x000000F0;

	const char *utf8_exclamation_mark = "\xE2\x81\xA7";
	const char *utf8_hand = "\xE2\x9C\x8B";
	const char *utf8_asterisk = "\xE2\x9C\xAA";
	const char *utf8_question_mark = "\xE2\x9D\x93";

	const char *utf8_icon = "";
	if (iconType == MB_ICONHAND)
	{
		utf8_icon = utf8_hand;
	}
	else if (iconType == MB_ICONQUESTION)
	{
		utf8_icon = utf8_question_mark;
	}
	else if (iconType == MB_ICONEXCLAMATION)
	{
		utf8_icon = utf8_exclamation_mark;
	}
	else if (iconType == MB_ICONASTERISK)
	{
		utf8_icon = utf8_asterisk;
	}

	printf("%s--------- %ls-------------------%s\n"
		   "%ls\n"
		   "%s-----------------------------------------------%s\n",
		   utf8_icon, title, utf8_icon, msg, utf8_icon, utf8_icon);

	// Get user input from stdin
	if (options == MB_YESNO)
	{
		while (1)
		{
			wprintf(L"Enter 'y' for Yes or 'n' for No: ");
			wint_t input = fgetwc(stdin);
			if (input == L'y' || input == L'Y')
			{
				return IDYES;
			}
			else if (input == L'n' || input == L'N')
			{
				return IDNO;
			}
			else
			{
				wprintf(L"Invalid input. Please enter 'y' for Yes or 'n' for No.\n");
			}
		}
	}
	else if (options == MB_OKCANCEL)
	{
		while (1)
		{

			wprintf(L"Enter 'o' for OK or 'c' for Cancel: ");
			wint_t input = fgetwc(stdin);
			if (input == L'o' || input == L'O')
			{
				return IDOK;
			}
			else if (input == L'c' || input == L'C')
			{
				return IDCANCEL;
			}
			else
			{
				wprintf(L"Invalid input. Please enter 'o' for OK or 'c' for Cancel.\n");
			}
		}
	}
	else if (options == MB_OK)
	{
		// Default to OK if neither MB_YESNO nor MB_OKCANCEL is specified
		printf("Press enter for OK: ");
		fgetwc(stdin);
		return IDOK;
	}
	else
	{
		// Default to OK if neither MB_YESNO nor MB_OKCANCEL is specified
		printf("---Messgebox type not implemented (0x%.8X)  ---------\n", options);
		printf("Press enter for OK: ");
		fgetwc(stdin);
		return IDOK;
	}

	return 0;
}