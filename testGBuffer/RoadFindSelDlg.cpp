// RoadFindSelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "testGBuffer.h"
#include "RoadFindSelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRoadFindSelDlg dialog


CRoadFindSelDlg::CRoadFindSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRoadFindSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRoadFindSelDlg)
	m_c1 = _T("");
	m_c2 = _T("");
	m_c3 = _T("");
	m_c4 = _T("");
	//}}AFX_DATA_INIT
}


void CRoadFindSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRoadFindSelDlg)
	DDX_CBString(pDX, IDC_COMBO1, m_c1);
	DDX_CBString(pDX, IDC_COMBO2, m_c2);
	DDX_CBString(pDX, IDC_COMBO3, m_c3);
	DDX_CBString(pDX, IDC_COMBO4, m_c4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRoadFindSelDlg, CDialog)
	//{{AFX_MSG_MAP(CRoadFindSelDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoadFindSelDlg message handlers
