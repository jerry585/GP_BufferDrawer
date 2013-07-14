#if !defined(AFX_ROADFINDSELDLG_H__2D759615_D52D_4974_AE2E_83FA4BB2CCAF__INCLUDED_)
#define AFX_ROADFINDSELDLG_H__2D759615_D52D_4974_AE2E_83FA4BB2CCAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RoadFindSelDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRoadFindSelDlg dialog

class CRoadFindSelDlg : public CDialog
{
// Construction
public:
	CRoadFindSelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRoadFindSelDlg)
	enum { IDD = IDD_DIALOG1 };
	CString	m_c1;
	CString	m_c2;
	CString	m_c3;
	CString	m_c4;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRoadFindSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRoadFindSelDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROADFINDSELDLG_H__2D759615_D52D_4974_AE2E_83FA4BB2CCAF__INCLUDED_)
