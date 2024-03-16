#include "StdAfx.h"
#include "KMotionDLL.h"
#include "Translate.h"
#include "dbg.h"

CTranslate Trans;

std::wstring Translate(std::string s)
{
	return Trans.Translate(s);
}

CTranslate::CTranslate() : CheckedForList(false), ListLoaded(false) {}

std::wstring CTranslate::Translate(std::string s) {
    std::wstring w = Translate(s.c_str());
    return w;
}

std::wstring CTranslate::Translate(const char* s) {
    // Convert const char* to std::wstring
    std::wstring w(s, s + strlen(s));
    if (CheckedForList && !ListLoaded) return w;

    if (!CheckedForList) {
        LoadTranslationList();
    }

    if (ListLoaded) {
        // look for a match
        for (size_t i = 0; i < EnglishList.size(); ++i) {
            if (w == EnglishList[i]) {
                return TranslateList[i];
            }
        }
    }

    return w;
}

void CTranslate::LoadTranslationList() {
    wchar_t wcsString[4001];

    CheckedForList = true;

    // Open the file with the specified encoding
    // Some editors like NotePad++ don't put BOM Byte Order Mark so better to let Windows decide
    std::string filePath = kmx::getLocalLanguageFilePath();
    //log_info("reading translationfile: %s", filePath.c_str());
    std::wifstream fileStream(filePath, std::ios::binary);

    if (!fileStream.is_open()) {
        return;
    }

    std::wstring sRead, Eng, Trans;

    while (fileStream.getline(wcsString, 4000)) {
        sRead = wcsString;
        
        sRead.erase(std::remove(sRead.begin(), sRead.end(), L'\n'), sRead.end());

        // for some reason slashes are changed to double slashes
        //std::replace(sRead.begin(), sRead.end(), L'\\r', L'\r');

        // file format is English Left ..#.. Translated Right
        size_t i = sRead.find(L"    ..#..    ", 0);
        if (i != std::wstring::npos) {
            //log_info("line: %zu -- %ls", i, sRead.c_str());
            Eng = sRead.substr(0, i);
            //log_info("Eng: %ls", Eng.c_str());
            Trans = sRead.substr(i + 13);
            //log_info("Trans: %ls", Trans.c_str());
            EnglishList.push_back(Eng);
            TranslateList.push_back(Trans);
        }

    };

    ListLoaded = true;
    fileStream.close();

    // sort by length
    for (size_t i = 0; i < EnglishList.size(); ++i) {
        for (size_t k = i + 1; k < EnglishList.size(); ++k) {
            if (EnglishList[k].length() > EnglishList[i].length()) {
                std::swap(EnglishList[i], EnglishList[k]);
                std::swap(TranslateList[i], TranslateList[k]);
            }
        }
    }
}