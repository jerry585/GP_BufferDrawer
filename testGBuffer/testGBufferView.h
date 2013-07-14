// testGBufferView.h : interface of the CTestGBufferView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTGBUFFERVIEW_H__8B796525_16ED_4846_A095_8C227C884071__INCLUDED_)
#define AFX_TESTGBUFFERVIEW_H__8B796525_16ED_4846_A095_8C227C884071__INCLUDED_

#include <vector>
#include <stack>
#include <algorithm>

#include "GP_PixelBuffer.h"
#include "GP_PixelDrawer.h"
#include "GP_BitMap.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
//网格可见区域路线查找
//区块定义
typedef struct _SE{
	int SE_FID;
	int VV;
	int start;
	int end;
	std::vector<_SE*> connect;
	_SE* next;

	//最短路径使用
	int correntminlen;
	int tested;
	_SE* roadpre;
}SE;
//连接属性定义
typedef struct _ConnectRecod{
	_SE* connect_SE;
	int connect_startid;
	int connect_endid;
	int connect_roadwidth;
}ConnectRecod; 


///////////////////////////////////////////////////////////////////////////
//简单Dijkstra演示
typedef struct _EDGE 
{
	int start_node;
	int end_node;
	int edge_length;
}EDGE;

typedef struct _NODE{
	int inodeID;
	int x;
	int y;
	std::vector<EDGE> connect;

	//最短路径使用
	int correntminlen;
	int tested;
	int pre_node;
}NODE;

//////////////////////////////////////////////////////////////////////////
//动目标
typedef struct _MOVENODE{
	int x;
	int y;
	int type;
	int size;
	GP_Color color;
	_MOVENODE(){
		x = rand()%600;
		y = rand()%600;
		type = rand()%3;
		size = rand()%50;
		color = GP_Color(rand()%255,rand()%255,rand()%255,rand()%255);
	};
	void move(){
		x += rand()%2 == 0 ? rand()%40 : -rand()%40;
		y += rand()%2 == 0 ? rand()%40 : -rand()%40;
		x %= 1200;
		y %= 600;
	};
}MOVENODE;

typedef struct _GP_DPoint{
	float dx;
	float dy;
}GP_DPoint;
typedef std::vector<GP_DPoint> GP_Polygon;
typedef std::vector<GP_Polygon> GP_PolygonList;


class GP_DrawingArea;

class CTestGBufferView : public CView
{
protected: // create from serialization only
	CTestGBufferView();
	DECLARE_DYNCREATE(CTestGBufferView)

// Attributes
public:
	CTestGBufferDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestGBufferView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTestGBufferView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTestGBufferView)
	afx_msg void OnPaint();
	afx_msg void OnIdInbmp();
	afx_msg void OnIdOutbmp();
	afx_msg void OnIdLine();
	afx_msg void OnIdCircle();
	afx_msg void OnIdCirclefill();
	afx_msg void OnIdRect();
	afx_msg void OnIdRectfill();
	afx_msg void OnIdRectfillh();
	afx_msg void OnIdClear();
	afx_msg void OnIdMovbmp();
	afx_msg void OnIdLinegdi();
	afx_msg void OnIdInmovbmp();
	afx_msg void OnIdOutmovbmp();
	afx_msg void OnIdBfbmp();
	afx_msg void OnIdSfbmp();
	afx_msg void OnIdXzbmp();
	afx_msg void OnIdXlbmp();
	afx_msg void OnIdHzjz();
	afx_msg void OnIdJsjx();
	afx_msg void OnIdJslx();
	afx_msg void OnIdDjhz();
	afx_msg void OnIdDjys();
	afx_msg void OnIdPmjq();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnIdDmbks();
	afx_msg void OnIdDmbjs();
	afx_msg void OnIdDrawmap();
	afx_msg void OnIdMpzoomin();
	afx_msg void OnIdMpzoomout();
	afx_msg void OnIdFilecre();
	afx_msg void OnIdFileout();
	afx_msg void OnIdRotatebmp();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnIdFilelist();
	afx_msg void OnIdSvbmp();
	afx_msg void OnIdMapread();
	afx_msg void OnIdFsbmp();
	afx_msg void OnIdFsbmp2();
	afx_msg void OnIdArea768();
	afx_msg void OnIdArea1200();
	afx_msg void OnIdDrbmp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	GP_PixelBuffer* m_pbuffer1;
	GP_PixelDrawer* m_pdrawer1;
	GP_DrawingArea* m_pDrawingArea1;
	bool m_bmp_draw;
	bool flag;
	int m_bmp_x;
	int m_bmp_y;
	int mint[256];

	int pixeltable[8][8];

	int m_iDispScale;

	int tool_id;
	int SE_Count;
	SE** sList;
	std::vector<SE*> readytotestList;

	int FillPolygonFeature (FILE* fp1, FILE* fp2,int m_iRecordNumber);

	int ReadMapDataInternal();

	void OnMoveLeft();
	void OnMoveRight();
	void OnMoveUp();
	void OnMoveDown();

	void DrawMapGP();
	void DrawMapGGI();

	GP_PolygonList polylist;

	//////////////////////////////////////////////////////////////////////////
	//Dijkstra简单演示
	NODE nodelist[5];

	//////////////////////////////////////////////////////////////////////////
	//动目标
	MOVENODE movenodelist[1000];
	UINT movetimer;
};

#ifndef _DEBUG  // debug version in testGBufferView.cpp
inline CTestGBufferDoc* CTestGBufferView::GetDocument()
   { return (CTestGBufferDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTGBUFFERVIEW_H__8B796525_16ED_4846_A095_8C227C884071__INCLUDED_)
