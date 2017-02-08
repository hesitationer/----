
// Test4Doc.cpp : CTestDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Test.h"
#endif

#include "TestDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTestDoc

IMPLEMENT_DYNCREATE(CTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CTestDoc, CDocument)
END_MESSAGE_MAP()


// CTestDoc 构造/析构

CTestDoc::CTestDoc()
{
	// TODO:  在此添加一次性构造代码
	m_dib = NULL;
	m_hDib = NULL;
	m_palDib = NULL;
	m_sizeDoc = CSize(1, 1);
}

CTestDoc::~CTestDoc()
{
	if (m_hDib)
	{
		::GlobalFree((HGLOBAL)m_hDib);
	}
	if (m_palDib)
	{
		delete m_palDib;
		m_palDib = NULL;
	}
	if (m_dib)
	{
		delete m_dib;
		m_dib = NULL;
	}
}

BOOL CTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO:  在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CTestDoc 序列化

void CTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  在此添加存储代码
	}
	else
	{
		// TODO:  在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CTestDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CTestDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CTestDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CTestDoc 诊断

#ifdef _DEBUG
void CTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CTestDoc 命令


BOOL CTestDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	CFile file;
	if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite))
	{//只读方式打开文件
		return FALSE;
	}

	DeleteContents();//删除文档中数据，确保载入图像数据之前文档为空

	BeginWaitCursor();
	if (m_dib)
	{
		delete m_dib;
		m_dib = NULL;
	}
	m_dib = new CDib;
	m_hDib = m_dib->LoadFile(file);
	m_szPathName = lpszPathName;
	if (!m_hDib)
		return FALSE;
	InitDib();
	EndWaitCursor();

	if (!m_hDib)
	{
		AfxMessageBox(_T("读取图像出错"));
		return FALSE;
	}

	SetPathName(lpszPathName);//设置文件名称
	SetModifiedFlag(FALSE);

	return TRUE;
}

void CTestDoc::InitDib()
{
	if (!m_hDib)
		return;
	if (m_palDib)
	{
		delete m_palDib;
		m_palDib = NULL;
	}

	LPSTR lpdib = (LPSTR)::GlobalLock(m_hDib);    // 保持图片数据内存一直有效
	if (m_dib->GetWidth(lpdib) > INT_MAX || m_dib->GetHeight(lpdib) > INT_MAX) // 判断图像是否过大
	{
		::GlobalUnlock(m_hDib);
		::GlobalFree(m_hDib);
		m_hDib = NULL;
		AfxMessageBox(_T("初始化失败"));
		return;
	}

	m_sizeDoc = CSize((int)m_dib->GetWidth(lpdib), (int)m_dib->GetHeight(lpdib));
	::GlobalUnlock(m_hDib);
	m_palDib = new CPalette;          // 创建新调色板
	if (!m_palDib)
	{
		::GlobalFree(m_hDib);
		m_hDib = NULL;
		return;
	}
	if (!m_dib->ConstructPalette(m_hDib, m_palDib))
	{
		delete m_palDib;
		m_palDib = NULL;
		return;
	}
}


BOOL CTestDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO:  在此添加专用代码和/或调用基类

	return CDocument::OnSaveDocument(lpszPathName);
}
