#include "stdafx.h"
#include "CPreviewFileDialog.h"
#include "DoPreview.h"

void CDoPreview::SetPreviewFile(const CString& csFileName)
{
	m_csFileName=csFileName;
}

void CDoPreview::DrawPreview(CDC *pDC,int X,int Y,int width,int height)
{
}