
// Test4Doc.cpp : CTestDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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


// CTestDoc ����/����

CTestDoc::CTestDoc()
{
	// TODO:  �ڴ����һ���Թ������
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

	// TODO:  �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CTestDoc ���л�

void CTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  �ڴ���Ӵ洢����
	}
	else
	{
		// TODO:  �ڴ���Ӽ��ش���
	}
}

#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CTestDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �޸Ĵ˴����Ի����ĵ�����
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

// ������������֧��
void CTestDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:     strSearchContent = _T("point;rectangle;circle;ole object;")��
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

// CTestDoc ���

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


// CTestDoc ����


BOOL CTestDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	CFile file;
	if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite))
	{//ֻ����ʽ���ļ�
		return FALSE;
	}

	DeleteContents();//ɾ���ĵ������ݣ�ȷ������ͼ������֮ǰ�ĵ�Ϊ��

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
		AfxMessageBox(_T("��ȡͼ�����"));
		return FALSE;
	}

	SetPathName(lpszPathName);//�����ļ�����
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

	LPSTR lpdib = (LPSTR)::GlobalLock(m_hDib);    // ����ͼƬ�����ڴ�һֱ��Ч
	if (m_dib->GetWidth(lpdib) > INT_MAX || m_dib->GetHeight(lpdib) > INT_MAX) // �ж�ͼ���Ƿ����
	{
		::GlobalUnlock(m_hDib);
		::GlobalFree(m_hDib);
		m_hDib = NULL;
		AfxMessageBox(_T("��ʼ��ʧ��"));
		return;
	}

	m_sizeDoc = CSize((int)m_dib->GetWidth(lpdib), (int)m_dib->GetHeight(lpdib));
	::GlobalUnlock(m_hDib);
	m_palDib = new CPalette;          // �����µ�ɫ��
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
	// TODO:  �ڴ����ר�ô����/����û���

	return CDocument::OnSaveDocument(lpszPathName);
}
