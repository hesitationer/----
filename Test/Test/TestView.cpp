
// Test4View.cpp : CTestView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "Test.h"
#endif

#include "TestDoc.h"
#include "TestView.h"
#include "MyDib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestView

IMPLEMENT_DYNCREATE(CTestView, CView)

BEGIN_MESSAGE_MAP(CTestView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_32771, &CTestView::OnRetinex)
	ON_COMMAND(ID_32772, &CTestView::OnDark)
END_MESSAGE_MAP()

// CTestView ����/����

CTestView::CTestView()
{
	// TODO:  �ڴ˴���ӹ������

}

CTestView::~CTestView()
{
}

BOOL CTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CTestView ����

void CTestView::OnDraw(CDC* pDC)
{
	CTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	HGLOBAL hDib = pDoc->GetHObject();

	if (hDib)   // �ж��Ƿ�Ϊ��
	{
		LPSTR lpdib = (LPSTR)::GlobalLock(hDib);
		// ��ȡdib���
		int cxdib = (int)pDoc->GetDib()->GetWidth(lpdib);
		// ��ȡdib�߶�
		int cydib = (int)pDoc->GetDib()->GetHeight(lpdib);

		double xby = (double)cxdib / (double)cydib;
		::GlobalUnlock(hDib);

		CRect rcdib;
		rcdib.top = rcdib.left = 0;
		rcdib.right = cxdib;
		rcdib.bottom = cydib;

		CRect crect;
		GetClientRect(crect);
		CRect rcDest;
		rcDest.top = (crect.bottom - rcdib.bottom) / 2;
		rcDest.left = (crect.right - rcdib.right) / 2;
		rcDest.right = rcDest.left + rcdib.right;
		rcDest.bottom = rcDest.top + rcdib.bottom;
		if (rcDest.top < 0)
		{
			rcDest.top = 0;
			rcDest.bottom = crect.bottom;
			rcDest.left = (LONG)(crect.right - rcDest.bottom * xby) / 2;
			rcDest.right = (LONG)(rcDest.left + rcDest.bottom * xby);
		}
		if (rcDest.left < 0)
		{
			rcDest.left = 0;
			rcDest.right = crect.right;
			rcDest.top = (LONG)(crect.bottom - rcDest.right / xby) / 2;
			rcDest.bottom = (LONG)(rcDest.top + rcDest.right / xby);
		}

		//  ���dib
		pDoc->GetDib()->DrawDib(pDC->m_hDC, &rcDest, pDoc->GetHObject(), &rcdib, pDoc->GetDocPal());
	}
}


// CTestView ��ӡ

BOOL CTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӵ�ӡ����е��������
}


// CTestView ���

#ifdef _DEBUG
void CTestView::AssertValid() const
{
	CView::AssertValid();
}

void CTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTestDoc* CTestView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTestDoc)));
	return (CTestDoc*)m_pDocument;
}
#endif //_DEBUG


BOOL CTestView::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return TRUE;//CView::OnEraseBkgnd(pDC);
}




void CTestView::OnRetinex()
{
	CTestDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	HGLOBAL hDib = pDoc->GetHObject();
	if (!hDib)
		return;

	CMyDib *newDib = (CMyDib*)pDoc->GetDib();
	newDib->ImageProcess(hDib);
	Invalidate(TRUE);

}


void CTestView::OnDark()
{
	CTestDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	HGLOBAL hDib = pDoc->GetHObject();
	if (!hDib)
		return;

	CMyDib *newDib = (CMyDib*)pDoc->GetDib();
	//DWORD start = GetTickCount();
	newDib->ImageProcessByDark(hDib);
	//DWORD end = GetTickCount();
	/*CFile file;
	if(!file.Open(_T("D:\\3.bmp"),CFile::modeCreate | CFile::modeWrite))
		return;
	newDib->SaveFile(hDib, file);
	file.Close();*/
	Invalidate(TRUE);
}
