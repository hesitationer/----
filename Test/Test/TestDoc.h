
// Test4Doc.h : CTestDoc ��Ľӿ�
//


#pragma once

#include "Dib.h"

class CTestDoc : public CDocument
{
protected: // �������л�����
	CTestDoc();
	DECLARE_DYNCREATE(CTestDoc)

	// ����
public:
	CString m_szPathName;     // ͼƬ�ļ�·��
protected:
	CDib *m_dib;               // ����Dib����
	HGLOBAL m_hDib;           // ����Dib���
	CPalette *m_palDib;       // ����Dib��ɫ�����
	CSize m_sizeDoc;          // �ļ���С 


	// ����
public:
	void InitDib();		//��ȡDib����
	void UpdateObject(HGLOBAL hDIB);	//����DIB����
	CDib *GetDib() const        //��ȡdib����
	{
		return m_dib;
	}
	HGLOBAL GetHObject() const	//��ȡDib����ľ��
	{
		return m_hDib;
	}
	CPalette *GetDocPal() const	//��ȡ��ɫ��ָ��
	{
		return m_palDib;
	}
	CSize GetDocDimension() const//��ȡ�ļ���С
	{
		return m_sizeDoc;
	}


	// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

	// ʵ��
public:
	virtual ~CTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
};
