// Dib.h: interface for the CDib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIB_H__AC952C3A_9B6B_4319_8D6E_E7F509348A88__INCLUDED_)
#define AFX_DIB_H__AC952C3A_9B6B_4319_8D6E_E7F509348A88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PalVersion   0x300                                 // ��ɫ��汾

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4) 


class CDib : public CObject  
{
public:
	CDib();
	virtual ~CDib();
	//operations
public:
	// ���ڲ���DIB�ĺ�������
	BOOL     DrawDib(HDC, LPRECT,HGLOBAL, LPRECT,CPalette*);// ��ʾλͼ
	BOOL     ConstructPalette(HGLOBAL,CPalette* );          // �����߼���ɫ��
	LPSTR    GetBits(LPSTR);                                // ȡ��λͼ���ݵ���ڵ�ַ
	DWORD    GetWidth(LPSTR);                               // ȡ��λͼ�Ŀ��
	DWORD    GetHeight(LPSTR);                              // ȡ��λͼ�ĸ߶�
	WORD     GetPalSize(LPSTR);                             // ȡ�õ�ɫ��Ĵ�С
	WORD     GetColorNum(LPSTR);                            // ȡ��λͼ��������ɫ��Ŀ
	WORD     GetBitCount(LPSTR);                            // ȡ��λͼ����ɫ���
	HGLOBAL  CopyObject(HGLOBAL);                           // ���ڸ���λͼ����
	DWORD    GetDibWidthBytes(LPSTR);                       // ��ȡλͼ����ֽ���
	BOOL     SaveFile(HGLOBAL , CFile&);                    // �洢λͼΪ�ļ�
	HGLOBAL  LoadFile(CFile&);                              // ���ļ��м���λͼ
	BOOL	 IsEmpty();	
	long     GetRectWidth(LPCRECT );                        // ȡ������Ŀ��
	long     GetRectHeight(LPCRECT);                        // ȡ������ĸ߶�

public:
	void ClearMemory();
	void InitMembers();
public:
	LPBITMAPINFO        lpbminfo;	// ָ��BITMAPINFO�ṹ��ָ��
	LPBITMAPINFOHEADER  lpbmihrd;	//ָ��BITMAPINFOHEADER�ṹ��ָ��
	BITMAPFILEHEADER	bmfHeader;  //BITMAPFILEHEADER�ṹ
	LPSTR				lpdib;      //ָ��DIB��ָ��
	LPSTR				lpDIBBits;  // DIB����ָ��
	DWORD				dwDIBSize;  //DIB��С

	HGLOBAL				m_hDib;//DIB����ľ��

	RGBQUAD*			lpRgbQuag;//ָ����ɫ���ָ��

	/**********************����Ϊ��dib����Ĳ���**************************/
	// ���ļ�����λͼ
	BOOL LoadFromFile(LPCTSTR lpszPath);
	// ��λͼ���浽�ļ�
	BOOL SaveToFile(LPCTSTR lpszPath);
	// ��ȡλͼ�ļ���
	LPCTSTR GetFileName();
	// ��ȡλͼ���
	LONG GetWidth();
	// ��ȡλͼ�߶�
	LONG GetHeight();
	// ��ȡλͼ�Ŀ�Ⱥ͸߶�
	CSize GetDimension();  
	// ��ȡλͼ��С
	DWORD GetSize();
	// ��ȡ����������ռλ��
	WORD GetBitCount();
	// ��ȡÿ��������ռ�ֽ���
	UINT GetLineByte();
	// ��ȡλͼ��ɫ��
	DWORD GetNumOfColor();
	// ��ȡλͼ��ɫ��
	LPRGBQUAD GetRgbQuad();
	// 24λ��ɫλͼת8λ�Ҷ�λͼ
	BOOL RgbToGrade();
	// 8λ�Ҷ�λͼת24λ��ɫλͼ
	BOOL GradeToRgb();
	// �ж��Ƿ�����ɫ��
	BOOL HasRgbQuad();
	// �ж��Ƿ��ǻҶ�ͼ
	BOOL IsGrade();
	// �ж�λͼ�Ƿ���Ч
	BOOL IsValid();  
protected:
	// ����λͼ��ɫ����
	DWORD CalcRgbQuadLength();
	// ������ɫ�����ɵ�ɫ��
	BOOL MakePalette();
	// ����ռ�
	void Empty(BOOL bFlag = TRUE);
private:
	// λͼ�ļ���
	TCHAR m_fileName[_MAX_PATH];
	// λͼ�ļ�ͷָ��    
	LPBITMAPFILEHEADER m_lpBmpFileHeader; // ��Ҫ��̬������ͷ� 
	// λͼָ�루������λͼ�ļ�ͷ���������ݣ�
	LPBYTE m_lpDib;                       // ��Ҫ��̬������ͷ�
	// λͼ��Ϣָ��
	LPBITMAPINFO m_lpBmpInfo;
	// λͼ��Ϣͷָ��
	LPBITMAPINFOHEADER m_lpBmpInfoHeader;  
	// λͼ��ɫ��ָ��
	LPRGBQUAD m_lpRgbQuad; 
	// λͼ����ָ��
	LPBYTE m_lpData; 
	// ��ɫ����
	HPALETTE m_hPalette;
	// �Ƿ�����ɫ��
	BOOL m_bHasRgbQuad;
	// λͼ�Ƿ���Ч
	BOOL m_bValid;
};

#endif // !defined(AFX_DIB_H__AC952C3A_9B6B_4319_8D6E_E7F509348A88__INCLUDED_)