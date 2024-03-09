/*
Module : ScintillaCtrl.h
Purpose: Defines the interface for an MFC wrapper class for the Scintilla edit control (www.scintilla.org)
Created: PJN / 19-03-2004

Copyright (c) 2004 - 2009 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


//////////////////// Macros / Defines /////////////////////////////////////////

#pragma once

#ifndef __SCINTILLACTRL_H__
#define __SCINTILLACTRL_H__

#ifndef SCINTILLA_H
#pragma message("To avoid this message, please put scintilla.h in your pre compiled header (normally stdafx.h)")
#include <Scintilla.h>
#endif

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

#ifndef SCINTILLACTRL_EXT_CLASS
#define SCINTILLACTRL_EXT_CLASS
#endif


//////////////////// Classes //////////////////////////////////////////////////

class SCINTILLACTRL_EXT_CLASS CScintillaCtrl : public CWnd
{
public:
//Constructors / Destructors
  CScintillaCtrl();
  virtual ~CScintillaCtrl();

//Creation
  BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwExStyle = 0, LPVOID lpParam = NULL);

//Misc
  void SetupDirectAccess();
  inline LRESULT Call(UINT message, WPARAM wParam, LPARAM lParam, BOOL bDirect = TRUE);
  LRESULT GetDirectFunction();
  LRESULT GetDirectPointer();

//Unicode support
#ifdef _UNICODE
  void AddText(int length, const wchar_t* text, BOOL bDirect = TRUE);
  void InsertText(long pos, const wchar_t* text, BOOL bDirect = TRUE);
  CStringW GetSelText(BOOL bDirect = TRUE);
  CStringW GetCurLine(BOOL bDirect = TRUE);
  void StyleSetFont(int style, const wchar_t* fontName, BOOL bDirect = TRUE);
  void SetWordChars(const wchar_t* characters, BOOL bDirect = TRUE);
  void AutoCShow(int lenEntered, const wchar_t* itemList, BOOL bDirect = TRUE);
  void AutoCStops(const wchar_t* characterSet, BOOL bDirect = TRUE);
  void AutoCSelect(const wchar_t* text, BOOL bDirect = TRUE);
  void AutoCSetFillUps(const wchar_t* characterSet, BOOL bDirect = TRUE);
  void UserListShow(int listType, const wchar_t* itemList, BOOL bDirect = TRUE);
  CStringW GetLine(int line, BOOL bDirect = TRUE);
  void ReplaceSel(const wchar_t* text, BOOL bDirect = TRUE);
  void SetText(const wchar_t* text, BOOL bDirect = TRUE);
  CStringW GetText(int length, BOOL bDirect = TRUE);
  int ReplaceTarget(int length, const wchar_t* text, BOOL bDirect = TRUE);
  int ReplaceTargetRE(int length, const wchar_t* text, BOOL bDirect = TRUE);
  int SearchInTarget(int length, const wchar_t* text, BOOL bDirect = TRUE);
  void CallTipShow(long pos, const wchar_t* definition, BOOL bDirect = TRUE);
  int TextWidth(int style, const wchar_t* text, BOOL bDirect = TRUE);
  void AppendText(int length, const wchar_t* text, BOOL bDirect = TRUE);
  int SearchNext(int flags, const wchar_t* text, BOOL bDirect = TRUE);
  int SearchPrev(int flags, const wchar_t* text, BOOL bDirect = TRUE);
  void CopyText(int length, const wchar_t* text, BOOL bDirect = TRUE);
  void SetWhitespaceChars(const wchar_t* characters, BOOL bDirect = TRUE);
  void SetProperty(const wchar_t* key, const wchar_t* value, BOOL bDirect = TRUE);
  void SetKeyWords(int keywordSet, const wchar_t* keyWords, BOOL bDirect = TRUE);
  void SetLexerLanguage(const wchar_t* language, BOOL bDirect = TRUE);
  void LoadLexerLibrary(const wchar_t* path, BOOL bDirect = TRUE);
  CStringW GetProperty(const wchar_t* key, BOOL bDirect = TRUE);
  CStringW GetPropertyExpanded(const wchar_t* key, BOOL bDirect = TRUE);
  int GetPropertyInt(const wchar_t* key, BOOL bDirect = TRUE);
  CStringW StyleGetFont(int style, BOOL bDirect = TRUE);

  static CStringW UTF82W(const char* pszText, int nLength);
  static CStringA W2UTF8(const wchar_t* pszText, int nLength);
#else
  CStringA GetSelText(BOOL bDirect = TRUE);
  CStringA GetCurLine(BOOL bDirect = TRUE);
  CStringA GetLine(int line, BOOL bDirect = TRUE);
  CStringA GetProperty(const char* key, BOOL bDirect = TRUE);
  CStringA GetText(int length, BOOL bDirect = TRUE);
  CStringA GetPropertyExpanded(const char* key, BOOL bDirect = TRUE);
  CStringA StyleGetFont(int style, BOOL bDirect = TRUE);
#endif

//Auto generated using the "ConvertScintillaiface.js" script
  void AddText(int length, const char* text, BOOL bDirect = TRUE);
  void AddStyledText(int length, char* c, BOOL bDirect = TRUE);
  void InsertText(long pos, const char* text, BOOL bDirect = TRUE);
  void ClearAll(BOOL bDirect = TRUE);
  void ClearDocumentStyle(BOOL bDirect = TRUE);
  int GetLength(BOOL bDirect = TRUE);
  int GetCharAt(long pos, BOOL bDirect = TRUE);
  long GetCurrentPos(BOOL bDirect = TRUE);
  long GetAnchor(BOOL bDirect = TRUE);
  int GetStyleAt(long pos, BOOL bDirect = TRUE);
  void Redo(BOOL bDirect = TRUE);
  void SetUndoCollection(BOOL collectUndo, BOOL bDirect = TRUE);
  void SelectAll(BOOL bDirect = TRUE);
  void SetSavePoint(BOOL bDirect = TRUE);
  int GetStyledText(TextRange* tr, BOOL bDirect = TRUE);
  BOOL CanRedo(BOOL bDirect = TRUE);
  int MarkerLineFromHandle(int handle, BOOL bDirect = TRUE);
  void MarkerDeleteHandle(int handle, BOOL bDirect = TRUE);
  BOOL GetUndoCollection(BOOL bDirect = TRUE);
  int GetViewWS(BOOL bDirect = TRUE);
  void SetViewWS(int viewWS, BOOL bDirect = TRUE);
  long PositionFromPoint(int x, int y, BOOL bDirect = TRUE);
  long PositionFromPointClose(int x, int y, BOOL bDirect = TRUE);
  void GotoLine(int line, BOOL bDirect = TRUE);
  void GotoPos(long pos, BOOL bDirect = TRUE);
  void SetAnchor(long posAnchor, BOOL bDirect = TRUE);
  int GetCurLine(int length, char* text, BOOL bDirect = TRUE);
  long GetEndStyled(BOOL bDirect = TRUE);
  void ConvertEOLs(int eolMode, BOOL bDirect = TRUE);
  int GetEOLMode(BOOL bDirect = TRUE);
  void SetEOLMode(int eolMode, BOOL bDirect = TRUE);
  void StartStyling(long pos, int mask, BOOL bDirect = TRUE);
  void SetStyling(int length, int style, BOOL bDirect = TRUE);
  BOOL GetBufferedDraw(BOOL bDirect = TRUE);
  void SetBufferedDraw(BOOL buffered, BOOL bDirect = TRUE);
  void SetTabWidth(int tabWidth, BOOL bDirect = TRUE);
  int GetTabWidth(BOOL bDirect = TRUE);
  void SetCodePage(int codePage, BOOL bDirect = TRUE);
  void SetUsePalette(BOOL usePalette, BOOL bDirect = TRUE);
  void MarkerDefine(int markerNumber, int markerSymbol, BOOL bDirect = TRUE);
  void MarkerSetFore(int markerNumber, COLORREF fore, BOOL bDirect = TRUE);
  void MarkerSetBack(int markerNumber, COLORREF back, BOOL bDirect = TRUE);
  int MarkerAdd(int line, int markerNumber, BOOL bDirect = TRUE);
  void MarkerDelete(int line, int markerNumber, BOOL bDirect = TRUE);
  void MarkerDeleteAll(int markerNumber, BOOL bDirect = TRUE);
  int MarkerGet(int line, BOOL bDirect = TRUE);
  int MarkerNext(int lineStart, int markerMask, BOOL bDirect = TRUE);
  int MarkerPrevious(int lineStart, int markerMask, BOOL bDirect = TRUE);
  void MarkerDefinePixmap(int markerNumber, const char* pixmap, BOOL bDirect = TRUE);
  void MarkerAddSet(int line, int set, BOOL bDirect = TRUE);
  void MarkerSetAlpha(int markerNumber, int alpha, BOOL bDirect = TRUE);
  void SetMarginTypeN(int margin, int marginType, BOOL bDirect = TRUE);
  int GetMarginTypeN(int margin, BOOL bDirect = TRUE);
  void SetMarginWidthN(int margin, int pixelWidth, BOOL bDirect = TRUE);
  int GetMarginWidthN(int margin, BOOL bDirect = TRUE);
  void SetMarginMaskN(int margin, int mask, BOOL bDirect = TRUE);
  int GetMarginMaskN(int margin, BOOL bDirect = TRUE);
  void SetMarginSensitiveN(int margin, BOOL sensitive, BOOL bDirect = TRUE);
  BOOL GetMarginSensitiveN(int margin, BOOL bDirect = TRUE);
  void StyleClearAll(BOOL bDirect = TRUE);
  void StyleSetFore(int style, COLORREF fore, BOOL bDirect = TRUE);
  void StyleSetBack(int style, COLORREF back, BOOL bDirect = TRUE);
  void StyleSetBold(int style, BOOL bold, BOOL bDirect = TRUE);
  void StyleSetItalic(int style, BOOL italic, BOOL bDirect = TRUE);
  void StyleSetSize(int style, int sizePoints, BOOL bDirect = TRUE);
  void StyleSetFont(int style, const char* fontName, BOOL bDirect = TRUE);
  void StyleSetEOLFilled(int style, BOOL filled, BOOL bDirect = TRUE);
  void StyleResetDefault(BOOL bDirect = TRUE);
  void StyleSetUnderline(int style, BOOL underline, BOOL bDirect = TRUE);
  COLORREF StyleGetFore(int style, BOOL bDirect = TRUE);
  COLORREF StyleGetBack(int style, BOOL bDirect = TRUE);
  BOOL StyleGetBold(int style, BOOL bDirect = TRUE);
  BOOL StyleGetItalic(int style, BOOL bDirect = TRUE);
  int StyleGetSize(int style, BOOL bDirect = TRUE);
  int StyleGetFont(int style, char* fontName, BOOL bDirect = TRUE);
  BOOL StyleGetEOLFilled(int style, BOOL bDirect = TRUE);
  BOOL StyleGetUnderline(int style, BOOL bDirect = TRUE);
  int StyleGetCase(int style, BOOL bDirect = TRUE);
  int StyleGetCharacterSet(int style, BOOL bDirect = TRUE);
  BOOL StyleGetVisible(int style, BOOL bDirect = TRUE);
  BOOL StyleGetChangeable(int style, BOOL bDirect = TRUE);
  BOOL StyleGetHotSpot(int style, BOOL bDirect = TRUE);
  void StyleSetCase(int style, int caseForce, BOOL bDirect = TRUE);
  void StyleSetCharacterSet(int style, int characterSet, BOOL bDirect = TRUE);
  void StyleSetHotSpot(int style, BOOL hotspot, BOOL bDirect = TRUE);
  void SetSelFore(BOOL useSetting, COLORREF fore, BOOL bDirect = TRUE);
  void SetSelBack(BOOL useSetting, COLORREF back, BOOL bDirect = TRUE);
  int GetSelAlpha(BOOL bDirect = TRUE);
  void SetSelAlpha(int alpha, BOOL bDirect = TRUE);
  BOOL GetSelEOLFilled(BOOL bDirect = TRUE);
  void SetSelEOLFilled(BOOL filled, BOOL bDirect = TRUE);
  void SetCaretFore(COLORREF fore, BOOL bDirect = TRUE);
  void AssignCmdKey(DWORD km, int msg, BOOL bDirect = TRUE);
  void ClearCmdKey(DWORD km, BOOL bDirect = TRUE);
  void ClearAllCmdKeys(BOOL bDirect = TRUE);
  void SetStylingEx(int length, const char* styles, BOOL bDirect = TRUE);
  void StyleSetVisible(int style, BOOL visible, BOOL bDirect = TRUE);
  int GetCaretPeriod(BOOL bDirect = TRUE);
  void SetCaretPeriod(int periodMilliseconds, BOOL bDirect = TRUE);
  void SetWordChars(const char* characters, BOOL bDirect = TRUE);
  void BeginUndoAction(BOOL bDirect = TRUE);
  void EndUndoAction(BOOL bDirect = TRUE);
  void IndicSetStyle(int indic, int style, BOOL bDirect = TRUE);
  int IndicGetStyle(int indic, BOOL bDirect = TRUE);
  void IndicSetFore(int indic, COLORREF fore, BOOL bDirect = TRUE);
  COLORREF IndicGetFore(int indic, BOOL bDirect = TRUE);
  void IndicSetUnder(int indic, BOOL under, BOOL bDirect = TRUE);
  BOOL IndicGetUnder(int indic, BOOL bDirect = TRUE);
  void SetWhitespaceFore(BOOL useSetting, COLORREF fore, BOOL bDirect = TRUE);
  void SetWhitespaceBack(BOOL useSetting, COLORREF back, BOOL bDirect = TRUE);
  void SetStyleBits(int bits, BOOL bDirect = TRUE);
  int GetStyleBits(BOOL bDirect = TRUE);
  void SetLineState(int line, int state, BOOL bDirect = TRUE);
  int GetLineState(int line, BOOL bDirect = TRUE);
  int GetMaxLineState(BOOL bDirect = TRUE);
  BOOL GetCaretLineVisible(BOOL bDirect = TRUE);
  void SetCaretLineVisible(BOOL show, BOOL bDirect = TRUE);
  COLORREF GetCaretLineBack(BOOL bDirect = TRUE);
  void SetCaretLineBack(COLORREF back, BOOL bDirect = TRUE);
  void StyleSetChangeable(int style, BOOL changeable, BOOL bDirect = TRUE);
  void AutoCShow(int lenEntered, const char* itemList, BOOL bDirect = TRUE);
  void AutoCCancel(BOOL bDirect = TRUE);
  BOOL AutoCActive(BOOL bDirect = TRUE);
  long AutoCPosStart(BOOL bDirect = TRUE);
  void AutoCComplete(BOOL bDirect = TRUE);
  void AutoCStops(const char* characterSet, BOOL bDirect = TRUE);
  void AutoCSetSeparator(int separatorCharacter, BOOL bDirect = TRUE);
  int AutoCGetSeparator(BOOL bDirect = TRUE);
  void AutoCSelect(const char* text, BOOL bDirect = TRUE);
  void AutoCSetCancelAtStart(BOOL cancel, BOOL bDirect = TRUE);
  BOOL AutoCGetCancelAtStart(BOOL bDirect = TRUE);
  void AutoCSetFillUps(const char* characterSet, BOOL bDirect = TRUE);
  void AutoCSetChooseSingle(BOOL chooseSingle, BOOL bDirect = TRUE);
  BOOL AutoCGetChooseSingle(BOOL bDirect = TRUE);
  void AutoCSetIgnoreCase(BOOL ignoreCase, BOOL bDirect = TRUE);
  BOOL AutoCGetIgnoreCase(BOOL bDirect = TRUE);
  void UserListShow(int listType, const char* itemList, BOOL bDirect = TRUE);
  void AutoCSetAutoHide(BOOL autoHide, BOOL bDirect = TRUE);
  BOOL AutoCGetAutoHide(BOOL bDirect = TRUE);
  void AutoCSetDropRestOfWord(BOOL dropRestOfWord, BOOL bDirect = TRUE);
  BOOL AutoCGetDropRestOfWord(BOOL bDirect = TRUE);
  void RegisterImage(int type, const char* xpmData, BOOL bDirect = TRUE);
  void ClearRegisteredImages(BOOL bDirect = TRUE);
  int AutoCGetTypeSeparator(BOOL bDirect = TRUE);
  void AutoCSetTypeSeparator(int separatorCharacter, BOOL bDirect = TRUE);
  void AutoCSetMaxWidth(int characterCount, BOOL bDirect = TRUE);
  int AutoCGetMaxWidth(BOOL bDirect = TRUE);
  void AutoCSetMaxHeight(int rowCount, BOOL bDirect = TRUE);
  int AutoCGetMaxHeight(BOOL bDirect = TRUE);
  void SetIndent(int indentSize, BOOL bDirect = TRUE);
  int GetIndent(BOOL bDirect = TRUE);
  void SetUseTabs(BOOL useTabs, BOOL bDirect = TRUE);
  BOOL GetUseTabs(BOOL bDirect = TRUE);
  void SetLineIndentation(int line, int indentSize, BOOL bDirect = TRUE);
  int GetLineIndentation(int line, BOOL bDirect = TRUE);
  long GetLineIndentPosition(int line, BOOL bDirect = TRUE);
  int GetColumn(long pos, BOOL bDirect = TRUE);
  void SetHScrollBar(BOOL show, BOOL bDirect = TRUE);
  BOOL GetHScrollBar(BOOL bDirect = TRUE);
  void SetIndentationGuides(int indentView, BOOL bDirect = TRUE);
  int GetIndentationGuides(BOOL bDirect = TRUE);
  void SetHighlightGuide(int column, BOOL bDirect = TRUE);
  int GetHighlightGuide(BOOL bDirect = TRUE);
  int GetLineEndPosition(int line, BOOL bDirect = TRUE);
  int GetCodePage(BOOL bDirect = TRUE);
  COLORREF GetCaretFore(BOOL bDirect = TRUE);
  BOOL GetUsePalette(BOOL bDirect = TRUE);
  BOOL GetReadOnly(BOOL bDirect = TRUE);
  void SetCurrentPos(long pos, BOOL bDirect = TRUE);
  void SetSelectionStart(long pos, BOOL bDirect = TRUE);
  long GetSelectionStart(BOOL bDirect = TRUE);
  void SetSelectionEnd(long pos, BOOL bDirect = TRUE);
  long GetSelectionEnd(BOOL bDirect = TRUE);
  void SetPrintMagnification(int magnification, BOOL bDirect = TRUE);
  int GetPrintMagnification(BOOL bDirect = TRUE);
  void SetPrintColourMode(int mode, BOOL bDirect = TRUE);
  int GetPrintColourMode(BOOL bDirect = TRUE);
  long FindText(int flags, TextToFind* ft, BOOL bDirect = TRUE);
  long FormatRange(BOOL draw, RangeToFormat* fr, BOOL bDirect = TRUE);
  int GetFirstVisibleLine(BOOL bDirect = TRUE);
  int SetFirstVisibleLine(int Line, BOOL bDirect = TRUE);
  int GetLine(int line, char* text, BOOL bDirect = TRUE);
  int GetLineCount(BOOL bDirect = TRUE);
  void SetMarginLeft(int pixelWidth, BOOL bDirect = TRUE);
  int GetMarginLeft(BOOL bDirect = TRUE);
  void SetMarginRight(int pixelWidth, BOOL bDirect = TRUE);
  int GetMarginRight(BOOL bDirect = TRUE);
  BOOL GetModify(BOOL bDirect = TRUE);
  void SetSel(long start, long end, BOOL bDirect = TRUE);
  int GetSelText(char* text, BOOL bDirect = TRUE);
  int GetTextRange(TextRange* tr, BOOL bDirect = TRUE);
  void HideSelection(BOOL normal, BOOL bDirect = TRUE);
  int PointXFromPosition(long pos, BOOL bDirect = TRUE);
  int PointYFromPosition(long pos, BOOL bDirect = TRUE);
  int LineFromPosition(long pos, BOOL bDirect = TRUE);
  long PositionFromLine(int line, BOOL bDirect = TRUE);
  void LineScroll(int columns, int lines, BOOL bDirect = TRUE);
  void ScrollCaret(BOOL bDirect = TRUE);
  void ReplaceSel(const char* text, BOOL bDirect = TRUE);
  void SetReadOnly(BOOL readOnly, BOOL bDirect = TRUE);
  void Null(BOOL bDirect = TRUE);
  BOOL CanPaste(BOOL bDirect = TRUE);
  BOOL CanUndo(BOOL bDirect = TRUE);
  void EmptyUndoBuffer(BOOL bDirect = TRUE);
  void Undo(BOOL bDirect = TRUE);
  void Cut(BOOL bDirect = TRUE);
  void Copy(BOOL bDirect = TRUE);
  void Paste(BOOL bDirect = TRUE);
  void Clear(BOOL bDirect = TRUE);
  void SetText(const char* text, BOOL bDirect = TRUE);
  int GetText(int length, char* text, BOOL bDirect = TRUE);
  int GetTextLength(BOOL bDirect = TRUE);
  void SetOvertype(BOOL overtype, BOOL bDirect = TRUE);
  BOOL GetOvertype(BOOL bDirect = TRUE);
  void SetCaretWidth(int pixelWidth, BOOL bDirect = TRUE);
  int GetCaretWidth(BOOL bDirect = TRUE);
  void SetTargetStart(long pos, BOOL bDirect = TRUE);
  long GetTargetStart(BOOL bDirect = TRUE);
  void SetTargetEnd(long pos, BOOL bDirect = TRUE);
  long GetTargetEnd(BOOL bDirect = TRUE);
  int ReplaceTarget(int length, const char* text, BOOL bDirect = TRUE);
  int ReplaceTargetRE(int length, const char* text, BOOL bDirect = TRUE);
  int SearchInTarget(int length, const char* text, BOOL bDirect = TRUE);
  void SetSearchFlags(int flags, BOOL bDirect = TRUE);
  int GetSearchFlags(BOOL bDirect = TRUE);
  void CallTipShow(long pos, const char* definition, BOOL bDirect = TRUE);
  void CallTipCancel(BOOL bDirect = TRUE);
  BOOL CallTipActive(BOOL bDirect = TRUE);
  long CallTipPosStart(BOOL bDirect = TRUE);
  void CallTipSetHlt(int start, int end, BOOL bDirect = TRUE);
  void CallTipSetBack(COLORREF back, BOOL bDirect = TRUE);
  void CallTipSetFore(COLORREF fore, BOOL bDirect = TRUE);
  void CallTipSetForeHlt(COLORREF fore, BOOL bDirect = TRUE);
  void CallTipUseStyle(int tabSize, BOOL bDirect = TRUE);
  int VisibleFromDocLine(int line, BOOL bDirect = TRUE);
  int DocLineFromVisible(int lineDisplay, BOOL bDirect = TRUE);
  int WrapCount(int line, BOOL bDirect = TRUE);
  void SetFoldLevel(int line, int level, BOOL bDirect = TRUE);
  int GetFoldLevel(int line, BOOL bDirect = TRUE);
  int GetLastChild(int line, int level, BOOL bDirect = TRUE);
  int GetFoldParent(int line, BOOL bDirect = TRUE);
  void ShowLines(int lineStart, int lineEnd, BOOL bDirect = TRUE);
  void HideLines(int lineStart, int lineEnd, BOOL bDirect = TRUE);
  BOOL GetLineVisible(int line, BOOL bDirect = TRUE);
  void SetFoldExpanded(int line, BOOL expanded, BOOL bDirect = TRUE);
  BOOL GetFoldExpanded(int line, BOOL bDirect = TRUE);
  void ToggleFold(int line, BOOL bDirect = TRUE);
  void EnsureVisible(int line, BOOL bDirect = TRUE);
  void SetFoldFlags(int flags, BOOL bDirect = TRUE);
  void EnsureVisibleEnforcePolicy(int line, BOOL bDirect = TRUE);
  void SetTabIndents(BOOL tabIndents, BOOL bDirect = TRUE);
  BOOL GetTabIndents(BOOL bDirect = TRUE);
  void SetBackSpaceUnIndents(BOOL bsUnIndents, BOOL bDirect = TRUE);
  BOOL GetBackSpaceUnIndents(BOOL bDirect = TRUE);
  void SetMouseDwellTime(int periodMilliseconds, BOOL bDirect = TRUE);
  int GetMouseDwellTime(BOOL bDirect = TRUE);
  int WordStartPosition(long pos, BOOL onlyWordCharacters, BOOL bDirect = TRUE);
  int WordEndPosition(long pos, BOOL onlyWordCharacters, BOOL bDirect = TRUE);
  void SetWrapMode(int mode, BOOL bDirect = TRUE);
  int GetWrapMode(BOOL bDirect = TRUE);
  void SetWrapVisualFlags(int wrapVisualFlags, BOOL bDirect = TRUE);
  int GetWrapVisualFlags(BOOL bDirect = TRUE);
  void SetWrapVisualFlagsLocation(int wrapVisualFlagsLocation, BOOL bDirect = TRUE);
  int GetWrapVisualFlagsLocation(BOOL bDirect = TRUE);
  void SetWrapStartIndent(int indent, BOOL bDirect = TRUE);
  int GetWrapStartIndent(BOOL bDirect = TRUE);
  void SetLayoutCache(int mode, BOOL bDirect = TRUE);
  int GetLayoutCache(BOOL bDirect = TRUE);
  void SetScrollWidth(int pixelWidth, BOOL bDirect = TRUE);
  int GetScrollWidth(BOOL bDirect = TRUE);
  void SetScrollWidthTracking(BOOL tracking, BOOL bDirect = TRUE);
  BOOL GetScrollWidthTracking(BOOL bDirect = TRUE);
  int TextWidth(int style, const char* text, BOOL bDirect = TRUE);
  void SetEndAtLastLine(BOOL endAtLastLine, BOOL bDirect = TRUE);
  BOOL GetEndAtLastLine(BOOL bDirect = TRUE);
  int TextHeight(int line, BOOL bDirect = TRUE);
  void SetVScrollBar(BOOL show, BOOL bDirect = TRUE);
  BOOL GetVScrollBar(BOOL bDirect = TRUE);
  void AppendText(int length, const char* text, BOOL bDirect = TRUE);
  BOOL GetTwoPhaseDraw(BOOL bDirect = TRUE);
  void SetTwoPhaseDraw(BOOL twoPhase, BOOL bDirect = TRUE);
  void TargetFromSelection(BOOL bDirect = TRUE);
  void LinesJoin(BOOL bDirect = TRUE);
  void LinesSplit(int pixelWidth, BOOL bDirect = TRUE);
  void SetFoldMarginColour(BOOL useSetting, COLORREF back, BOOL bDirect = TRUE);
  void SetFoldMarginHiColour(BOOL useSetting, COLORREF fore, BOOL bDirect = TRUE);
  void LineDown(BOOL bDirect = TRUE);
  void LineDownExtend(BOOL bDirect = TRUE);
  void LineUp(BOOL bDirect = TRUE);
  void LineUpExtend(BOOL bDirect = TRUE);
  void CharLeft(BOOL bDirect = TRUE);
  void CharLeftExtend(BOOL bDirect = TRUE);
  void CharRight(BOOL bDirect = TRUE);
  void CharRightExtend(BOOL bDirect = TRUE);
  void WordLeft(BOOL bDirect = TRUE);
  void WordLeftExtend(BOOL bDirect = TRUE);
  void WordRight(BOOL bDirect = TRUE);
  void WordRightExtend(BOOL bDirect = TRUE);
  void Home(BOOL bDirect = TRUE);
  void HomeExtend(BOOL bDirect = TRUE);
  void LineEnd(BOOL bDirect = TRUE);
  void LineEndExtend(BOOL bDirect = TRUE);
  void DocumentStart(BOOL bDirect = TRUE);
  void DocumentStartExtend(BOOL bDirect = TRUE);
  void DocumentEnd(BOOL bDirect = TRUE);
  void DocumentEndExtend(BOOL bDirect = TRUE);
  void PageUp(BOOL bDirect = TRUE);
  void PageUpExtend(BOOL bDirect = TRUE);
  void PageDown(BOOL bDirect = TRUE);
  void PageDownExtend(BOOL bDirect = TRUE);
  void EditToggleOvertype(BOOL bDirect = TRUE);
  void Cancel(BOOL bDirect = TRUE);
  void DeleteBack(BOOL bDirect = TRUE);
  void Tab(BOOL bDirect = TRUE);
  void BackTab(BOOL bDirect = TRUE);
  void NewLine(BOOL bDirect = TRUE);
  void FormFeed(BOOL bDirect = TRUE);
  void VCHome(BOOL bDirect = TRUE);
  void VCHomeExtend(BOOL bDirect = TRUE);
  void ZoomIn(BOOL bDirect = TRUE);
  void ZoomOut(BOOL bDirect = TRUE);
  void DelWordLeft(BOOL bDirect = TRUE);
  void DelWordRight(BOOL bDirect = TRUE);
  void DelWordRightEnd(BOOL bDirect = TRUE);
  void LineCut(BOOL bDirect = TRUE);
  void LineDelete(BOOL bDirect = TRUE);
  void LineTranspose(BOOL bDirect = TRUE);
  void LineDuplicate(BOOL bDirect = TRUE);
  void LowerCase(BOOL bDirect = TRUE);
  void UpperCase(BOOL bDirect = TRUE);
  void LineScrollDown(BOOL bDirect = TRUE);
  void LineScrollUp(BOOL bDirect = TRUE);
  void DeleteBackNotLine(BOOL bDirect = TRUE);
  void HomeDisplay(BOOL bDirect = TRUE);
  void HomeDisplayExtend(BOOL bDirect = TRUE);
  void LineEndDisplay(BOOL bDirect = TRUE);
  void LineEndDisplayExtend(BOOL bDirect = TRUE);
  void HomeWrap(BOOL bDirect = TRUE);
  void HomeWrapExtend(BOOL bDirect = TRUE);
  void LineEndWrap(BOOL bDirect = TRUE);
  void LineEndWrapExtend(BOOL bDirect = TRUE);
  void VCHomeWrap(BOOL bDirect = TRUE);
  void VCHomeWrapExtend(BOOL bDirect = TRUE);
  void LineCopy(BOOL bDirect = TRUE);
  void MoveCaretInsideView(BOOL bDirect = TRUE);
  int LineLength(int line, BOOL bDirect = TRUE);
  void BraceHighlight(long pos1, long pos2, BOOL bDirect = TRUE);
  void BraceBadLight(long pos, BOOL bDirect = TRUE);
  long BraceMatch(long pos, BOOL bDirect = TRUE);
  BOOL GetViewEOL(BOOL bDirect = TRUE);
  void SetViewEOL(BOOL visible, BOOL bDirect = TRUE);
  int GetDocPointer(BOOL bDirect = TRUE);
  void SetDocPointer(int pointer, BOOL bDirect = TRUE);
  void SetModEventMask(int mask, BOOL bDirect = TRUE);
  int GetEdgeColumn(BOOL bDirect = TRUE);
  void SetEdgeColumn(int column, BOOL bDirect = TRUE);
  int GetEdgeMode(BOOL bDirect = TRUE);
  void SetEdgeMode(int mode, BOOL bDirect = TRUE);
  COLORREF GetEdgeColour(BOOL bDirect = TRUE);
  void SetEdgeColour(COLORREF edgeColour, BOOL bDirect = TRUE);
  void SearchAnchor(BOOL bDirect = TRUE);
  int SearchNext(int flags, const char* text, BOOL bDirect = TRUE);
  int SearchPrev(int flags, const char* text, BOOL bDirect = TRUE);
  int LinesOnScreen(BOOL bDirect = TRUE);
  void UsePopUp(BOOL allowPopUp, BOOL bDirect = TRUE);
  BOOL SelectionIsRectangle(BOOL bDirect = TRUE);
  void SetZoom(int zoom, BOOL bDirect = TRUE);
  int GetZoom(BOOL bDirect = TRUE);
  int CreateDocument(BOOL bDirect = TRUE);
  void AddRefDocument(int doc, BOOL bDirect = TRUE);
  void ReleaseDocument(int doc, BOOL bDirect = TRUE);
  int GetModEventMask(BOOL bDirect = TRUE);
  void SCISetFocus(BOOL focus, BOOL bDirect = TRUE);
  BOOL GetFocus(BOOL bDirect = TRUE);
  void SetStatus(int statusCode, BOOL bDirect = TRUE);
  int GetStatus(BOOL bDirect = TRUE);
  void SetMouseDownCaptures(BOOL captures, BOOL bDirect = TRUE);
  BOOL GetMouseDownCaptures(BOOL bDirect = TRUE);
  void SetCursor(int cursorType, BOOL bDirect = TRUE);
  int GetCursor(BOOL bDirect = TRUE);
  void SetControlCharSymbol(int symbol, BOOL bDirect = TRUE);
  int GetControlCharSymbol(BOOL bDirect = TRUE);
  void WordPartLeft(BOOL bDirect = TRUE);
  void WordPartLeftExtend(BOOL bDirect = TRUE);
  void WordPartRight(BOOL bDirect = TRUE);
  void WordPartRightExtend(BOOL bDirect = TRUE);
  void SetVisiblePolicy(int visiblePolicy, int visibleSlop, BOOL bDirect = TRUE);
  void DelLineLeft(BOOL bDirect = TRUE);
  void DelLineRight(BOOL bDirect = TRUE);
  void SetXOffset(int newOffset, BOOL bDirect = TRUE);
  int GetXOffset(BOOL bDirect = TRUE);
  void ChooseCaretX(BOOL bDirect = TRUE);
  void GrabFocus(BOOL bDirect = TRUE);
  void SetXCaretPolicy(int caretPolicy, int caretSlop, BOOL bDirect = TRUE);
  void SetYCaretPolicy(int caretPolicy, int caretSlop, BOOL bDirect = TRUE);
  void SetPrintWrapMode(int mode, BOOL bDirect = TRUE);
  int GetPrintWrapMode(BOOL bDirect = TRUE);
  void SetHotspotActiveFore(BOOL useSetting, COLORREF fore, BOOL bDirect = TRUE);
  COLORREF GetHotspotActiveFore(BOOL bDirect = TRUE);  
  void SetHotspotActiveBack(BOOL useSetting, COLORREF back, BOOL bDirect = TRUE);
  COLORREF GetHotspotActiveBack(BOOL bDirect = TRUE);  
  void SetHotspotActiveUnderline(BOOL underline, BOOL bDirect = TRUE);
  BOOL GetHotspotActiveUnderline(BOOL bDirect = TRUE);
  void SetHotspotSingleLine(BOOL singleLine, BOOL bDirect = TRUE);
  BOOL GetHotspotSingleLine(BOOL bDirect = TRUE);
  void ParaDown(BOOL bDirect = TRUE);
  void ParaDownExtend(BOOL bDirect = TRUE);
  void ParaUp(BOOL bDirect = TRUE);
  void ParaUpExtend(BOOL bDirect = TRUE);
  long PositionBefore(long pos, BOOL bDirect = TRUE);
  long PositionAfter(long pos, BOOL bDirect = TRUE);
  void CopyRange(long start, long end, BOOL bDirect = TRUE);
  void CopyText(int length, const char* text, BOOL bDirect = TRUE);
  void SetSelectionMode(int mode, BOOL bDirect = TRUE);
  int GetSelectionMode(BOOL bDirect = TRUE);
  long GetLineSelStartPosition(int line, BOOL bDirect = TRUE);
  long GetLineSelEndPosition(int line, BOOL bDirect = TRUE);
  void LineDownRectExtend(BOOL bDirect = TRUE);
  void LineUpRectExtend(BOOL bDirect = TRUE);
  void CharLeftRectExtend(BOOL bDirect = TRUE);
  void CharRightRectExtend(BOOL bDirect = TRUE);
  void HomeRectExtend(BOOL bDirect = TRUE);
  void VCHomeRectExtend(BOOL bDirect = TRUE);
  void LineEndRectExtend(BOOL bDirect = TRUE);
  void PageUpRectExtend(BOOL bDirect = TRUE);
  void PageDownRectExtend(BOOL bDirect = TRUE);
  void StutteredPageUp(BOOL bDirect = TRUE);
  void StutteredPageUpExtend(BOOL bDirect = TRUE);
  void StutteredPageDown(BOOL bDirect = TRUE);
  void StutteredPageDownExtend(BOOL bDirect = TRUE);
  void WordLeftEnd(BOOL bDirect = TRUE);
  void WordLeftEndExtend(BOOL bDirect = TRUE);
  void WordRightEnd(BOOL bDirect = TRUE);
  void WordRightEndExtend(BOOL bDirect = TRUE);
  void SetWhitespaceChars(const char* characters, BOOL bDirect = TRUE);
  void SetCharsDefault(BOOL bDirect = TRUE);
  int AutoCGetCurrent(BOOL bDirect = TRUE);
  void Allocate(int bytes, BOOL bDirect = TRUE);
  int TargetAsUTF8(char* s, BOOL bDirect = TRUE);
  void SetLengthForEncode(int bytes, BOOL bDirect = TRUE);
  int EncodedFromUTF8(const char* utf8, char* encoded, BOOL bDirect = TRUE);
  int FindColumn(int line, int column, BOOL bDirect = TRUE);
  BOOL GetCaretSticky(BOOL bDirect = TRUE);
  void SetCaretSticky(BOOL useCaretStickyBehaviour, BOOL bDirect = TRUE);
  void ToggleCaretSticky(BOOL bDirect = TRUE);
  void SetPasteConvertEndings(BOOL convert, BOOL bDirect = TRUE);
  BOOL GetPasteConvertEndings(BOOL bDirect = TRUE);
  void SelectionDuplicate(BOOL bDirect = TRUE);
  void SetCaretLineBackAlpha(int alpha, BOOL bDirect = TRUE);
  int GetCaretLineBackAlpha(BOOL bDirect = TRUE);
  void SetCaretStyle(int caretStyle, BOOL bDirect = TRUE);
  int GetCaretStyle(BOOL bDirect = TRUE);
  void SetIndicatorCurrent(int indicator, BOOL bDirect = TRUE);
  int GetIndicatorCurrent(BOOL bDirect = TRUE);
  void SetIndicatorValue(int value, BOOL bDirect = TRUE);
  int GetIndicatorValue(BOOL bDirect = TRUE);
  void IndicatorFillRange(int position, int fillLength, BOOL bDirect = TRUE);
  void IndicatorClearRange(int position, int clearLength, BOOL bDirect = TRUE);
  int IndicatorAllOnFor(int position, BOOL bDirect = TRUE);
  int IndicatorValueAt(int indicator, int position, BOOL bDirect = TRUE);
  int IndicatorStart(int indicator, int position, BOOL bDirect = TRUE);
  int IndicatorEnd(int indicator, int position, BOOL bDirect = TRUE);
  void SetPositionCache(int size, BOOL bDirect = TRUE);
  int GetPositionCache(BOOL bDirect = TRUE);
  void CopyAllowLine(BOOL bDirect = TRUE);
  //LRESULT GetCharacterPointer(BOOL bDirect = TRUE);
  //void SetKeysUnicode(BOOL keysUnicode, BOOL bDirect = TRUE);
  //BOOL GetKeysUnicode(BOOL bDirect = TRUE);
  void StartRecord(BOOL bDirect = TRUE);
  void StopRecord(BOOL bDirect = TRUE);
  void SetLexer(int lexer, BOOL bDirect = TRUE);
  int GetLexer(BOOL bDirect = TRUE);
  void Colourise(long start, long end, BOOL bDirect = TRUE);
  void SetProperty(const char* key, const char* value, BOOL bDirect = TRUE);
  void SetKeyWords(int keywordSet, const char* keyWords, BOOL bDirect = TRUE);
  void SetLexerLanguage(const char* language, BOOL bDirect = TRUE);
  void LoadLexerLibrary(const char* path, BOOL bDirect = TRUE);
  int GetProperty(const char* key, char* buf, BOOL bDirect = TRUE);
  int GetPropertyExpanded(const char* key, char* buf, BOOL bDirect = TRUE);
  int GetPropertyInt(const char* key, BOOL bDirect = TRUE);
  int GetStyleBitsNeeded(BOOL bDirect = TRUE);

protected:
  DECLARE_DYNAMIC(CScintillaCtrl)

//Member variables
  LRESULT m_DirectFunction;
  LRESULT m_DirectPointer;
};

#endif //__SCINTILLACTRL_H__

