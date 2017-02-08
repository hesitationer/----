// Dib.h: interface for the CDib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIB_H__AC952C3A_9B6B_4319_8D6E_E7F509348A88__INCLUDED_)
#define AFX_DIB_H__AC952C3A_9B6B_4319_8D6E_E7F509348A88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PalVersion   0x300                                 // 调色板版本

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4) 


class CDib : public CObject  
{
public:
	CDib();
	virtual ~CDib();
	//operations
public:
	// 用于操作DIB的函数声明
	BOOL     DrawDib(HDC, LPRECT,HGLOBAL, LPRECT,CPalette*);// 显示位图
	BOOL     ConstructPalette(HGLOBAL,CPalette* );          // 构造逻辑调色板
	LPSTR    GetBits(LPSTR);                                // 取得位图数据的入口地址
	DWORD    GetWidth(LPSTR);                               // 取得位图的宽度
	DWORD    GetHeight(LPSTR);                              // 取得位图的高度
	WORD     GetPalSize(LPSTR);                             // 取得调色板的大小
	WORD     GetColorNum(LPSTR);                            // 取得位图包含的颜色数目
	WORD     GetBitCount(LPSTR);                            // 取得位图的颜色深度
	HGLOBAL  CopyObject(HGLOBAL);                           // 用于复制位图对象
	DWORD    GetDibWidthBytes(LPSTR);                       // 获取位图宽度字节数
	BOOL     SaveFile(HGLOBAL , CFile&);                    // 存储位图为文件
	HGLOBAL  LoadFile(CFile&);                              // 从文件中加载位图
	BOOL	 IsEmpty();	
	long     GetRectWidth(LPCRECT );                        // 取得区域的宽度
	long     GetRectHeight(LPCRECT);                        // 取得区域的高度

public:
	void ClearMemory();
	void InitMembers();
public:
	LPBITMAPINFO        lpbminfo;	// 指向BITMAPINFO结构的指针
	LPBITMAPINFOHEADER  lpbmihrd;	//指向BITMAPINFOHEADER结构的指针
	BITMAPFILEHEADER	bmfHeader;  //BITMAPFILEHEADER结构
	LPSTR				lpdib;      //指向DIB的指针
	LPSTR				lpDIBBits;  // DIB像素指针
	DWORD				dwDIBSize;  //DIB大小

	HGLOBAL				m_hDib;//DIB对象的句柄

	RGBQUAD*			lpRgbQuag;//指向颜色表的指针

	/**********************以下为新dib对象的操作**************************/
	// 从文件加载位图
	BOOL LoadFromFile(LPCTSTR lpszPath);
	// 将位图保存到文件
	BOOL SaveToFile(LPCTSTR lpszPath);
	// 获取位图文件名
	LPCTSTR GetFileName();
	// 获取位图宽度
	LONG GetWidth();
	// 获取位图高度
	LONG GetHeight();
	// 获取位图的宽度和高度
	CSize GetDimension();  
	// 获取位图大小
	DWORD GetSize();
	// 获取单个像素所占位数
	WORD GetBitCount();
	// 获取每行像素所占字节数
	UINT GetLineByte();
	// 获取位图颜色数
	DWORD GetNumOfColor();
	// 获取位图颜色表
	LPRGBQUAD GetRgbQuad();
	// 24位彩色位图转8位灰度位图
	BOOL RgbToGrade();
	// 8位灰度位图转24位彩色位图
	BOOL GradeToRgb();
	// 判断是否含有颜色表
	BOOL HasRgbQuad();
	// 判断是否是灰度图
	BOOL IsGrade();
	// 判断位图是否有效
	BOOL IsValid();  
protected:
	// 计算位图颜色表长度
	DWORD CalcRgbQuadLength();
	// 根据颜色表生成调色板
	BOOL MakePalette();
	// 清理空间
	void Empty(BOOL bFlag = TRUE);
private:
	// 位图文件名
	TCHAR m_fileName[_MAX_PATH];
	// 位图文件头指针    
	LPBITMAPFILEHEADER m_lpBmpFileHeader; // 需要动态分配和释放 
	// 位图指针（包含除位图文件头的所有内容）
	LPBYTE m_lpDib;                       // 需要动态分配和释放
	// 位图信息指针
	LPBITMAPINFO m_lpBmpInfo;
	// 位图信息头指针
	LPBITMAPINFOHEADER m_lpBmpInfoHeader;  
	// 位图颜色表指针
	LPRGBQUAD m_lpRgbQuad; 
	// 位图数据指针
	LPBYTE m_lpData; 
	// 调色板句柄
	HPALETTE m_hPalette;
	// 是否有颜色表
	BOOL m_bHasRgbQuad;
	// 位图是否有效
	BOOL m_bValid;
};

#endif // !defined(AFX_DIB_H__AC952C3A_9B6B_4319_8D6E_E7F509348A88__INCLUDED_)