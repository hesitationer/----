
// Test4Doc.h : CTestDoc 类的接口
//


#pragma once

#include "Dib.h"

class CTestDoc : public CDocument
{
protected: // 仅从序列化创建
	CTestDoc();
	DECLARE_DYNCREATE(CTestDoc)

	// 特性
public:
	CString m_szPathName;     // 图片文件路径
protected:
	CDib *m_dib;               // 声明Dib对象
	HGLOBAL m_hDib;           // 声明Dib句柄
	CPalette *m_palDib;       // 创建Dib调色板对象
	CSize m_sizeDoc;          // 文件大小 


	// 操作
public:
	void InitDib();		//获取Dib对象
	void UpdateObject(HGLOBAL hDIB);	//更新DIB对象
	CDib *GetDib() const        //获取dib对象
	{
		return m_dib;
	}
	HGLOBAL GetHObject() const	//获取Dib对象的句柄
	{
		return m_hDib;
	}
	CPalette *GetDocPal() const	//获取调色板指针
	{
		return m_palDib;
	}
	CSize GetDocDimension() const//获取文件大小
	{
		return m_sizeDoc;
	}


	// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

	// 实现
public:
	virtual ~CTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
};
