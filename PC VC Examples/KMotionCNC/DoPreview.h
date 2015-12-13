#pragma once


class CDoPreview : public CAbstractPreview
{
public:
	virtual void SetPreviewFile(const CString& csFileName);
	virtual void DrawPreview(CDC *pDC,int x,int y,int width,int height);
};