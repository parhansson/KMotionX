#include "stdafx.h"
#include "KMotionDLL.h"
#include "Translate.h"

// Translate.cpp : Translate to local language
/*********************************************************************/
/*         Copyright (c) 2003-2024  DynoMotion Incorporated          */
/*********************************************************************/

extern CString MainPath;

CTranslate Trans;

CStringW Translate(CString s)
{
	return Trans.Translate(s);
}

CTranslate::CTranslate()
{
	CheckedForList = ListLoaded = false;
}

CStringW CTranslate::Translate(CString s)
{
	CStringW w = Translate(s.GetBuffer());
	return (w);
}

CStringW CTranslate::Translate(char* s)
{
	CStringW w = s;
	if (CheckedForList && !ListLoaded)  return w;


	if (!CheckedForList)
	{
		wchar_t wcsString[4001];

		CheckedForList = true;

		// Open the file with the specified encoding
		// Some editors like NotePad++ don't put BOM Byte Order Mark so better to let Windows decide
		FILE* fStream;
		errno_t e = _tfopen_s(&fStream, MainPath + "\\Data\\LocalLanguage.txt", _T("rt,ccs=UNICODE"));
		if (e != 0)  // failed..CString sRead;
			return w;

		CStringW sRead, Eng, Trans;
		bool bReadData;
		do
		{
			bReadData = (NULL != fgetws(wcsString, 4000, fStream));

			sRead = wcsString;

			sRead.Remove('\n');

			// for some reason slashes are changed to double slashes
			sRead.Replace(L"\\r", L"\r");

			// file format is English Left ..#.. Translated Right
			int i = sRead.Find((CStringW)"    ..#..    ", 0);

			Eng = sRead.Left(i);
			Trans = sRead.Right(sRead.GetLength() - i - 13);

			EnglishList.AddTail(Eng);
			TanslateList.AddTail(Trans);

		} while (bReadData);

		ListLoaded = true;

		fclose(fStream);

		CStringW sip, skp;
		CStringW sip2, skp2;
		// sort by length
		POSITION i2 = TanslateList.GetHeadPosition(), k2, k;
		for (POSITION i = EnglishList.GetHeadPosition(); i != NULL; )
		{
			k = i;
			EnglishList.GetNext(k);  // k starts next after i
			if (k == NULL)  // all done
				break;
			sip = EnglishList.GetAt(i);
			EnglishList.GetNext(i);

			k2 = i2;
			TanslateList.GetNext(k2);
			sip2 = TanslateList.GetAt(i2);
			TanslateList.GetNext(i2);


			for (; k != NULL;)
			{
				skp = EnglishList.GetAt(k);
				EnglishList.GetNext(k);
				skp2 = TanslateList.GetAt(k2);
				TanslateList.GetNext(k2);
				if (skp.GetLength() > sip.GetLength())
				{
					CStringW temp = sip;
					skp = sip;  // swap in both lists
					skp = temp;
					temp = sip2;
					skp2 = sip2;
					skp2 = temp;
				}
			}
		}

	}

	if (ListLoaded)
	{
		// look for match

		CStringW e,t;

		POSITION i2 = TanslateList.GetHeadPosition();
		for (POSITION i = EnglishList.GetHeadPosition(); i != NULL; )
		{
			e = EnglishList.GetNext(i);   // get English
			t = TanslateList.GetNext(i2); // get translated
			if (w == e)  // match?
			{
				return t;
			}
		}
	}

	return w;
}