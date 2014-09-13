//********************************************
// Texture.cpp
//********************************************
// class CTexture
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 17/12/97
// Modified : 19/12/97
//********************************************

#include "..\stdafx.h"

//////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////

//********************************************
// Constructor
//********************************************
CTexture::CTexture()
{
	m_pData = NULL;
	m_Width = 0;
	m_WidthByte32 = 0;
	m_Height = 0;
	m_Depth = 0;
	m_FileName = _T("");
}

//********************************************
// Destructor
//********************************************
CTexture::~CTexture()
{
	Free();
}


//////////////////////////////////////////////
// DATAS
//////////////////////////////////////////////

//********************************************
// Alloc
//********************************************
int CTexture::Alloc(unsigned int width,
										unsigned int height,
										unsigned int depth)
{
	Free();

	unsigned int BytePerPixel = (unsigned int)(depth / 8);
	unsigned int Width32 = WidthByte32(width,depth);

	// Only rgb and rgba modes
	ASSERT(BytePerPixel == 3 || 
		     BytePerPixel == 4);

	m_pData = new unsigned char [Width32 * height];
	if(m_pData == NULL)
		{
		TRACE("CTexture::Alloc : Insuffisant memory\n");
		AfxMessageBox("CTexture::Alloc : Insufisant memory");
		return 0;
		}

	// Set members variables
	m_Width = width;
	m_WidthByte32 = Width32;
	m_Height = height;
	m_Depth = depth;
	UpdateHeader();

	return 1;
}

//********************************************
// Free
//********************************************
void CTexture::Free()
{
	if(m_pData != NULL)
		{
		delete [] m_pData;
		m_pData = NULL;
		}
	m_Width = 0;
	m_Height = 0;
	m_Depth = 0;
}


//////////////////////////////////////////////
//////////////////////////////////////////////
// FILE READING
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// ReadFile
// Redirection
//********************************************
int CTexture::ReadFile(char *filename,
											 unsigned int width,
											 unsigned int height,
											 unsigned int depth)
{
	// Cleanup
	Free();

	// Storage
	m_FileName = filename;

	// Extension
	CString string = filename;
	string.MakeLower();
	TRACE("CTexture::ReadFile : file : %s\n",filename);
	CString extension = string.Right(4);

	// Redirection BMP
	if(extension == ".bmp")
		return ReadFileBMP(filename);


	// Redirection RAW
	if(extension == ".raw")
		return ReadFileRAW(filename,width,height,depth);

	// Unrecognized file format
	CString message;
	message.Format("CTexture::ReadFile : invalid file redirection : %s\n",filename);
	AfxMessageBox(string);

	return 0;
}


//********************************************
// ReadFileBMP (*.bmp)
//********************************************
// Read windows bmp files
// Accept only 24 bits
// Size : 2^n x 2^m
//********************************************
int CTexture::ReadFileBMP(char *filename)
{

	// Check for valid bmp file
	CFile file;
	CFileException ex;
	
	// Try to open file
	if(!file.Open(filename, CFile::modeRead | CFile::typeBinary,&ex))
	{
		#ifdef _DEBUG
		  afxDump << "File could not be opened " << ex.m_cause << "\n";
		#endif
		AfxMessageBox("Unable to open file for reading");
		return 0;
	}

	// File header
	BITMAPFILEHEADER FileHeader;
	TRY
	{
		file.Read(&FileHeader,sizeof(BITMAPFILEHEADER));
	}
	CATCH(CFileException, e)
	{
		#ifdef _DEBUG
				afxDump << "Error during reading " << e->m_cause << "\n";
		#endif
		AfxMessageBox("Error during reading file header");
		file.Close();
		return 0;
	}
	END_CATCH

	TRACE("FileHeader.bfType : %d\n",FileHeader.bfType);
	TRACE("FileHeader.bfSize : %d\n",FileHeader.bfSize);
	TRACE("FileHeader.bfReserved1 : %d\n",FileHeader.bfReserved1);
	TRACE("FileHeader.bfReserved2 : %d\n",FileHeader.bfReserved2);
	TRACE("FileHeader.bfOffBits : %d\n",FileHeader.bfOffBits);

	// Is it a Windows BMP file ? (BM)
  WORD sign = ((WORD) ('M' << 8) | 'B');
	if(FileHeader.bfType != sign)
	{
		AfxMessageBox("Invalid BMP file");
		file.Close();
		return 0;
	}

	// Image header
	TRY
	{
		file.Read(&m_Header,sizeof(BITMAPINFOHEADER));
	}
	CATCH(CFileException, e)
	{
		#ifdef _DEBUG
				afxDump << "Error during reading " << e->m_cause << "\n";
		#endif
		AfxMessageBox("Error during reading image header");
		file.Close();
		return 0;
	}
	END_CATCH

	// DEBUG
	TRACE("\n");
	TRACE("IMAGE HEADER :\n");
	TRACE("biSize : %d\n",m_Header.biSize);
	TRACE("biWidth : %d\n",m_Header.biWidth);
	TRACE("biHeight : %d\n",m_Header.biHeight);
	TRACE("biPlanes : %d\n",m_Header.biPlanes);
	TRACE("biBitCount : %d\n",m_Header.biBitCount);
	TRACE("biCompression : %d\n",m_Header.biCompression);
	TRACE("biSizeImage : %d\n",m_Header.biSizeImage);
	TRACE("biXPelsPerMeter : %d\n",m_Header.biXPelsPerMeter);
	TRACE("biYPelsPerMeter : %d\n",m_Header.biYPelsPerMeter);
	TRACE("biClrUsed : %d\n",m_Header.biClrUsed);
	TRACE("biClrImportant : %d\n",m_Header.biClrImportant);

	// 24 bits ?
	if(m_Header.biPlanes != 1 ||
		 m_Header.biBitCount != 24)
	{
		AfxMessageBox("Texture file must have 24 bits depth");
		file.Close();
		return 0;
	}

	// Alloc (does call Free before)
	Free();
	m_pData = new unsigned char[m_Header.biSizeImage];
	if(m_pData == NULL)
	{
		AfxMessageBox("Insuffisant memory");
		file.Close();
		return 0;
	}

	// Update datas
	m_Width = m_Header.biWidth;
	m_Height = m_Header.biHeight;
	m_Depth = m_Header.biBitCount;

	// Image reading
	TRY
	{
		file.Read(m_pData,m_Header.biSizeImage);
	}
	CATCH(CFileException, e)
	{
		#ifdef _DEBUG
				afxDump << "Error during reading " << e->m_cause << "\n";
		#endif
		AfxMessageBox("Error during reading image");
		file.Close();
		return 0;
	}
	END_CATCH

	// Close file
  file.Close();

	// Success, also set FileName
	m_FileName = filename;

	UpdateWidthByte32();

	return 1;
}

//********************************************
// UpdateWidthByte32
//********************************************
void CTexture::UpdateWidthByte32()
{
	m_WidthByte32 = WidthByte32(m_Width,m_Depth);
}

//********************************************
// WidthByte32
//********************************************
unsigned int CTexture::WidthByte32(unsigned int width,
																	 unsigned int depth)
{
	// 32 bits alignment (4 bytes)
	int rest=(width*depth/8)%4;
	if(rest != 0)
		return (width*depth/8 + 4-rest);
	else
		return (width*depth/8);
}

//********************************************
// UpdateHeader
//********************************************
void CTexture::UpdateHeader()
{
	UpdateWidthByte32();

	m_Header.biWidth = m_Width;
	m_Header.biHeight = m_Height;
	m_Header.biSizeImage = m_WidthByte32 * m_Height;

	m_Header.biSize = 40;
	m_Header.biPlanes = 1;
	m_Header.biBitCount = m_Depth;
	m_Header.biCompression = 0;
	m_Header.biXPelsPerMeter = 0;
	m_Header.biYPelsPerMeter = 0;
	m_Header.biClrUsed = 0;
	m_Header.biClrImportant = 0;
}


//********************************************
// ReadFileRAW (*.raw)
//********************************************
// Read raw files
// Accept only 24 or 32 bits
// Size : 2^n x 2^m
//********************************************
int CTexture::ReadFileRAW(char *filename,
													unsigned int width,
													unsigned int height,
													unsigned int depth)
{

	ASSERT(width>0);
	ASSERT(height>0);
	ASSERT(depth>0);
	ASSERT(depth%8==0);
	ASSERT(depth/8==3 || depth/8==4);

	// Check for valid file
	CFile file;
	CFileException ex;
	
	// Try to open file
	if(!file.Open(filename, CFile::modeRead | CFile::typeBinary,&ex))
	{
		#ifdef _DEBUG
		  afxDump << "File could not be opened " << ex.m_cause << "\n";
		#endif
		AfxMessageBox("Unable to open file for reading");
		return 0;
	}

	// Alloc (does call Free before)
	if(!Alloc(width,height,depth))
	{
		AfxMessageBox("Insuffisant memory");
		file.Close();
		return 0;
	}

	// Image reading
	TRY
	{
		file.Read(m_pData,m_Width*m_Height*depth/8);
	}
	CATCH(CFileException, e)
	{
		#ifdef _DEBUG
				afxDump << "Error during reading " << e->m_cause << "\n";
		#endif
		AfxMessageBox("Error during reading image");
		file.Close();
		return 0;
	}
	END_CATCH

	// Close file
  file.Close();

	// Success, also set FileName
	m_FileName = filename;

	return 1;
}

//////////////////////////////////////////////
//////////////////////////////////////////////
// FILE SAVING
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// SaveFile
// Redirection
//********************************************
int CTexture::SaveFile(char *filename)
{
	CString string = filename;
	string.MakeLower();
	TRACE("CTexture::SaveFile : file : %s\n",filename);

	CString extension = string.Right(4);

	// Redirection RAW
	if(extension == ".raw")
		return SaveFileRAW(filename);

	// Redirection BMP
	if(extension == ".bmp")
		return SaveFileBMP(filename);

	// Unrecognized file format
	CString message;
	message.Format("CTexture::SaveFile : invalid file redirection : %s\n",filename);
	AfxMessageBox(message);

	return 0;
}


//********************************************
// SaveFileRAW
//********************************************
int CTexture::SaveFileRAW(char *filename)
{
	// Check for valid image
	if((m_Width * m_Height * m_Depth) == 0)
		{
		AfxMessageBox("CTexture::SaveFileRAW : invalid image");
		return 0;
		}

	// Check for valid file
	CFile file;
	CFileException ex;
	
	// Try to open file
	if(!file.Open(filename, CFile::modeWrite | CFile::typeBinary,&ex))
	{
		#ifdef _DEBUG
		  afxDump << "File could not be opened " << ex.m_cause << "\n";
		#endif
		AfxMessageBox("Unable to open file for writing");
		return 0;
	}

	// Image writing
	TRY
	{
		file.Write(m_pData,m_Width*m_Height*m_Depth/8);
	}
	CATCH(CFileException, e)
	{
		#ifdef _DEBUG
				afxDump << "Error during writing " << e->m_cause << "\n";
		#endif
		AfxMessageBox("Error during writing image");
		file.Close();
		return 0;
	}
	END_CATCH

	// Close file
  file.Close();

	return 1;
}


//********************************************
// SaveFileBMP (*.bmp)
//********************************************
// Save windows bmp files
// Accept only 24 bits
//********************************************
int CTexture::SaveFileBMP(char *filename)
{
	if(!IsValid())
		return 0;

	if(m_Depth != 24)
		return 0;

	// Check for valid bmp file
	CFile file;
	CFileException ex;
	
	// Try to open file
	if(!file.Open(filename,CFile::modeCreate | CFile::modeWrite | CFile::typeBinary,&ex))
	{
		#ifdef _DEBUG
		  afxDump << "File could not be opened " << ex.m_cause << "\n";
		#endif
		AfxMessageBox("Unable to open file for writing");
		return 0;
	}

	// File header
	BITMAPFILEHEADER FileHeader;
  WORD sign = ((WORD) ('M' << 8) | 'B');
	FileHeader.bfType = sign;
  FileHeader.bfSize = 14 + 40 + m_WidthByte32*m_Height; 
  FileHeader.bfReserved1 = 0; 
  FileHeader.bfReserved2 = 0; 
  FileHeader.bfOffBits = 54; 

	TRACE("\nSave BMP File...\n");
	TRACE("FileHeader.bfType : %d\n",FileHeader.bfType);
	TRACE("FileHeader.bfSize : %d\n",FileHeader.bfSize);
	TRACE("FileHeader.bfReserved1 : %d\n",FileHeader.bfReserved1);
	TRACE("FileHeader.bfReserved2 : %d\n",FileHeader.bfReserved2);
	TRACE("FileHeader.bfOffBits : %d\n",FileHeader.bfOffBits);

	TRY
	{
		file.Write(&FileHeader,sizeof(BITMAPFILEHEADER));
	}
	CATCH(CFileException, e)
	{
		#ifdef _DEBUG
				afxDump << "Error during writing " << e->m_cause << "\n";
		#endif
		AfxMessageBox("Error during writing file header");
		file.Close();
		return 0;
	}
	END_CATCH

	// Image header
	TRY
	{
		file.Write(&m_Header,sizeof(BITMAPINFOHEADER));
	}
	CATCH(CFileException, e)
	{
		#ifdef _DEBUG
				afxDump << "Error during writing " << e->m_cause << "\n";
		#endif
		AfxMessageBox("Error during writing image header");
		file.Close();
		return 0;
	}
	END_CATCH

	// DEBUG
	TRACE("\n");
	TRACE("IMAGE HEADER :\n");
	TRACE("biSize : %d\n",m_Header.biSize);
	TRACE("biWidth : %d\n",m_Header.biWidth);
	TRACE("biHeight : %d\n",m_Header.biHeight);
	TRACE("biPlanes : %d\n",m_Header.biPlanes);
	TRACE("biBitCount : %d\n",m_Header.biBitCount);
	TRACE("biCompression : %d\n",m_Header.biCompression);
	TRACE("biSizeImage : %d\n",m_Header.biSizeImage);
	TRACE("biXPelsPerMeter : %d\n",m_Header.biXPelsPerMeter);
	TRACE("biYPelsPerMeter : %d\n",m_Header.biYPelsPerMeter);
	TRACE("biClrUsed : %d\n",m_Header.biClrUsed);
	TRACE("biClrImportant : %d\n",m_Header.biClrImportant);

	// Image writing
	TRY
	{
		file.Write(m_pData,m_Header.biSizeImage);
	}
	CATCH(CFileException, e)
	{
		#ifdef _DEBUG
				afxDump << "Error during writing " << e->m_cause << "\n";
		#endif
		AfxMessageBox("Error during writing image");
		file.Close();
		return 0;
	}
	END_CATCH

	// Close file
  file.Close();

	return 1;
}




//////////////////////////////////////////////
//////////////////////////////////////////////
// CHECKING
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// IsValid
//********************************************
int CTexture::IsValid()
{
	int success = 0;
	success = (m_Depth == 24) || (m_Depth == 32);
	success &= (m_Width != 0);
	success &= (m_Height != 0);
	success &= (m_pData != NULL);
	if(!success)
		{
		TRACE("\n");
		TRACE("Invalid texture\n");
		TRACE("Width  : %d\n",m_Width);
		TRACE("Height : %d\n",m_Height);
		TRACE("Depth  : %d\n",m_Depth);
		TRACE("Data   : %x\n",m_pData);
		}
	return success;
}


//********************************************
// HigherPowerOfTwo
//********************************************
int CTexture::HigherPowerOfTwo(int value)
{
	ASSERT(value > 0);
	if(value <= 0)
		return value;

	int power = 1;
	int x = 0;

	while(1)
	{
		x = (int)pow(2.0,power);
		if(x >= value)
			return x;
		power++;
	}

	return value;
}

//********************************************
// LowerPowerOfTwo
//********************************************
int CTexture::LowerPowerOfTwo(int value)
{
	ASSERT(value > 0);
	if(value <= 0)
		return value;

	int power = 1;
	int x = 0;

	while(1)
	{
		x = (int)pow(2.0,power);
		if(x >= value)
			return (int)pow(2.0,power-1);
		power++;
	}

	return value;
}

//********************************************
// SameSize
//********************************************
int CTexture::SameSize(CTexture *pTexture)
{
	int success = (m_Width == pTexture->GetWidth());
	success &= (m_Height == pTexture->GetHeight());
	return success;
}


//********************************************
// Flip BGR to RGB
//********************************************
int CTexture::BGRtoRGB(void)
{
	if(!IsValid())
		return 0;

	unsigned char pixel;
	int BytePerPixel = m_Depth/8;
	for(unsigned int j=0;j<m_Height;j++)
		for(unsigned int i=0;i<m_Width;i++)
		{
			pixel = m_pData[m_WidthByte32*j+i*BytePerPixel+2];
			m_pData[m_WidthByte32*j+i*BytePerPixel+2] = m_pData[m_WidthByte32*j+i*BytePerPixel];
			m_pData[m_WidthByte32*j+i*BytePerPixel] = pixel;
		}
	return 1;
}

//////////////////////////////////////////////
//////////////////////////////////////////////
// DUPLICATE
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// Extract
//********************************************
int CTexture::Extract(int left,
											int top,
											int right,
											int bottom)
{
	ASSERT(IsValid());

	// Saturate
	if(right == -1)
		right = m_Width-1;
	if(bottom == -1)
		bottom = m_Height-1;

	// Check
	if(left >= right || top >= bottom)
		return 0;
	if(left < 0  || left >= (int)m_Width || 
		 right < 0 || right >= (int)m_Width)
		return 0;
	if(top < 0  || top >= (int)m_Height || 
		 bottom < 0 || bottom >= (int)m_Height)
		return 0;

	int NewWidth = right-left+1;
	int NewWidthByte32 = WidthByte32(NewWidth,m_Depth);
	int NewHeight = bottom-top+1;
	int BytePerPixel = m_Depth / 8;
	int i,j,k;

	TRACE("Start extracting...\n");
	TRACE("New width : %d\n",NewWidth);
	TRACE("New height : %d\n",NewHeight);

	ASSERT(NewWidth>=1);
	ASSERT(NewHeight>=1);

	// Alloc
	unsigned char *pData = new unsigned char[NewWidthByte32*NewHeight];
	if(pData == NULL)
		{
		AfxMessageBox("Insuffisant memeory");
		return 0;
		}

	for(j=0;j<NewHeight;j++)
		for(i=0;i<NewWidth;i++)
			for(k=0;k<BytePerPixel;k++)
				pData[NewWidthByte32*j+i*BytePerPixel+k] = m_pData[m_WidthByte32*(m_Height-1-(j+top))+(i+left)*BytePerPixel+k];

	// Replace datas
	delete [] m_pData;
	m_pData = pData;
	m_Width = NewWidth;
	m_WidthByte32 = NewWidthByte32;
	m_Height = NewHeight;

	UpdateHeader();

	return 1;
}


//********************************************
// DuplicateMirror
//********************************************
int CTexture::DuplicateMirror(int left,
															int top,
															int right,
															int bottom)
{

	if(!Extract(left,top,right,bottom))
		return 0;

	left = 0;
	right = m_Width-1;
	top = 0;
	bottom = m_Height-1;

	int NewWidth = 2*m_Width;
	int NewWidthByte32 = WidthByte32(NewWidth,m_Depth);
	int NewHeight = 2*m_Height;
	int BytePerPixel = m_Depth / 8;
	int i,j,k;

	TRACE("Start duplicate mirror...\n");
	TRACE("New width : %d\n",NewWidth);
	TRACE("New widthbyte32 : %d\n",NewWidthByte32);
	TRACE("New height : %d\n",NewHeight);

	ASSERT(NewWidth>=1);
	ASSERT(NewHeight>=1);

	// Alloc
	unsigned char *pData = new unsigned char[NewWidthByte32*NewHeight];
	if(pData == NULL)
		{
		AfxMessageBox("Insuffisant memeory");
		return 0;
		}

	// o o
	// x o
	for(j=0;j<NewHeight/2;j++)
		for(i=0;i<NewWidth/2;i++)
			for(k=0;k<BytePerPixel;k++)
				pData[NewWidthByte32*j+i*BytePerPixel+k] = m_pData[m_WidthByte32*(bottom-(j+top))+(i+left)*BytePerPixel+k];
	// o o
	// o x
	for(j=0;j<NewHeight/2;j++)
		for(i=NewWidth/2;i<NewWidth;i++)
			for(k=0;k<BytePerPixel;k++)
				pData[NewWidthByte32*j+i*BytePerPixel+k] = m_pData[m_WidthByte32*(bottom-(j+top))+(right-(i-NewWidth/2+left))*BytePerPixel+k];
	// x o
	// o o
	for(j=NewHeight/2;j<NewHeight;j++)
		for(i=0;i<NewWidth/2;i++)
			for(k=0;k<BytePerPixel;k++)
				pData[NewWidthByte32*j+i*BytePerPixel+k] = m_pData[m_WidthByte32*(j-NewHeight/2+top)+(i+left)*BytePerPixel+k];
	// o x
	// o o
	for(j=NewHeight/2;j<NewHeight;j++)
		for(i=NewWidth/2;i<NewWidth;i++)
			for(k=0;k<BytePerPixel;k++)
				pData[NewWidthByte32*j+i*BytePerPixel+k] = m_pData[m_WidthByte32*(j-NewHeight/2+top)+(right-(i-NewWidth/2+left))*BytePerPixel+k];

	// Replace datas
	delete [] m_pData;
	m_pData = pData;
	m_Width = NewWidth;
	m_WidthByte32 = NewWidthByte32;
	m_Height = NewHeight;

	UpdateHeader();

	return 1;
}




//********************************************
// DuplicateRepeatWidth
//********************************************
int CTexture::DuplicateRepeatWidth(int left,
																	 int top,
																	 int right,
																	 int bottom)
{
	if(!Extract(left,top,right,bottom))
		return 0;

	left = 0;
	right = m_Width-1;
	top = 0;
	bottom = m_Height-1;

	int NewWidth = 2*m_Width;
	int NewWidthByte32 = WidthByte32(NewWidth,m_Depth);
	int NewHeight = m_Height;
	int BytePerPixel = m_Depth / 8;
	int i,j,k;

	TRACE("Start duplicate mirror...\n");
	TRACE("New width : %d\n",NewWidth);
	TRACE("New widthbyte32 : %d\n",NewWidthByte32);
	TRACE("New height : %d\n",NewHeight);

	ASSERT(NewWidth>=1);
	ASSERT(NewHeight>=1);

	// Alloc
	unsigned char *pData = new unsigned char[NewWidthByte32*NewHeight];
	if(pData == NULL)
		{
		AfxMessageBox("Insuffisant memeory");
		return 0;
		}

	// x o
	for(j=0;j<NewHeight;j++)
		for(i=0;i<NewWidth/2;i++)
			for(k=0;k<BytePerPixel;k++)
				pData[NewWidthByte32*j+i*BytePerPixel+k] = m_pData[m_WidthByte32*(bottom-(j+top))+(i+left)*BytePerPixel+k];
	// o x
	for(j=0;j<NewHeight;j++)
		for(i=NewWidth/2;i<NewWidth;i++)
			for(k=0;k<BytePerPixel;k++)
				pData[NewWidthByte32*j+i*BytePerPixel+k] = m_pData[m_WidthByte32*(bottom-(j+top))+(i-NewWidth/2+left)*BytePerPixel+k];

	// Replace datas
	delete [] m_pData;
	m_pData = pData;
	m_Width = NewWidth;
	m_WidthByte32 = NewWidthByte32;
	m_Height = NewHeight;

	UpdateHeader();

	return 1;
}





//////////////////////////////////////////////
//////////////////////////////////////////////
// ALPHA
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// SetAlphaLayer
//********************************************
int CTexture::SetAlphaLayer(unsigned char alpha) // 0 - 255
{
	// Check
	if(!IsValid())
		return 0;

	if(m_Depth != 32)
		return 0;

	// Fill alpha layer 
	int size = m_Width * m_Height;
	for(int i=0;i<4*size;i+=4)
		m_pData[i+3] = alpha;

	return 1;
}

//********************************************
// AddAlphaLayer
//********************************************
int CTexture::AddAlphaLayer(unsigned char alpha) // 0 - 255
{
	// Check
	if(!IsValid())
		return 0;

	// Has soon alpha
	if(HasAlpha())
		return SetAlphaLayer(alpha);

	// Alloc memory
	unsigned char *pData = new unsigned char[4*m_Width*m_Height];
	if(pData == NULL)
		{
		AfxMessageBox("CTexture::AddAlphaLayer : insuffisant memory");
		return 0;
		}

	// Fill new data
	int size = m_Width * m_Height;
	int BytePerPixel = m_Depth / 8;
	ASSERT(size > 0);
	for(int i=0;i<size;i++)
	{
		pData[4*i+0] = m_pData[BytePerPixel*i+0];
		pData[4*i+1] = m_pData[BytePerPixel*i+1];
		pData[4*i+2] = m_pData[BytePerPixel*i+2];
		pData[4*i+3] = alpha;
	}

	// Set new depth
	m_Depth = 32;

	// Replace datas
	delete [] m_pData;
	m_pData = pData;

	return 1;
}


//********************************************
// PutAlpha
// From RGB to grey scales, then alpha layer
//********************************************
int CTexture::PutAlpha(CTexture *pTexture) 
{
	// Check
	if(!IsValid())
		return 0;
	if(!pTexture->IsValid())
		return 0;

	if(!SameSize(pTexture))
		return 0;

	if(!AddAlphaLayer(0))
		return 0;

	// Fill new data
	unsigned char *pData = pTexture->GetData();
	int size = m_Width * m_Height;
	int BytePerPixel = pTexture->GetDepth() / 8;
	ASSERT(size > 0);
	for(int i=0;i<size;i++)
		m_pData[4*i+3] = (unsigned char)((int)pData[BytePerPixel*i+0]+
		                                 (int)pData[BytePerPixel*i+1]+
																		 (int)pData[BytePerPixel*i+2])/3;

	return 1;
}

//////////////////////////////////////////////
//////////////////////////////////////////////
// DISPLAY
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// Draw
//********************************************
int CTexture::Draw(CDC *pDC,
									 int xOffset,
									 int yOffset,
									 int width,
									 int height)
{
	// Checking
	if(!IsValid())
	  return 0;

	// Flags	
	if(width == -1)
		width = m_Width;
	if(height == -1)
		height = m_Height;

	// Painting
	return SetDIBitsToDevice(pDC->m_hDC,
		                       xOffset,
													 yOffset,
													 width,
													 height,
													 0,
													 0,
													 0,
													 m_Height,
													 GetData(),
													 (CONST BITMAPINFO *)&m_Header,
													 DIB_RGB_COLORS);
}

//********************************************
// ReadBuffer
//********************************************
int CTexture::ReadBuffer(unsigned char *buffer, 
												 int width, 
												 int height, 
												 int depth)
{
	if(buffer == NULL)
		return 0;

	if(!Alloc(width,height,depth))
		return 0;

	int BytePerPixel = depth / 8;

	for(int j=0;j<height;j++)
		for(int i=0;i<width;i++)
			for(int k=0;k<BytePerPixel;k++)
			{
				m_pData[m_WidthByte32*j + i*BytePerPixel+k] = 
				  buffer[(width*j+i)*BytePerPixel+k];
				//TRACE("pixel : %d\n",(int)buffer[(width*j+i)*BytePerPixel+k]);
			}

	return 1;
}

//********************************************
// ReadBuffer
//********************************************
int CTexture::ReadBuffer(float *buffer, 
												 int width, 
												 int height, 
												 int depth)
{
	if(buffer == NULL)
		return 0;

	if(!Alloc(width,height,depth))
		return 0;

	int BytePerPixel = depth / 8;

	for(int j=0;j<height;j++)
		for(int i=0;i<width;i++)
			for(int k=0;k<BytePerPixel;k++)
			{
				ASSERT(buffer[(width*j+i)*BytePerPixel+k] <= 1.0f);
				ASSERT(buffer[(width*j+i)*BytePerPixel+k] >= 0.0f);
				m_pData[m_WidthByte32*j + i*BytePerPixel+k] = (BYTE)(255.0f * buffer[(width*j+i)*BytePerPixel+k]);
			}

	return 1;
}

//********************************************
// Grey
//********************************************
int CTexture::Grey(unsigned int x,
									 unsigned int y)
{
	ASSERT(x<m_Width && x>=0);
	ASSERT(y<m_Height && y>=0);
	int BytePerPixel = m_Depth / 8;
	// Grey scale
	if(BytePerPixel == 1)
		return (int)m_pData[m_WidthByte32*y + x]; 
	else // 24 or 32 bits (alpha layer)
		return (int)((int)m_pData[m_WidthByte32*y + x*BytePerPixel+0]+ 
		             (int)m_pData[m_WidthByte32*y + x*BytePerPixel+1]+
								 (int)m_pData[m_WidthByte32*y + x*BytePerPixel+2])/3;
}

//********************************************
// Color
//********************************************
void CTexture::Color(unsigned int x,
 									   unsigned int y, 
										 unsigned char *pRed,
										 unsigned char *pGreen,
										 unsigned char *pBlue)
{
	ASSERT(x<m_Width && x>=0);
	ASSERT(y<m_Height && y>=0);
	int BytePerPixel = m_Depth / 8;
	// Grey scale
	if(BytePerPixel == 1)
	{
		*pRed = m_pData[m_WidthByte32*y + x]; 
		*pGreen = m_pData[m_WidthByte32*y + x]; 
		*pBlue = m_pData[m_WidthByte32*y + x]; 
	}
	else // 24 or 32 bits (alpha layer)
	{
		*pRed = m_pData[m_WidthByte32*y + x*BytePerPixel]; 
		*pGreen = m_pData[m_WidthByte32*y + x*BytePerPixel+1]; 
		*pBlue = m_pData[m_WidthByte32*y + x*BytePerPixel+2]; 
	}
}


HANDLE CTexture::ExportHandle()
{
	HANDLE handle;

	TRACE("Export handle...");
  // Process source handle size
  int size = sizeof(BITMAPINFOHEADER) + m_WidthByte32 * m_Height;
  // Alloc memory
	TRACE("alloc...");
  handle = (HANDLE)::GlobalAlloc (GHND,size);
  if(handle != NULL)
	{
		char *pData = (char *) ::GlobalLock((HGLOBAL)handle);
		TRACE("lock...");
		// Copy header
		TRACE("header...");
		memcpy(pData,&m_Header,sizeof(BITMAPINFOHEADER));
		// Copy datas
		TRACE("datas...");
		memcpy(pData+sizeof(BITMAPINFOHEADER),m_pData,m_WidthByte32*m_Height);
		// Unlock
		TRACE("unlock...");
		::GlobalUnlock((HGLOBAL)handle);
	}
	TRACE("ok\n");
	return handle;
}



// ** EOF **



