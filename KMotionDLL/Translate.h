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

    CString Translate(CString s);

private:
    bool CheckedForList;
    bool ListLoaded;

    std::vector<std::wstring> EnglishList;
    std::vector<std::wstring> TranslateList;

    void LoadTranslationList();
};
extern CTranslate Trans;  // global instance

CString Translate(CString s);
#else
#pragma once

// This class is exported from the GCodeInterpreter.dll


class  CTranslate {
public:
	CTranslate();
	CString Translate(CString s);

	bool CheckedForList;
	bool ListLoaded;

	CList<CString, CString> EnglishList;
	CList<CString, CString> TanslateList;

};

extern CTranslate Trans;  // global instance

CString Translate(CString s);
#endif
#endif //TRANSLATE_H_