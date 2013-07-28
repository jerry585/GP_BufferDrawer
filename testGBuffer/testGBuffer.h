// testGBuffer.h : main header file for the TESTGBUFFER application
//

#if !defined(AFX_TESTGBUFFER_H__5B21B61E_DCE4_41CF_A992_36101576DD97__INCLUDED_)
#define AFX_TESTGBUFFER_H__5B21B61E_DCE4_41CF_A992_36101576DD97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
//	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestGBufferApp:
// See testGBuffer.cpp for the implementation of this class
//

class CTestGBufferApp : public CWinApp
{
public:
	CTestGBufferApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestGBufferApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CTestGBufferApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTGBUFFER_H__5B21B61E_DCE4_41CF_A992_36101576DD97__INCLUDED_)
