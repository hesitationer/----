
// Test4View.cpp : CTestView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_32771, &CTestView::OnRetinex)
	ON_COMMAND(ID_32772, &CTestView::OnDark)
END_MESSAGE_MAP()

// CTestView 构造/析构

CTestView::CTestView()
{
	// TODO:  在此处添加构造代码

}

CTestView::~CTestView()
{
}

BOOL CTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CTestView 绘制

void CTestView::OnDraw(CDC* pDC)
{
	CTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	HGLOBAL hDib = pDoc->GetHObject();

	if (hDib)   // 判断是否为空
	{
		LPSTR lpdib = (LPSTR)::GlobalLock(hDib);
		// 获取dib宽度
		int cxdib = (int)pDoc->GetDib()->GetWidth(lpdib);
		// 获取dib高度
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

		//  输出dib
		pDoc->GetDib()->DrawDib(pDC->m_hDC, &rcDest, pDoc->GetHObject(), &rcdib, pDoc->GetDocPal());
	}
}


// CTestView 打印

BOOL CTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void CTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}


// CTestView 诊断

#ifdef _DEBUG
void CTestView::AssertValid() const
{
	CView::AssertValid();
}

void CTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTestDoc* CTestView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTestDoc)));
	return (CTestDoc*)m_pDocument;
}
#endif //_DEBUG


BOOL CTestView::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

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
