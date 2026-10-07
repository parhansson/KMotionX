#ifndef TRANSLATE_H_
#define TRANSLATE_H_

#ifdef _KMOTIONX
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

//extern std::wstring MainPath;

class CTranslate {
public:
    CTranslate();

    std::wstring Translate(std::string s);
    std::wstring Translate(const char* s);

private:
    bool CheckedForList;
    bool ListLoaded;

    std::vector<std::wstring> EnglishList;
    std::vector<std::wstring> TranslateList;
	std::wstring Translate(std::string* s);

    void LoadTranslationList();
};
extern CTranslate Trans;  // global instance

std::wstring Translate(std::string s);
#else
#pragma once

// This class is exported from the GCodeInterpreter.dll


class  CTranslate {
public:
	CTranslate();
	CStringW Translate(CString s);
	CStringW Translate(char*);

	bool CheckedForList;
	bool ListLoaded;

	CList<CStringW, CStringW> EnglishList;
	CList<CStringW, CStringW> TanslateList;

};

extern CTranslate Trans;  // global instance

CStringW Translate(CString s);
#endif
#endif //TRANSLATE_H_