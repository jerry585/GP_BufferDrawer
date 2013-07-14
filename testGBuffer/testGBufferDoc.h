// testGBufferDoc.h : interface of the CTestGBufferDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTGBUFFERDOC_H__3C523FCC_75DA_44F1_AEC3_506807B717A8__INCLUDED_)
#define AFX_TESTGBUFFERDOC_H__3C523FCC_75DA_44F1_AEC3_506807B717A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTestGBufferDoc : public CDocument
{
protected: // create from serialization only
	CTestGBufferDoc();
	DECLARE_DYNCREATE(CTestGBufferDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestGBufferDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTestGBufferDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTestGBufferDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTGBUFFERDOC_H__3C523FCC_75DA_44F1_AEC3_506807B717A8__INCLUDED_)
