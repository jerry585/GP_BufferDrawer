// testGBufferDoc.cpp : implementation of the CTestGBufferDoc class
//

#include "stdafx.h"
#include "testGBuffer.h"

#include "testGBufferDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestGBufferDoc

IMPLEMENT_DYNCREATE(CTestGBufferDoc, CDocument)

BEGIN_MESSAGE_MAP(CTestGBufferDoc, CDocument)
	//{{AFX_MSG_MAP(CTestGBufferDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestGBufferDoc construction/destruction

CTestGBufferDoc::CTestGBufferDoc()
{
	// TODO: add one-time construction code here

}

CTestGBufferDoc::~CTestGBufferDoc()
{
}

BOOL CTestGBufferDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CTestGBufferDoc serialization

void CTestGBufferDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTestGBufferDoc diagnostics

#ifdef _DEBUG
void CTestGBufferDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTestGBufferDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTestGBufferDoc commands
