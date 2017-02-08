// MyDib.cpp: implementation of the CDib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Dib.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/**
 * Dib�ļ�ͷ��־���ַ���"BM"��
 */
#define DIB_MARKER   ((WORD) ('M' << 8) | 'B') // �����ж�λͼ�ı�־�� 



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDib::CDib()
{
	InitMembers();
}

CDib::~CDib()
{
	ClearMemory();
}

/*************************************************************************
* �������ƣ�DrawDib()
* ����˵��:
*   HDC hDC������豸DC
*   LPRECT lpDCRect�����ƾ�������
*   HGLOBAL hDIB ��DIB����ľ��
*   LPRECT lpDIBRect ��DIB���������
*   CPalette* pPal����ɫ���ָ��
* ��������:BOOL
* ��������:�ú�����Ҫ��������DIB����   
************************************************************************/
BOOL CDib::DrawDib(HDC     hDC,
				   LPRECT  lpDCRect,
				   HGLOBAL    hDIB,
				   LPRECT  lpDIBRect,
				   CPalette* pPal)
{
	BOOL     bSuccess=FALSE;      // �ػ��ɹ���־
	HPALETTE hOldPal=NULL;        // ��ǰ�ĵ�ɫ��	
	if (hDIB == NULL)// �ж��Ƿ�����Ч��DIB����
	{		
		return FALSE;// ���ǣ��򷵻�
	}

	CDC memdc;
	CDC *pDC = CDC::FromHandle(hDC);
	memdc.CreateCompatibleDC(pDC);
	CBitmap membmp;
	CRect rect;
	GetClientRect(pDC->GetWindow()->GetSafeHwnd(),rect);
	membmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	memdc.SelectObject(&membmp);
	PatBlt(memdc, 0, 0, rect.Width(), rect.Height(), WHITENESS);
	HDC hdcmem = memdc.m_hDC;

	lpdib  = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);// ����DIB	
	lpDIBBits = GetBits(lpdib);// �ҵ�DIBͼ��������ʼλ��	
	if (pPal != NULL)// ��ȡDIB��ɫ�壬��ѡȡ���豸������
	{
		HPALETTE hPal = (HPALETTE) pPal->m_hObject;	
		hOldPal = ::SelectPalette(hdcmem, hPal, TRUE);
	}	
	::SetStretchBltMode(hdcmem, COLORONCOLOR | STRETCH_ORSCANS);// ������ʾģʽ		
	bSuccess = ::StretchDIBits(hdcmem,		  // �豸�������
		lpDCRect->left,                   //Ŀ��X����
		lpDCRect->top,                    // Ŀ��Y����
		GetRectWidth(lpDCRect),            // Ŀ����
		GetRectHeight(lpDCRect),           // Ŀ��߶�
		lpDIBRect->left,                // ԴX����
		lpDIBRect->top,                 // ԴY����
		GetRectWidth(lpDIBRect),           // Դ���
		GetRectHeight(lpDIBRect),          // Դ�߶�
		lpDIBBits,                      // ָ��dib���ص�ָ��
		(LPBITMAPINFO)lpdib,         // ָ��λͼ��Ϣ�ṹ��ָ��
		DIB_RGB_COLORS,                 // ʹ�õ���ɫ��Ŀ
		SRCCOPY);                       // ��դ��������	
	::GlobalUnlock(hDIB);	// �������
	
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memdc, 0, 0, SRCCOPY);

	if (hOldPal != NULL)// �ָ�ϵͳ��ɫ��
	{
		::SelectPalette(hDC, hOldPal, TRUE);
	}	
	membmp.DeleteObject();
	memdc.DeleteDC();
	return bSuccess;
}


/*************************************************************************
* �������ƣ�ConstructPalette(HGLOBAL hDIB, CPalette* pPal)
* ��������:
*   HGLOBAL hDIB��DIB����ľ��
*   CPalette* pPal����ɫ���ָ��
* ��������:BOOL
* ����˵��:�ú�������DIB����һ���߼���ɫ��  
************************************************************************/
BOOL CDib::ConstructPalette(HGLOBAL hDIB, CPalette* pPal)
{	

	HANDLE hLogPal;// �߼���ɫ��ľ��	
	int iLoop;// ѭ������	
	BOOL bSuccess = FALSE;// �������	
	if (hDIB == NULL)//�ж��Ƿ�����Ч��DIB����
	{		
		return FALSE;// ����FALSE
	}	
	lpdib = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);// ����DIB
	lpbminfo= (LPBITMAPINFO)lpdib;			
	long wNumColors =GetColorNum(lpdib);// ��ȡDIB����ɫ���е���ɫ��Ŀ	
	if (wNumColors != 0)
	{		
		hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE)// ����Ϊ�߼���ɫ���ڴ�
			+ sizeof(PALETTEENTRY)
			* wNumColors);				
		if (hLogPal == 0)// ���ʧ�����˳�
		{			
			::GlobalUnlock((HGLOBAL) hDIB);// �������			
			return FALSE;
		}		
		LPLOGPALETTE lpPal = (LPLOGPALETTE) ::GlobalLock((HGLOBAL) hLogPal);		

		lpPal->palVersion = PalVersion;// ���õ�ɫ��汾��				
		lpPal->palNumEntries = (WORD)wNumColors;// ������ɫ��Ŀ		
		for (iLoop=0; iLoop<(int)wNumColors;iLoop++)// ��ȡ��ɫ��
		{			
			lpPal->palPalEntry[iLoop].peRed =lpbminfo->bmiColors[iLoop].rgbRed;// ��ȡ��ԭɫ����						
			lpPal->palPalEntry[iLoop].peGreen =lpbminfo->bmiColors[iLoop].rgbGreen;							
			lpPal->palPalEntry[iLoop].peBlue =lpbminfo->bmiColors[iLoop].rgbBlue;								
			lpPal->palPalEntry[iLoop].peFlags =0;// ����λ			
		}		
		bSuccess=pPal->CreatePalette(lpPal);// �����߼���ɫ�崴����ɫ�壬������ָ��			
		::GlobalUnlock((HGLOBAL) hLogPal);// �������				
		::GlobalFree((HGLOBAL) hLogPal);// �ͷ��߼���ɫ��
	}		
	::GlobalUnlock((HGLOBAL) hDIB);// �������		
	return bSuccess;// ���ؽ��
}

/*************************************************************************
* �������ƣ�GetBits(LPSTR lpdib)
* ��������:
*   LPSTR lpdib��ָ��DIB�����ָ��
* ��������:LPSTR 
* ��������:����DIB���ص���ʼλ�ã�������ָ������ָ��
************************************************************************/

LPSTR CDib::GetBits(LPSTR lpdib)
{
	return (lpdib + ((LPBITMAPINFOHEADER)lpdib)->biSize+GetPalSize(lpdib));
	//	return (lpdib + *(LPDWORD)lpdib+GetPalSize(lpdib));
}

/*************************************************************************
* �������ƣ�GetWidth(LPSTR lpdib)
* ��������:
*   LPSTR lpdib��ָ��DIB�����ָ��
* ��������:DWORD
* ��������:�ú�������DIB��ͼ��Ŀ��
************************************************************************/

DWORD CDib::GetWidth(LPSTR lpdib)
{
	return ((LPBITMAPINFOHEADER)lpdib)->biWidth;//����DIB���
}


/*************************************************************************
* �������ƣ�GetHeight(LPSTR lpdib)
* ��������:
*   LPSTR lpdib ��ָ��DIB�����ָ��
* ��������:DWORD
* ��������:�ú�������DIB��ͼ��ĸ߶�
************************************************************************/


DWORD CDib::GetHeight(LPSTR lpdib)
{
	return ((LPBITMAPINFOHEADER)lpdib)->biHeight;//����DIB�߶�

}
/*************************************************************************
* �������ƣ�GetPalSize(LPSTR lpdib)
* ��������:
*   LPSTR lpdib��ָ��DIB�����ָ��
* ��������:WORD
* ��������:�ú�������DIB�е�ɫ��Ĵ�С
************************************************************************/

WORD CDib::GetPalSize(LPSTR lpdib)
{	
	return (WORD)(GetColorNum(lpdib) * sizeof(RGBQUAD));// ����DIB�е�ɫ��Ĵ�С
}

/*************************************************************************
* �������ƣ�GetColorNum(LPSTR lpdib)
* ��������:
*   LPSTR lpdib��ָ��DIB�����ָ��
* ��������:WORD
* ��������:�ú�������DIB�е�ɫ�����ɫ������
************************************************************************/
WORD CDib::GetColorNum(LPSTR lpdib)
{				
	long dwClrUsed = ((LPBITMAPINFOHEADER)lpdib)->biClrUsed;	// ��ȡdwClrUsedֵ	
	if (dwClrUsed != 0)
	{			
		return (WORD)dwClrUsed;// ���dwClrUsed��Ϊ0��ֱ�ӷ��ظ�ֵ
	}		
	WORD wBitCount = ((LPBITMAPINFOHEADER)lpdib)->biBitCount;// ��ȡbiBitCountֵ		
	switch (wBitCount)// �������ص�λ��������ɫ��Ŀ
	{
	case 1:
		return 2;

	case 4:
		return 16;

	case 8:
		return 256;

	default:
		return 0;
	}
}

/*************************************************************************
* �������ƣ�GetBitCount(LPSTR lpdib)
* ��������:
*   LPSTR lpdib��ָ��DIB�����ָ��
* ��������:WORD 
* ��������:�ú�������DIBBitCount
************************************************************************/
WORD CDib::GetBitCount(LPSTR lpdib)
{
	return ((LPBITMAPINFOHEADER)lpdib)->biBitCount;// ����λ��
}

/*************************************************************************
* �������ƣ�CopyObject (HGLOBAL hGlob)
* ��������:
*   HGLOBAL hGlob ��Ҫ���Ƶ��ڴ�����
* ��������:HGLOBAL
* ��������:�ú�������ָ�����ڴ�����
************************************************************************/

HGLOBAL CDib::CopyObject (HGLOBAL hGlob)
{
	if (hGlob== NULL)
		return NULL;	
	DWORD dwLen = ::GlobalSize((HGLOBAL)hGlob);// ��ȡָ���ڴ������С		
	HGLOBAL hTemp = ::GlobalAlloc(GHND, dwLen);// �������ڴ�ռ�		
	if (hTemp!= NULL)// �жϷ����Ƿ�ɹ�
	{	
		void* lpTemp = ::GlobalLock((HGLOBAL)hTemp);// ����
		void* lp     = ::GlobalLock((HGLOBAL) hGlob);				
		memcpy(lpTemp, lp, dwLen);// ����		
		::GlobalUnlock(hTemp);// �������
		::GlobalUnlock(hGlob);
	}
	return hTemp;
}

/*************************************************************************
* �������ƣ�SaveFile(HGLOBAL hDib, CFile& file)
* ��������:
*   HGLOBAL hDib��Ҫ�����DIB
*   CFile& file�������ļ�CFile
* ��������:BOOL
* ��������:��ָ����DIB���󱣴浽ָ����CFile��
*************************************************************************/

BOOL CDib::SaveFile(HGLOBAL hDib, CFile& file)
{
	if (hDib == NULL)
	{		
		return FALSE;// ���DIBΪ�գ�����FALSE
	}	
	lpbmihrd = (LPBITMAPINFOHEADER) ::GlobalLock((HGLOBAL) hDib);// ��ȡBITMAPINFO�ṹ��������	
	if (lpbmihrd == NULL)
	{		
		return FALSE;// Ϊ�գ�����FALSE
	}		
	bmfHeader.bfType = DIB_MARKER;	// ����ļ�ͷ	
	dwDIBSize = *(LPDWORD)lpbmihrd + GetPalSize((LPSTR)lpbmihrd);// �ļ�ͷ��С����ɫ���С			
	DWORD dwBmBitsSize;// ���صĴ�С		
	dwBmBitsSize = WIDTHBYTES((lpbmihrd->biWidth)*((DWORD)lpbmihrd->biBitCount)) * lpbmihrd->biHeight;// ��СΪWidth * Height		
	dwDIBSize += dwBmBitsSize;// �����DIBÿ���ֽ���Ϊ4�ı���ʱ�Ĵ�С
	lpbmihrd->biSizeImage = dwBmBitsSize;// ����biSizeImage	
	bmfHeader.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);// �ļ���С		
	bmfHeader.bfReserved1 = 0;// ����������
	bmfHeader.bfReserved2 = 0;
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + lpbmihrd->biSize// ����ƫ����bfOffBits
		+ GetPalSize((LPSTR)lpbmihrd);
	TRY
	{		
		file.Write(&bmfHeader, sizeof(BITMAPFILEHEADER));// д�ļ�ͷ				
		file.Write(lpbmihrd, dwDIBSize);// дDIBͷ������
	}
	CATCH (CFileException, e)
	{		
		::GlobalUnlock((HGLOBAL) hDib);// �������				
		THROW_LAST();// �׳��쳣
	}
	END_CATCH		
		::GlobalUnlock((HGLOBAL) hDib);// �������	
	return TRUE;// ����TRUE
}


/*************************************************************************
* �������ƣ�LoadFile(CFile& file)
* ��������:
*   CFile& file��Ҫ��ȡ���ļ��ļ�CFile
* ��������:HGLOBAL
* ��������:��ָ�����ļ��е�DIB�������ָ�����ڴ�������
*************************************************************************/

HGLOBAL CDib::LoadFile(CFile& file)
{

	DWORD dwFileSize;

	dwFileSize = (DWORD)file.GetLength();// ��ȡ�ļ���С	
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))// ��ȡDIB�ļ�ͷ
	{		
		return NULL;// ��С��һ�£�����NULL��
	}
	if (bmfHeader.bfType != DIB_MARKER)// �ж��Ƿ���DIB����
	{
		AfxMessageBox(_T("����λͼ����"));
		return NULL;// ��������򷵻�NULL��
	}	
	m_hDib = (HGLOBAL) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwFileSize - sizeof(BITMAPFILEHEADER));// ����DIB�ڴ�
	if (!m_hDib)
	{		
		return NULL;// ����ʧ�ܣ�����NULL
	}
	/////////////////////////////////////////////////////////////////////////
	//��CDib��ĳ�Ա������ֵ	
	lpdib = (LPSTR) ::GlobalLock((HGLOBAL) m_hDib);// ����
	lpbminfo=(BITMAPINFO*)lpdib;
	lpbmihrd=(BITMAPINFOHEADER*)lpdib;
	lpRgbQuag=(RGBQUAD*)(lpdib + lpbmihrd->biSize);
	int m_numberOfColors = GetColorNum((LPSTR)lpbmihrd);
	if (!lpbmihrd->biClrUsed)
		lpbmihrd->biClrUsed = m_numberOfColors;
	DWORD colorTableSize = m_numberOfColors * sizeof(RGBQUAD);
	lpDIBBits = lpdib + lpbmihrd->biSize + colorTableSize;
	/////////////////////////////////////////////////////////////////////////

	if (file.Read(lpdib, dwFileSize - sizeof(BITMAPFILEHEADER)) !=// ������
		dwFileSize - sizeof(BITMAPFILEHEADER) )//��С��һ��
	{					
		::GlobalUnlock((HGLOBAL) m_hDib);	// �������			
		::GlobalFree((HGLOBAL) m_hDib);	// �ͷ��ڴ�
		return NULL;
	}		
	::GlobalUnlock((HGLOBAL) m_hDib);// �������		
	return m_hDib;// ����DIB���
}

/*************************************************************************
* �������ƣ�GetRectWidth(LPCRECT lpRect)
* ��������:
*   LPCRECT lpRect��ָ����������ָ��
* ��������:long
* ��������:��ȡ��������Ŀ��
*************************************************************************/

long  CDib::GetRectWidth(LPCRECT lpRect)
{
	long nWidth=lpRect->right - lpRect->left;
	return nWidth;
}
/*************************************************************************
* �������ƣ�GetRectHeight(LPCRECT lpRect)
* ��������:
*   LPCRECT lpRect��ָ����������ָ��
* ��������:long
* ��������:��ȡ��������ĸ߶�
*************************************************************************/
long  CDib::GetRectHeight(LPCRECT lpRect)
{
	long nHeight=lpRect->bottom - lpRect->top;
	return nHeight;
}

/*************************************************************************
* �������ƣ�InitMembers() 
* ��������: void
* ��������:��ʼ����ĳ�Ա����
*************************************************************************/
void CDib::InitMembers()
{
	m_hDib=NULL;
	lpbmihrd=NULL;	// ָ��BITMAPINFO�ṹ��ָ��
	lpdib=NULL;
	lpDIBBits=NULL;
	dwDIBSize=0;
	lpRgbQuag=NULL;

	/******************����Ϊ��dib�Ĳ���*********************/
	// ���ݳ�Ա��ʼ��
	_tcscpy(m_fileName, _T(""));
	m_lpBmpFileHeader = NULL;
	m_lpDib = NULL;   
	m_lpBmpInfo = NULL;
	m_lpBmpInfoHeader = NULL;
	m_lpRgbQuad = NULL;
	m_lpData = NULL;
	m_hPalette = NULL;
	m_bHasRgbQuad = FALSE;
	m_bValid = FALSE;
}

/*************************************************************************
* �������ƣ�ClearMemory() 
* ��������: void
* ��������:��λ��ĳ�Ա����
*************************************************************************/
void CDib::ClearMemory()
{
	if(m_hDib!=NULL)
		::GlobalFree(m_hDib);
	lpbmihrd=NULL;	// ָ��BITMAPINFO�ṹ��ָ��
	lpdib=NULL;
	dwDIBSize=0;
	lpRgbQuag=NULL;	
	//dwDIBSize=0;

	/***************************��������dib�Ĳ���*************************/
	Empty();
}

BOOL CDib::IsEmpty()
{
	if (m_hDib == NULL && lpdib == NULL)
	{
		AfxMessageBox(_T("δ����ͼƬ���޷�����!"));
		return TRUE;
	}

	return FALSE;
}

/*******************************��������dib�Ĳ���************************/
//=======================================================
// �������ܣ� ���ļ�����λͼ
// ��������� LPCTSTR lpszPath-������λͼ�ļ�·��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::LoadFromFile(LPCTSTR lpszPath)
{
	// ��¼λͼ�ļ���
	_tcscpy(m_fileName, lpszPath);

	// �Զ�ģʽ��λͼ�ļ�
	CFile dibFile;
	if(!dibFile.Open(m_fileName, CFile::modeRead | CFile::shareDenyWrite))
	{
		return FALSE;
	}

	// ����ռ�
	Empty(); 

	// Ϊλͼ�ļ�ͷ����ռ䣬����ʼ��Ϊ0
	m_lpBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
	memset(m_lpBmpFileHeader, 0, sizeof(BITMAPFILEHEADER)); 

	// ��ȡλͼ�ļ�ͷ
	int nCount = dibFile.Read((void *)m_lpBmpFileHeader, sizeof(BITMAPFILEHEADER));
	if(nCount != sizeof(BITMAPFILEHEADER)) 
	{
		return FALSE;
	} 

	// �жϴ��ļ��ǲ���λͼ�ļ�����0x4d42������BM����
	if(m_lpBmpFileHeader->bfType == 0x4d42)
	{
		// ��λͼ�ļ�

		// �����λͼ�ļ�ͷ�Ŀռ��С������ռ䲢��ʼ��Ϊ0
		DWORD dwDibSize = DWORD(dibFile.GetLength() - sizeof(BITMAPFILEHEADER));
		m_lpDib = new BYTE[dwDibSize];
		memset(m_lpDib, 0, dwDibSize);

		// ��ȡ��λͼ�ļ�ͷ����������
		dibFile.Read(m_lpDib, dwDibSize);

		// �ر�λͼ�ļ�
		dibFile.Close();

		// ����λͼ��Ϣָ��
		m_lpBmpInfo = (LPBITMAPINFO)m_lpDib;

		// ����λͼ��Ϣͷָ��
		m_lpBmpInfoHeader = (LPBITMAPINFOHEADER)m_lpDib;

		// ����λͼ��ɫ��ָ��
		m_lpRgbQuad = (LPRGBQUAD)(m_lpDib + m_lpBmpInfoHeader->biSize);

		// ���λͼû������λͼʹ�õ���ɫ����������
		if(m_lpBmpInfoHeader->biClrUsed == 0)
		{
			m_lpBmpInfoHeader->biClrUsed = GetNumOfColor();
		}

		// ������ɫ����
		DWORD dwRgbQuadLength = CalcRgbQuadLength();

		// ����λͼ����ָ��
		m_lpData = m_lpDib + m_lpBmpInfoHeader->biSize + dwRgbQuadLength;

		// �ж��Ƿ�����ɫ��
		if(m_lpRgbQuad == (LPRGBQUAD)m_lpData)
		{
			m_lpRgbQuad = NULL;    // ��λͼ��ɫ��ָ���ÿ�
			m_bHasRgbQuad = FALSE; // ����ɫ��
		}
		else
		{
			m_bHasRgbQuad = TRUE;  // ����ɫ��
			MakePalette();         // ������ɫ�����ɵ�ɫ��
		}        

		// ����λͼ��С����Ϊ�ܶ�λͼ�ļ��������ô��
		m_lpBmpInfoHeader->biSizeImage = GetSize();

		// λͼ��Ч
		m_bValid = TRUE;

		return TRUE;
	}
	else
	{
		// ����λͼ�ļ�
		m_bValid = FALSE;

		return FALSE;
	}     

}

//=======================================================
// �������ܣ� ��λͼ���浽�ļ�
// ��������� LPCTSTR lpszPath-λͼ�ļ�����·��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::SaveToFile(LPCTSTR lpszPath)
{
	// ��дģʽ���ļ�
	CFile dibFile;
	if(!dibFile.Open(lpszPath, CFile::modeCreate | CFile::modeReadWrite 
		| CFile::shareExclusive))
	{
		return FALSE;
	}

	// ��¼λͼ�ļ���
	_tcscpy(m_fileName, lpszPath);

	// ���ļ�ͷ�ṹд���ļ�
	dibFile.Write(m_lpBmpFileHeader, sizeof(BITMAPFILEHEADER));

	// ���ļ���Ϣͷ�ṹд���ļ�
	dibFile.Write(m_lpBmpInfoHeader, sizeof(BITMAPINFOHEADER));

	// ������ɫ����
	DWORD dwRgbQuadlength = CalcRgbQuadLength();

	// �������ɫ��Ļ�������ɫ��д���ļ�
	if(dwRgbQuadlength != 0)
	{
		dibFile.Write(m_lpRgbQuad, dwRgbQuadlength);
	}                                                        

	// ��λͼ����д���ļ�
	DWORD dwDataSize = GetLineByte() * GetHeight();
	dibFile.Write(m_lpData, dwDataSize);

	// �ر��ļ�
	dibFile.Close();

	return TRUE;
}

//=======================================================
// �������ܣ� ��ȡλͼ�ļ���
// ��������� ��
// ����ֵ��   LPCTSTR-λͼ�ļ���
//=======================================================
LPCTSTR CDib::GetFileName()
{
	return m_fileName;
}

//=======================================================
// �������ܣ� ��ȡλͼ���
// ��������� ��
// ����ֵ��   LONG-λͼ���
//=======================================================
LONG CDib::GetWidth()
{
	return m_lpBmpInfoHeader->biWidth;
}

//=======================================================
// �������ܣ� ��ȡλͼ�߶�
// ��������� ��
// ����ֵ��   LONG-λͼ�߶�
//=======================================================
LONG CDib::GetHeight()
{
	return m_lpBmpInfoHeader->biHeight;
}

//=======================================================
// �������ܣ� ��ȡλͼ�Ŀ�Ⱥ͸߶�
// ��������� ��
// ����ֵ��   CSize-λͼ�Ŀ�Ⱥ͸߶�
//=======================================================
CSize CDib::GetDimension()
{
	return CSize(GetWidth(), GetHeight());
}

//=======================================================
// �������ܣ� ��ȡλͼ��С
// ��������� ��
// ����ֵ��   DWORD-λͼ��С
//=======================================================
DWORD CDib::GetSize()
{
	if(m_lpBmpInfoHeader->biSizeImage != 0)
	{
		return m_lpBmpInfoHeader->biSizeImage;
	}
	else
	{       
		return GetWidth() * GetHeight();
	}
}

//=======================================================
// �������ܣ� ��ȡ����������ռλ��
// ��������� ��
// ����ֵ��   WORD-����������ռλ��
//=======================================================
WORD CDib::GetBitCount()
{
	return m_lpBmpInfoHeader->biBitCount;
}       

//=======================================================
// �������ܣ� ��ȡÿ��������ռ�ֽ���
// ��������� ��
// ����ֵ��   UINT-ÿ��������ռ�ֽ���
//=======================================================
UINT CDib::GetLineByte()
{ 
	return (GetWidth() * GetBitCount() / 8 + 3) / 4 * 4;;
}

//=======================================================
// �������ܣ� ��ȡλͼ��ɫ��
// ��������� ��
// ����ֵ��   DWORD-λͼ��ɫ��
//=======================================================
DWORD CDib::GetNumOfColor()
{
	UINT dwNumOfColor;     

	if ((m_lpBmpInfoHeader->biClrUsed == 0) 
		&& (m_lpBmpInfoHeader->biBitCount < 9))
	{
		switch (m_lpBmpInfoHeader->biBitCount)
		{
		case 1: dwNumOfColor = 2; break;
		case 4: dwNumOfColor = 16; break;
		case 8: dwNumOfColor = 256;
		}
	}
	else
	{
		dwNumOfColor = m_lpBmpInfoHeader->biClrUsed;
	}  		

	return dwNumOfColor; 
}

//=======================================================
// �������ܣ� ����λͼ��ɫ����
// ��������� ��
// ����ֵ��   DWORD-λͼ��ɫ����
//=======================================================
DWORD CDib::CalcRgbQuadLength()
{
	DWORD dwNumOfColor = GetNumOfColor();
	if(dwNumOfColor > 256)
	{
		dwNumOfColor = 0;
	}
	return  dwNumOfColor * sizeof(RGBQUAD);
}

//=======================================================
// �������ܣ� ��ȡλͼ��ɫ��
// ��������� ��
// ����ֵ��   LPRGBQUAD-λͼ��ɫ��ָ��
//=======================================================
LPRGBQUAD CDib::GetRgbQuad()
{
	return m_lpRgbQuad;
}

//=======================================================
// �������ܣ� ������ɫ�����ɵ�ɫ��
// ��������� ��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::MakePalette()
{
	// ������ɫ����
	DWORD dwRgbQuadLength = CalcRgbQuadLength();

	// �����ɫ����Ϊ0���������߼���ɫ��
	if(dwRgbQuadLength == 0) 
	{
		return FALSE;
	}

	//ɾ���ɵĵ�ɫ�����
	if(m_hPalette != NULL) 
	{
		DeleteObject(m_hPalette);
		m_hPalette = NULL;
	}

	// ���뻺��������ʼ��Ϊ0
	DWORD dwNumOfColor = GetNumOfColor();
	DWORD dwSize = 2 * sizeof(WORD) + dwNumOfColor * sizeof(PALETTEENTRY);
	LPLOGPALETTE lpLogPalette = (LPLOGPALETTE) new BYTE[dwSize];
	memset(lpLogPalette, 0, dwSize);

	// �����߼���ɫ��
	lpLogPalette->palVersion = 0x300;
	lpLogPalette->palNumEntries = (WORD)dwNumOfColor;
	LPRGBQUAD lpRgbQuad = (LPRGBQUAD) m_lpRgbQuad;
	for(int i = 0; i < (int)dwNumOfColor; i++) 
	{
		lpLogPalette->palPalEntry[i].peRed = lpRgbQuad->rgbRed;
		lpLogPalette->palPalEntry[i].peGreen = lpRgbQuad->rgbGreen;
		lpLogPalette->palPalEntry[i].peBlue = lpRgbQuad->rgbBlue;
		lpLogPalette->palPalEntry[i].peFlags = 0;
		lpRgbQuad++;
	}

	// �����߼���ɫ��
	m_hPalette = CreatePalette(lpLogPalette);

	// �ͷŻ�����
	delete [] lpLogPalette;

	return TRUE;
}

//=======================================================
// �������ܣ� 24λ��ɫλͼת8λ�Ҷ�λͼ
// ��������� ��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::RgbToGrade()
{
	// λͼ��Ч��ʧ�ܷ���
	if(!IsValid())
	{
		return FALSE;
	}

	// ����24λλͼ��ʧ�ܷ���
	if(GetBitCount() != 24)
	{
		return FALSE;
	}

	// ��ѹ��λͼ��ʧ�ܷ���
	if(m_lpBmpInfoHeader->biCompression != BI_RGB)
	{
		return FALSE;
	}

	// ������ǻҶ�λͼ������Ҫת��
	if(!IsGrade())
	{
		// ��ȡԭλͼ��Ϣ
		LONG lHeight = GetHeight();
		LONG lWidth = GetWidth();
		UINT uLineByte = GetLineByte();

		// ����Ҷ�λͼ��������ռ�
		UINT uGradeBmpLineByte = (lWidth + 3) / 4 * 4;
		DWORD dwGradeBmpDataSize = uGradeBmpLineByte * lHeight; 

		// ����Ҷ�λͼ����ռ�
		DWORD dwGradeBmpSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + dwGradeBmpDataSize;

		// ���ûҶ�λͼ�ļ�ͷ
		LPBITMAPFILEHEADER lpGradeBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
		memset(lpGradeBmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
		lpGradeBmpFileHeader->bfType = 0x4d42;
		lpGradeBmpFileHeader->bfSize = sizeof(BITMAPFILEHEADER) + dwGradeBmpSize;
		lpGradeBmpFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
			+ sizeof(RGBQUAD) * 256;
		lpGradeBmpFileHeader->bfReserved1 = 0;
		lpGradeBmpFileHeader->bfReserved2 = 0;            

		// Ϊ�Ҷ�λͼ����ռ䣬����ʼ��Ϊ0
		LPBYTE lpGradeBmp = (LPBYTE)new BYTE[dwGradeBmpSize];
		memset(lpGradeBmp, 0, dwGradeBmpSize);

		// ���ûҶ�λͼ��Ϣͷ
		LPBITMAPINFOHEADER lpGradeBmpInfoHeader = (LPBITMAPINFOHEADER)(lpGradeBmp);
		lpGradeBmpInfoHeader->biBitCount = 8;
		lpGradeBmpInfoHeader->biClrImportant = 0;
		lpGradeBmpInfoHeader->biClrUsed = 256;
		lpGradeBmpInfoHeader->biCompression = BI_RGB;
		lpGradeBmpInfoHeader->biHeight = lHeight;
		lpGradeBmpInfoHeader->biPlanes = 1;
		lpGradeBmpInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
		lpGradeBmpInfoHeader->biSizeImage = dwGradeBmpDataSize;
		lpGradeBmpInfoHeader->biWidth = lWidth;
		lpGradeBmpInfoHeader->biXPelsPerMeter = m_lpBmpInfoHeader->biXPelsPerMeter;
		lpGradeBmpInfoHeader->biYPelsPerMeter = m_lpBmpInfoHeader->biYPelsPerMeter;

		// ���ûҶ�λͼ��ɫ��
		LPRGBQUAD lpGradeBmpRgbQuad = (LPRGBQUAD)(lpGradeBmp + sizeof(BITMAPINFOHEADER));

		// ��ʼ��8λ�Ҷ�ͼ�ĵ�ɫ����Ϣ
		LPRGBQUAD lpRgbQuad;               
		for(int k = 0; k < 256; k++)
		{
			lpRgbQuad = (LPRGBQUAD)(lpGradeBmpRgbQuad + k);
			lpRgbQuad->rgbBlue = k; 
			lpRgbQuad->rgbGreen = k;
			lpRgbQuad->rgbRed = k;
			lpRgbQuad->rgbReserved = 0;
		}

		// �Ҷ�λͼ���ݴ���
		BYTE r, g, b; 
		LPBYTE lpGradeBmpData = (LPBYTE)(lpGradeBmp + sizeof(BITMAPINFOHEADER) 
			+ sizeof(RGBQUAD) * 256);
		// ������ɫת��
		for(int i = 0; i < lHeight; i++)
		{
			for(int j = 0; j < lWidth; j++)
			{
				b = m_lpData[i * uLineByte + 3 * j];
				g = m_lpData[i * uLineByte + 3 * j + 1];
				r = m_lpData[i * uLineByte + 3 * j + 2];
				lpGradeBmpData[i * uGradeBmpLineByte + j] = (BYTE)(0.299 * r + 0.587 * g + 0.114 * b); 
			}
		}

		// �ͷ�ԭ��λͼ�ռ�
		Empty(FALSE);

		// �����趨ԭλͼָ��ָ��
		m_lpBmpFileHeader = lpGradeBmpFileHeader;
		m_lpDib = lpGradeBmp;
		m_lpBmpInfo = (LPBITMAPINFO)(lpGradeBmp);
		m_lpBmpInfoHeader = lpGradeBmpInfoHeader;
		m_lpRgbQuad = lpGradeBmpRgbQuad;
		m_lpData = lpGradeBmpData;

		// ������ɫ���־
		m_bHasRgbQuad = TRUE;  
		// ����λͼ��Ч��־
		m_bValid = TRUE;
		// ���ɵ�ɫ��
		MakePalette();
	}

	return TRUE;   
}   

//=======================================================
// �������ܣ� 8λ�Ҷ�λͼת24λ��ɫλͼ
// ��������� ��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::GradeToRgb()
{
	// λͼ��Ч��ʧ���˳�
	if(!IsValid())
	{
		return FALSE;
	}

	// ����8λλͼ��ʧ���˳�
	if(GetBitCount() != 8)
	{
		return FALSE;
	}

	// ��ѹ��λͼ��ʧ�ܷ���
	if(m_lpBmpInfoHeader->biCompression != BI_RGB)
	{
		return FALSE;
	}

	// �ǻҶ�ͼʱ������ת��
	if(IsGrade())
	{
		// ��ȡԭλͼ��Ϣ
		LONG lHeight = GetHeight();
		LONG lWidth = GetWidth();
		UINT uLineByte = GetLineByte();

		// �����ɫλͼ��������ռ�
		UINT uColorBmpLineByte = (lWidth * 24 / 8 + 3) / 4 * 4;
		DWORD dwColorBmpDataSize = uColorBmpLineByte * lHeight; 

		// �����ɫλͼ����ռ�
		DWORD dwColorBmpSize = sizeof(BITMAPINFOHEADER) + dwColorBmpDataSize;

		// ���ò�ɫλͼ�ļ�ͷ
		LPBITMAPFILEHEADER lpColorBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
		memset(lpColorBmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
		lpColorBmpFileHeader->bfType = 0x4d42;
		lpColorBmpFileHeader->bfSize = sizeof(BITMAPFILEHEADER) + dwColorBmpSize;
		lpColorBmpFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		lpColorBmpFileHeader->bfReserved1 = 0;
		lpColorBmpFileHeader->bfReserved2 = 0;    

		// Ϊ��ɫλͼ����ռ䣬����ʼ��Ϊ0
		LPBYTE lpColorBmp = (LPBYTE)new BYTE[dwColorBmpSize];
		memset(lpColorBmp, 0, dwColorBmpSize);

		// ���ò�ɫλͼ��Ϣͷ
		LPBITMAPINFOHEADER lpColorBmpInfoHeader = (LPBITMAPINFOHEADER)(lpColorBmp);
		lpColorBmpInfoHeader->biBitCount = 24;
		lpColorBmpInfoHeader->biClrImportant = 0;
		lpColorBmpInfoHeader->biClrUsed = 0;
		lpColorBmpInfoHeader->biCompression = BI_RGB;
		lpColorBmpInfoHeader->biHeight = lHeight;
		lpColorBmpInfoHeader->biPlanes = 1;
		lpColorBmpInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
		lpColorBmpInfoHeader->biSizeImage = dwColorBmpDataSize;
		lpColorBmpInfoHeader->biWidth = lWidth;
		lpColorBmpInfoHeader->biXPelsPerMeter = m_lpBmpInfoHeader->biXPelsPerMeter;
		lpColorBmpInfoHeader->biYPelsPerMeter = m_lpBmpInfoHeader->biYPelsPerMeter;

		// ��ɫλͼ���ݴ���
		LPBYTE lpColorBmpData = (LPBYTE)(lpColorBmp + sizeof(BITMAPINFOHEADER));
		// ������ɫת��
		for(int i = 0; i < lHeight; i++)
		{
			for(int j = 0; j < lWidth; j++)
			{
				BYTE btValue = m_lpData[i * uLineByte + j]; 
				lpColorBmpData[i * uColorBmpLineByte + 3 * j] = btValue;
				lpColorBmpData[i * uColorBmpLineByte + 3 * j + 1] = btValue;
				lpColorBmpData[i * uColorBmpLineByte + 3 * j + 2] = btValue;  
			}
		}

		// �ͷ�ԭ��λͼ�ռ�
		Empty(FALSE);

		// �����趨ԭλͼָ��ָ��
		m_lpBmpFileHeader = lpColorBmpFileHeader;
		m_lpDib = lpColorBmp;
		m_lpBmpInfo = (LPBITMAPINFO)(lpColorBmp);
		m_lpBmpInfoHeader = lpColorBmpInfoHeader;
		m_lpRgbQuad = NULL;
		m_lpData = lpColorBmpData;

		// ������ɫ���־
		m_bHasRgbQuad = FALSE;  
		// ����λͼ��Ч��־
		m_bValid = TRUE;        
	}        

	return TRUE;   
}   

//=======================================================
// �������ܣ� �ж��Ƿ�����ɫ��
// ��������� ��
// ����ֵ��   �жϽ����TRUE-������ɫ��FALSE-������ɫ��
//=======================================================
BOOL CDib::HasRgbQuad()
{
	return m_bHasRgbQuad;
}

//=======================================================
// �������ܣ� �ж��Ƿ��ǻҶ�ͼ
// ��������� ��
// ����ֵ��   �жϽ����TRUE-�ǻҶ�ͼ��FALSE-�ǲ�ɫͼ
//=======================================================
BOOL CDib::IsGrade()
{
	return (GetBitCount() < 9 && GetBitCount() > 0);
}

//=======================================================
// �������ܣ� �ж�λͼ�Ƿ���Ч
// ��������� ��
// ����ֵ��   �жϽ����TRUE-λͼ��Ч��FALSE-λͼ��Ч
//=======================================================
BOOL CDib::IsValid()
{
	return m_bValid;
}

//=======================================================
// �������ܣ� ����ռ�
// ��������� BOOL bFlag-TRUE ȫ����գ�FALSE �������
// ����ֵ��   ��
//=======================================================
void CDib::Empty(BOOL bFlag)
{
	// �ļ������
	if(bFlag)
	{
		_tcscpy(m_fileName, _T(""));
	}      

	// �ͷ�λͼ�ļ�ͷָ��ռ�
	if(m_lpBmpFileHeader != NULL)
	{
		delete [] m_lpBmpFileHeader;
		m_lpBmpFileHeader = NULL;
	}    

	// �ͷ�λͼָ��ռ�
	if(m_lpDib != NULL)
	{
		delete [] m_lpDib;
		m_lpDib = NULL;
		m_lpBmpInfo = NULL;
		m_lpBmpInfoHeader = NULL;
		m_lpRgbQuad = NULL;
		m_lpData = NULL;           
	}       

	// �ͷŵ�ɫ��
	if(m_hPalette != NULL)
	{
		DeleteObject(m_hPalette);
		m_hPalette = NULL;
	}    

	// ���ò�����ɫ��
	m_bHasRgbQuad = FALSE;

	// ����λͼ��Ч
	m_bValid = FALSE;

}  

/*************************************************************************
* �������ƣ�GetDibWidthBytes(LPSTR lpdib)
* ��������:
*   LPSTR lpdib��ָ��DIB�����ָ��
* ��������:DWORD
* ��������:�ú�������DIB����ֽ���
************************************************************************/
DWORD CDib::GetDibWidthBytes(LPSTR lpdib)
{
	WORD biBitCount = lpbmihrd->biBitCount;
	LONG lWidth = lpbmihrd->biWidth;

	DWORD dwWidthBytes = (DWORD)lWidth;
	if (1 == biBitCount)
		dwWidthBytes = (lWidth + 7) / 8;
	else if (4 == biBitCount)
		dwWidthBytes = (lWidth + 1) / 2;
	else if (24 == biBitCount)
		dwWidthBytes = 3 * lWidth;

	while ((dwWidthBytes & 3) != 0)
		++dwWidthBytes;
	return dwWidthBytes;
}