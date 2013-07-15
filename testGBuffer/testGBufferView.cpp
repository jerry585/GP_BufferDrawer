// testGBufferView.cpp : implementation of the CTestGBufferView class
//

#include "stdafx.h"
#include "testGBuffer.h"

#include "testGBufferDoc.h"
#include "testGBufferView.h"

#include <time.h>
#include <math.h>
#include <vector>
#include <string>

#include "RoadFindSelDlg.h"

#include "GP_Performance.h"

#include "GP_BaseTools.h"

#include "GP_DrawingArea.h"

#include "GP_Utility.h"

#include "pico/picostorage.hpp"
#include "pico/os.hpp"
#include "pico/offsetfile.hpp"

#include "bmpfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

//遍历指定目录下所有指定文件，返回文件名列表，格式限定在 "*.emp" "*.sms"
GIS_API std::vector<std::string> giseGetFileList(const char* cszParent,const char* cszFormat)
{
	std::vector<std::string> list;
#ifdef _WIN32	//WINDOWS下
	std::string strVecdataFindDir = std::string(cszParent);
	strVecdataFindDir += cszFormat;
	std::string strVectorMapDataPath = cszParent;
	int total = -1;
	WIN32_FIND_DATA wfd={0};
	wfd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
#ifndef _UNICODE
	HANDLE handle = FindFirstFile(strVecdataFindDir.c_str(),&wfd);
#else
	TCHAR tmp[1024];
	MultiByteToWideChar(CP_ACP,0,(LPCSTR)strVecdataFindDir.c_str(),-1,tmp,1024);
	HANDLE handle = FindFirstFile (tmp, &wfd);
#endif
	if (INVALID_HANDLE_VALUE == handle)
		return list;
	total++;
#ifndef _UNICODE
	char* name = wfd.cFileName;
#else
	char tmp[128];
	WideCharToMultiByte(CP_ACP,0,wfd.cFileName,-1,tmp,128,NULL,NULL);
	char* name = tmp;
#endif
	if (strcmp(name,".") != 0  && strcmp(name,"..") != 0)
	{
		std::string strEmpFilePathtemp = strVectorMapDataPath+name;
		FILE* fp = fopen(strEmpFilePathtemp.c_str(),"rb");
		if (fp)
		{
			list.push_back(name);
			fclose(fp);
		}
	}
	if (handle)
	{
		wfd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
		while(FindNextFile(handle,&wfd))
		{
			total++;
#ifndef _UNICODE
			name  = wfd.cFileName;
#else
			char tmp[128];
			WideCharToMultiByte(CP_ACP,0,wfd.cFileName,-1,tmp,128,NULL,NULL);
			name = tmp;
#endif
			if (strcmp(name,".") != 0  && strcmp(name,"..") != 0)
			{
				std::string strEmpFilePathtemp = strVectorMapDataPath+name;
				FILE* fp = fopen(strEmpFilePathtemp.c_str(),"rb");
				if (fp)
				{
					list.push_back(name);
					fclose(fp);
				}
			}
			wfd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
		}
		FindClose(handle);	
	}
	return list;
#else //VX下
	int rt;
	DIR* dirptr = opendir((char*)cszParent);
	if( dirptr == NULL){
		printf("giseGetFileList error 1\n");
		assert(false);
		return list;
	}
	struct dirent * mydirent;

	while((mydirent = readdir(dirptr))!=NULL)
	{		
		if( strcmp(mydirent->d_name,".") == 0  || strcmp(mydirent->d_name,"..") == 0 )
			continue;
		//格式检测
		if (strstr(cszFormat,".EMP")!=NULL || strstr(cszFormat,".emp")!=NULL)
		{
			if(strstr(mydirent->d_name,".EMP")!=NULL || strstr(mydirent->d_name,".emp")!=NULL)
			{
				std::string strName = mydirent->d_name;
				list.push_back(strName);
			}
		}
		else if()
		{
			if(strstr(mydirent->d_name,".SMS")!=NULL || strstr(mydirent->d_name,".sms")!=NULL)
			{
				std::string strName = mydirent->d_name;
				list.push_back(strName);
			}
		}
	}
	rt = closedir(dirptr);
	if( rt == -1)
	{
		printf("giseGetFileList error 2\n");
		assert(false);
	}
	return list;
#endif
}


/////////////////////////////////////////////////////////////////////////////
// CTestGBufferView
int g_Angle = 0;

IMPLEMENT_DYNCREATE(CTestGBufferView, CView)

BEGIN_MESSAGE_MAP(CTestGBufferView, CView)
	//{{AFX_MSG_MAP(CTestGBufferView)
	ON_WM_PAINT()
	ON_COMMAND(AFX_ID_INBMP, OnIdInbmp)
	ON_COMMAND(AFX_ID_OUTBMP, OnIdOutbmp)
	ON_COMMAND(AFX_ID_LINE, OnIdLine)
	ON_COMMAND(AFX_ID_CIRCLE, OnIdCircle)
	ON_COMMAND(AFX_ID_CIRCLEFILL, OnIdCirclefill)
	ON_COMMAND(AFX_ID_RECT, OnIdRect)
	ON_COMMAND(AFX_ID_RECTFILL, OnIdRectfill)
	ON_COMMAND(AFX_ID_RECTFILLH, OnIdRectfillh)
	ON_COMMAND(AFX_ID_CLEAR, OnIdClear)
	ON_COMMAND(AFX_ID_MOVBMP, OnIdMovbmp)
	ON_COMMAND(AFX_ID_LINEGDI, OnIdLinegdi)
	ON_COMMAND(AFX_ID_INMOVBMP, OnIdInmovbmp)
	ON_COMMAND(AFX_ID_OUTMOVBMP, OnIdOutmovbmp)
	ON_COMMAND(AFX_ID_BFBMP, OnIdBfbmp)
	ON_COMMAND(AFX_ID_SFBMP, OnIdSfbmp)
	ON_COMMAND(AFX_ID_XZBMP, OnIdXzbmp)
	ON_COMMAND(AFX_ID_XLBMP, OnIdXlbmp)
	ON_COMMAND(AFX_ID_HZJZ, OnIdHzjz)
	ON_COMMAND(AFX_ID_JSJX, OnIdJsjx)
	ON_COMMAND(AFX_ID_JSLX, OnIdJslx)
	ON_COMMAND(AFX_ID_DJHZ, OnIdDjhz)
	ON_COMMAND(AFX_ID_DJYS, OnIdDjys)
	ON_COMMAND(AFX_ID_PMJQ, OnIdPmjq)
	ON_WM_TIMER()
	ON_COMMAND(AFX_ID_DMBKS, OnIdDmbks)
	ON_COMMAND(AFX_ID_DMBJS, OnIdDmbjs)
	ON_COMMAND(AFX_ID_DRAWMAP, OnIdDrawmap)
	ON_COMMAND(AFX_ID_MPZOOMIN, OnIdMpzoomin)
	ON_COMMAND(AFX_ID_MPZOOMOUT, OnIdMpzoomout)
	ON_COMMAND(AFX_ID_FILECRE, OnIdFilecre)
	ON_COMMAND(AFX_ID_FILEOUT, OnIdFileout)
	ON_COMMAND(AFX_ID_ROTATEBMP, OnIdRotatebmp)
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_COMMAND(AFX_ID_FILELIST, OnIdFilelist)
	ON_COMMAND(AFX_ID_SVBMP, OnIdSvbmp)
	ON_COMMAND(AFX_ID_MAPREAD, OnIdMapread)
	ON_COMMAND(AFX_ID_FSBMP, OnIdFsbmp)
	ON_COMMAND(AFX_ID_FSBMP2, OnIdFsbmp2)
	ON_COMMAND(AFX_ID_AREA768, OnIdArea768)
	ON_COMMAND(AFX_ID_AREA1200, OnIdArea1200)
	ON_COMMAND(AFX_ID_DRBMP, OnIdDrbmp)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestGBufferView construction/destruction


int CTestGBufferView::FillPolygonFeature (FILE* fp1, FILE* fp2,int m_iRecordNumber)
{
	polylist.clear();
	char record[5]="",temp[100]="";

	int i,j,k,ite,id,m,contentlength,shapetype,
		NumParts,NumPoints,PointInPolygon;

	double Xmin,Ymin,Xmax,Ymax,x,y;

	bool success=false;

	vector<int> v_islandid;   //记录岛多边形的ID
	vector<int> vid;   //记录带岛多边形的ID	

	int FeatureId = 1;
	
	int index=1;

	while (index <= m_iRecordNumber)     //循环所有记录
	{
		v_islandid.clear () ;      
		vid.clear () ;

		fread(&record,sizeof(char),4,fp1);    //读记录号
		record[4]='\0';

		fread(&contentlength,sizeof(char),4,fp1);    //跳过记录长度

		fread(&shapetype,sizeof(char),4,fp1);        //跳过图形类型

		if(shapetype == 0)
		{

			index++;
			
			FeatureId++;
			
			continue;
		}

		fread(&Xmin,sizeof(double),1,fp1);     //跳过边界        
		fread(&Ymin,sizeof(double),1,fp1);		
		fread(&Xmax,sizeof(double),1,fp1);		
		fread(&Ymax,sizeof(double),1,fp1);

        fread(&NumParts,sizeof(int),1,fp1);    //读多边形的个数

		fread(&NumPoints,sizeof(int),1,fp1);   //读全部点数

		int *Parts=(int*)malloc(sizeof(int)*NumParts);

		fread(Parts,sizeof(int),NumParts,fp1);  //读出索引数组

		if (NumParts==1) //一个多边形
		{
			if(NumPoints < 3)
			{

				for (i=0; i<NumPoints; i++)
				{
					fread(&x,sizeof(double),1,fp1);   //X坐标					
					fread(&y,sizeof(double),1,fp1);   //Y坐标
				}
				FeatureId++;
				
				index++;

				continue;
			}
	
// 			if(NumPoints<4)
// 				pGeoPoly->SetPointCount(4);
// 			
// 			else
// 				pGeoPoly->SetPointCount(NumPoints);
       	
			GP_Polygon polygon;
			for (i=0; i<NumPoints; i++)
			{
				fread(&x,sizeof(double),1,fp1);   //读X坐标
				fread(&y,sizeof(double),1,fp1);   //读Y坐标
				GP_DPoint pt;
				pt.dx = x;
				pt.dy = y;

				polygon.push_back(pt);
// 				pGeoPoly->SetPoint(i,SGIS_Point((int)(x*xy_scale)-m_iXOffSet,(int)(y*xy_scale)-m_iYOffSet));				

			}

			polylist.push_back(polygon);
			FeatureId++;

			index++;
		}

		else if (NumParts>1)    //含有多个多边形
		{
			vector<int> v_PolyId(NumParts);    //记录环的ID

			v_PolyId.clear();
			
			for (i=0; i<NumParts; i++)      //循环每个环
            {

				v_PolyId[i] = FeatureId;

		 	    if (i==NumParts-1)
				   PointInPolygon=NumPoints-Parts[i];
			    
				else
				   PointInPolygon=Parts[i+1]-Parts[i];	
				
				if(PointInPolygon < 3)
				{
					//跳过点数			
					for(j=0;j<PointInPolygon;j++)
					{
						fread(&x,sizeof(double),1,fp1);							
						fread(&y,sizeof(double),1,fp1);
					}					
					
					NumParts--;
					
					continue;	
				}
				
// 				if(PointInPolygon == 3)
// 					pGeoPolygon->SetPointCount(PointInPolygon+1);
// 				else
// 					pGeoPolygon->SetPointCount(PointInPolygon);
			    GP_Polygon polygon;
				for (j=0; j<PointInPolygon; j++)    //对于环的所有点
			    {
					fread(&x,sizeof(double),1,fp1);    //读X坐标
		
					fread(&y,sizeof(double),1,fp1);    //读Y坐标	
					
					GP_DPoint pt;
					pt.dx = x;
					pt.dy = y;
					
					polygon.push_back(pt);
//                     pGeoPolygon->SetPoint(j,SGIS_Point((int)(x*xy_scale)-m_iXOffSet,(int)(y*xy_scale)-m_iYOffSet));
			    }	
				polylist.push_back(polygon);
				FeatureId++;
			}

			index++;
    	}

		free(Parts);
    }

	return FeatureId;
}

CTestGBufferView::CTestGBufferView()
{
	// TODO: add construction code here
	m_pbuffer1 = NULL;
	m_pdrawer1 = NULL;
	m_bmp_draw = false;
	flag = false;
	m_bmp_x = 0;
	m_bmp_y = 0;
	tool_id = 0;
	movetimer = 0;
	m_iDispScale = 1;
	memset(&mint[0],0,256*sizeof(int));
	for(int i = 0 ; i < 8 ; i ++)
	{
		for (int k = 0 ; k < 8 ; k ++)
		{
			time_t t;
			//srand(time(&t));
			pixeltable[i][k] = 1-abs(rand()%3-1);
		}
	}
	SE_Count = 0;
	gpPerformanceReset();

	ReadMapDataInternal();

	GP_BaseTools* pTools = GP_GetBaseTools();
	double s_ret = 0.0;
	s_ret = pTools->gp_sin(0);
	s_ret = pTools->gp_sin(30);
	s_ret = pTools->gp_sin(60);
	s_ret = pTools->gp_sin(90);
	s_ret = pTools->gp_sin(120);
	s_ret = pTools->gp_sin(150);
	s_ret = pTools->gp_sin(180);
	s_ret = pTools->gp_sin(210);
	s_ret = pTools->gp_sin(240);
	s_ret = pTools->gp_sin(270);
	s_ret = pTools->gp_sin(300);
	s_ret = pTools->gp_sin(330);
	s_ret = pTools->gp_sin(360);

	return;

	const char* fname1 = "E:\\world_shape\\world_adm0_Project.shp";
	const char* fname2 = "E:\\world_shape\\world_adm0_Project.dbf";
	FILE* fp1 = fopen(fname1,"rb");
	if (fp1 == NULL)
	{
		return;
	}
	FILE* fp2 = fopen(fname2,"rb");
	if (fp2 == NULL)
	{
		return;
	}
	char temp[1500]="",FieldType[2]="",fieldname[20]="",LayerName[100]="";
	double Xmin,Xmax,Ymin,Ymax;

	fread(temp,sizeof(char),100,fp1);  //跳过第一个图形文件头（100字节） 
	
	memcpy(&Xmin, temp+36, 8);
	memcpy(&Ymin, temp+44, 8);
	memcpy(&Xmax, temp+52, 8);
	memcpy(&Ymax, temp+60, 8);

	int shapetype = (unsigned int)temp[32];
	fread(temp,sizeof(char),32,fp2);   
	
	int headlength=(unsigned char)temp[8]+(unsigned char)temp[9]*256; 
	
	int m_iRecordNumber=(unsigned char)temp[4]+(unsigned char)temp[5]*256+(unsigned char)temp[6]*65536+(unsigned char)temp[7]*16777216;

	int m_iFieldCount = (headlength-1)/32 -1; //计算属性文件中的字段数	
	if(m_iFieldCount < 0)
	{
		strcpy(temp, "获得图层属性字段数失败! ");
		if(fp1 != NULL)
			fclose(fp1);
		if(fp2 != NULL)
			fclose(fp2);
		return;
	}
	
	for (i=0;i<m_iFieldCount;i++)
	{
		fread(&fieldname,sizeof(char),11,fp2);  //读文件中的字段名
		
// 		pstr1 = new char[strlen(fieldname) + 1] ;
// 		
// 		strcpy(pstr1, fieldname) ;
// 		
// 		v_fieldname.push_back(pstr1) ;
		
		fread(&FieldType,sizeof(char),1,fp2);  //读字段类型
		FieldType[1]='\0';
		_strlwr(FieldType);
		
		fread(temp,sizeof(int),1,fp2);   //跳过字段偏移量
		
		fread(temp,sizeof(char),1,fp2);  //读字段长度
		
// 		fieldlength=(unsigned char)temp[0];
// 		m_vFieldLength.push_back(fieldlength);
		
		fread(temp,sizeof(char),15,fp2); //跳过其他说明
		
	}
	
	 fread(temp,sizeof(char),1,fp2); //跳过终止

	FillPolygonFeature(fp1,fp2,m_iRecordNumber);



}

CTestGBufferView::~CTestGBufferView()
{
	if (m_pbuffer1)
	{
		m_pbuffer1->Release();
		m_pbuffer1 = NULL;
	}
	if (m_pdrawer1)
	{
		m_pdrawer1->Release();
		m_pdrawer1 = NULL;
	}
}

int CTestGBufferView::ReadMapDataInternal()
{
	Dir dir = openStorage("c:\\workspace\\testfile.ddd");
	if (!dir)
	{
		return 0;
	}
	File fp1 = dir.openFile("map_world.dat");
	if (!fp1)
	{
		return 0;
	}
	
	int plylistsize = 0;
	int ncount = 0;
	int offset = 0;

	ncount = fp1.pread(&plylistsize,4,offset);
	offset += ncount;
	polylist.clear();

	int i = 0;
	int j = 0;
	for (i = 0; i < plylistsize; i++)
	{
		int ptsize = 0;
		ncount = fp1.pread(&ptsize,4,offset);
		offset += ncount;
		GP_Polygon poly;
		for (j = 0; j < ptsize; j++)
		{
			int ptX = 0;
			int ptY = 0;
			ncount = fp1.pread(&ptX,4,offset);
			offset += ncount;
			ncount = fp1.pread(&ptY,4,offset);
			offset += ncount;
			GP_DPoint pt;
			pt.dx = ptX/360000.0;
			pt.dy = ptY/360000.0;
			poly.push_back(pt);
		}
		polylist.push_back(poly);
	}
	return 1;
}

BOOL CTestGBufferView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTestGBufferView drawing

void CTestGBufferView::OnDraw(CDC* pDC)
{
	CTestGBufferDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CTestGBufferView printing

BOOL CTestGBufferView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTestGBufferView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTestGBufferView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CTestGBufferView diagnostics

#ifdef _DEBUG
void CTestGBufferView::AssertValid() const
{
	CView::AssertValid();
}

void CTestGBufferView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTestGBufferDoc* CTestGBufferView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTestGBufferDoc)));
	return (CTestGBufferDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTestGBufferView message handlers

void CTestGBufferView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	//dc.Attach(::GetWindowDC(::GetDesktopWindow()));
	CRect rect;
	GetClientRect(&rect);
	//::GetWindowRect(::GetDesktopWindow(),&rect);
	dc.FillRect(&rect,&CBrush(RGB(100,100,100)));

	static bool flag2 = false;
	if (!flag2)
	{
		flag2 = true;
		flag = true;
		m_bmp_draw = false;
		
		m_pbuffer1 = new GP_PixelBuffer(1024,768);
		m_pDrawingArea1 = new GP_DrawingArea;
		m_pDrawingArea1->GP_SetDrawingArea(0,0,1024,768);
		m_pdrawer1 = new GP_PixelDrawer(m_pbuffer1);
		
		m_bmp_x = 100;
		m_bmp_y = 100;

// 		GP_BitMap bmp;
// 		bmp.LoadFromFile("D:\\ttt.bmp");
// 		int t1 = clock();
// 		m_pdrawer1->DrawBitMap(0,100,bmp,50);
// 		int t2 = clock(); 
// 		float s2 = (float)(t2-t1)/CLOCKS_PER_SEC;
// 		m_pdrawer1->DrawBitMap(101,100,bmp,100);
// 		int t3 = clock();
// 		float s3 = (float)(t3-t2)/CLOCKS_PER_SEC;
// 		m_pdrawer1->DrawBitMap(202,100,bmp,150);
// 		int t4 = clock();
// 		float s4 = (float)(t4-t3)/CLOCKS_PER_SEC;
// 		m_pdrawer1->DrawBitMap(303,100,bmp,200);
// 		int t5 = clock();
// 		float s5 = (float)(t5-t4)/CLOCKS_PER_SEC;
// 		m_pdrawer1->DrawBitMap(404,100,bmp,255);
// 		int t6 = clock();
// 		float s6 = (float)(t6-t5)/CLOCKS_PER_SEC;
// 		int t7 = clock();

// 		int t1 = clock();
// 		for (int i = 0;i< 1000;i++)
// 		{
// 			m_pdrawer1->DrawBitMap(100,10,bmp,200);
// 		}
// 		int t2 = clock();
// 		float s2 = (float)(t2-t1)/CLOCKS_PER_SEC;
// 
// 		t2 = clock();
// 		for (i = 0;i< 1000;i++)
// 		{
// 			m_pdrawer1->DrawBitMap(500,10,bmp,255); 
// 		}
// 		int t3 = clock();
// 		float s3 = (float)(t3-t2)/CLOCKS_PER_SEC;
// 
// 		CString strT;
// 		strT.Format(_T("1000 次半透明 cost time %f秒 \n1000 次不透明 cost time %f秒"),s2,s3);
//		AfxMessageBox(strT);
	}

	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);

}

void CTestGBufferView::OnIdInbmp() 
{
	if (!m_bmp_draw)
	{
		m_bmp_draw = true;
		CClientDC dc(this);
		GP_BitMap bmp;
		bmp.LoadFromFile("D:\\ttt.bmp");
		
		for (int i = 0;i<= 30;i++)
		{
			m_pdrawer1->Clear();
			Sleep(10);
			m_pdrawer1->DrawBitMap(m_bmp_x,m_bmp_y,bmp,i*255/30);
			m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
		}
	}
}

void CTestGBufferView::OnIdOutbmp() 
{
	if (m_bmp_draw)
	{
		m_bmp_draw = false;
		CClientDC dc(this);
		GP_BitMap bmp;
		bmp.LoadFromFile("D:\\ttt.bmp");

		for (int j = 30;j>= 0;j--)
		{
			m_pdrawer1->Clear();
			Sleep(10); 
			m_pdrawer1->DrawBitMap(m_bmp_x,m_bmp_y,bmp,j*255/30);
			m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
		}
	}
}

void CTestGBufferView::OnIdLine() 
{
	CClientDC dc(this);

	int t1 = clock();
	for (int i = 0;i<100000;i++)
	{
		m_pdrawer1->DrawLine(10,10,100,100,GP_Color(255,0,0),2);
	}
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	int t2 = clock(); 
	float s3 = (float)(t2-t1)/CLOCKS_PER_SEC;


	t1 = clock();
	for (i = 0;i<100000;i++)
	{
		m_pdrawer1->DrawLine(100,200,200,200,GP_Color(0,0,255),2);
	}
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	t2 = clock(); 
	float s4 = (float)(t2-t1)/CLOCKS_PER_SEC;

	t1 = clock();
	for (i = 0;i<100000;i++)
	{
		m_pdrawer1->DrawLine(200,200,200,100,GP_Color(255,255,0),2);
	}
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	t2 = clock(); 
	float s5 = (float)(t2-t1)/CLOCKS_PER_SEC;

	t1 = clock();
	for (i = 0;i<100000;i++)
	{
		m_pdrawer1->DrawBezierLine2PT(100,100,150,50,200,200,150,250,GP_Color(255,0,0));
	}
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	t2 = clock(); 
	float s6 = (float)(t2-t1)/CLOCKS_PER_SEC;

	GP_Point plist[8] = {GP_Point(300,100),GP_Point(350,150),GP_Point(400,150),GP_Point(350,125),GP_Point(350,150),GP_Point(350,200),GP_Point(400,250),GP_Point(500,200)};
	GP_Point plistL[4] = {GP_Point(100,0),GP_Point(200,50),GP_Point(150,50),GP_Point(200,150)};
	t1 = clock();
	for (i = 0;i<100000;i++)
	{
		m_pdrawer1->DrawBezierLinePTS(plist,8,GP_Color(0,0,255));
	}
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	t2 = clock(); 
	float s7 = (float)(t2-t1)/CLOCKS_PER_SEC;

// 	POINT plist2[8];
// 	plist2[0].x = 300;plist2[0].y = 100;
// 	plist2[1].x = 350;plist2[1].y = 150;
// 	plist2[2].x = 400;plist2[2].y = 150;
// 	plist2[3].x = 350;plist2[3].y = 125;
// 	plist2[4].x = 350;plist2[4].y = 150;
// 	plist2[5].x = 350;plist2[5].y = 200;
// 	plist2[6].x = 400;plist2[6].y = 250;
// 	plist2[7].x = 500;plist2[7].y = 200;
// 	dc.PolyBezier(&plist2[0],6);
	//dc.PolyDraw(&plist2[0],PT_BEZIERTO,6);

	t1 = clock();
	for (i = 0;i<100000;i++)
	{
		m_pdrawer1->DrawLineAnti(510,10,600,150,GP_Color(255,0,0),2);
	}
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	t2 = clock(); 
	float s8 = (float)(t2-t1)/CLOCKS_PER_SEC;

	t1 = clock();
	for (i = 0;i<100000;i++)
	{
		m_pdrawer1->DrawLineAnti(600,200,700,200,GP_Color(0,0,255),2);
	}
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	t2 = clock(); 
	float s9 = (float)(t2-t1)/CLOCKS_PER_SEC;

	t1 = clock();
	for (i = 0;i<100000;i++)
	{
		m_pdrawer1->DrawLineAnti(700,200,700,100,GP_Color(255,255,0),2);
	}
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	t2 = clock(); 
	float s10 = (float)(t2-t1)/CLOCKS_PER_SEC;


	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);

	

// 	CString strT;
// 	strT.Format(_T("100000 次GP画直线 cost time %f秒(斜线)"),s3);
// 	dc.TextOut(700,20,strT);
// 	strT.Format(_T("100000 次GP画直线 cost time %f秒(水平)"),s4);
// 	dc.TextOut(700,40,strT);
// 	strT.Format(_T("100000 次GP画直线 cost time %f秒(垂直)"),s5);
// 	dc.TextOut(700,60,strT);
// 	strT.Format(_T("100000 次GP画Bezier曲线 cost time %f秒(2点)"),s6);
// 	dc.TextOut(700,80,strT);
// 	strT.Format(_T("100000 次GP画Bezier曲线 cost time %f秒(4点)"),s7);
// 	dc.TextOut(700,100,strT);
// 	strT.Format(_T("100000 次GP画反走样直线 cost time %f秒(斜线)"),s8);
// 	dc.TextOut(700,120,strT);
// 	strT.Format(_T("100000 次GP画反走样直线 cost time %f秒(水平)"),s9);
// 	dc.TextOut(700,140,strT);
// 	strT.Format(_T("100000 次GP画反走样直线 cost time %f秒(垂直)"),s10);
// 	dc.TextOut(700,160,strT);

	std::string s;
	gpPerformanceStat(s);
	TGP_Performance* p = GetPerformancePtr();
	char tmp[512];
	int step = 20;
	for (TGP_Performance::iterator it = p->begin(); it != p->end(); ++it)
	{
		sprintf(tmp,"%s:%0.2fs(%d)次",(*it).first, (*it).second.tmTime.toSecondsDouble(),(*it).second.iRound);
		dc.TextOut(700,step,tmp);
		step+=20;
	}
}

void CTestGBufferView::OnIdCircle() 
{
	CClientDC dc(this);
	m_pdrawer1->DrawCircle(300,100,50,GP_Color(0,255,0));
	m_pdrawer1->DrawEllispe(500,100,100,50,GP_Color(0,0,0));
	GP_Point plist4[4] = {GP_Point(300,200),GP_Point(350,250),GP_Point(400,250),GP_Point(500,200)};
	m_pdrawer1->DrawPolygon(plist4,4,GP_Color(0,0,255));
	m_pdrawer1->FillPolygonScanLine(plist4,4,GP_Color(0,255,255));

	GP_Point plist5[5] = {GP_Point(550,200),GP_Point(600,250),GP_Point(650,50),GP_Point(650,250),GP_Point(750,200)};
	m_pdrawer1->DrawPolygon(plist5,5,GP_Color(0,255,0));
	m_pdrawer1->FillPolygonScanLine(plist5,5,GP_Color(100,255,255,0));

	GP_Point plist6[3] = {GP_Point(350,100),GP_Point(600,100),GP_Point(450,300)};
	m_pdrawer1->DrawPolygon(plist6,3,GP_Color(255,0,0));
	m_pdrawer1->FillPolygonScanLine(plist6,3,GP_Color(100,0,255,0));

	m_pdrawer1->Update(dc.GetSafeHdc(),10,10); 
}

void CTestGBufferView::OnIdCirclefill() 
{
	CClientDC dc(this);
	m_pdrawer1->FillCircle(300,25,50,GP_Color(255,0,0,255));
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
}

void CTestGBufferView::OnIdRect() 
{
	CClientDC dc(this);
	m_pdrawer1->DrawRect(50,50,100,100,GP_Color(0,255,0));
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
}

void CTestGBufferView::OnIdRectfill() 
{
	CClientDC dc(this);
	int t1 = clock();
	for (int i = 0;i<10000;i++)
	{
		m_pdrawer1->FillRect(430,0,480,50,GP_Color(255,0,0,255));
	}
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	int t2 = clock(); 
	float s1 = (float)(t2-t1)/CLOCKS_PER_SEC;

	t1 = clock();
	for (i = 0;i<10000;i++)
	{
		m_pdrawer1->FillRect(490,0,540,50,GP_Color(100,0,0,255));
	}
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	t2 = clock(); 
	float s2 = (float)(t2-t1)/CLOCKS_PER_SEC;
	
	CString strT;
	strT.Format(_T("100000 次GP填充矩形 cost time %f秒(不透明)"),s1);
	dc.TextOut(700,20,strT);
	strT.Format(_T("100000 次GP填充矩形 cost time %f秒(透明)"),s2);
	dc.TextOut(700,40,strT);
}

void CTestGBufferView::OnIdRectfillh() 
{
	CClientDC dc(this);
	m_pdrawer1->FillRect(455,25,505,75,GP_Color(100,0,255,0));
	m_pdrawer1->FillRect(480,50,530,100,GP_Color(150,50,50,50));
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
}

void CTestGBufferView::OnIdClear() 
{
	CClientDC dc(this);
	m_pdrawer1->Clear();
	m_bmp_draw = false;
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
}

void CTestGBufferView::OnIdMovbmp()  
{
	if (m_bmp_draw)
	{
		CClientDC dc(this);
		GP_BitMap bmp;
		bmp.LoadFromFile("D:\\ttt.bmp");
		if (flag)
		{
			flag = false;
			for (int j = 30;j>= 0;j--)
			{
				m_pdrawer1->Clear();
				Sleep(10); 
				m_bmp_x += 10;
// 				m_bmp_y += 10;
				m_pdrawer1->DrawBitMap(m_bmp_x,m_bmp_y,bmp);
				m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
			}
		} 
		else
		{
			flag = true;
			for (int j = 30;j>= 0;j--)
			{
				m_pdrawer1->Clear();
				Sleep(10); 
				m_bmp_x -= 10;
// 				m_bmp_y -= 10;
				m_pdrawer1->DrawBitMap(m_bmp_x,m_bmp_y,bmp);
				m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
			}
		}
	}
}

void CTestGBufferView::OnIdLinegdi() 
{
	CClientDC dc(this);
	m_bmp_draw = false;
	m_pdrawer1->Clear();
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);

	int t1 = clock();
	for (int i = 0;i<10000;i++)
	{
		m_pdrawer1->DrawLine(20,20,120,120,GP_Color(0,0,0));
		m_pdrawer1->DrawLine(120,120,120,220,GP_Color(0,0,0));
		m_pdrawer1->DrawLine(120,220,220,220,GP_Color(0,0,0));
		m_pdrawer1->DrawLine(220,220,220,120,GP_Color(0,0,0));
	}
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	int t2 = clock(); 
	
	float s1 = (float)(t2-t1)/CLOCKS_PER_SEC;

	t1 = clock();
	for (i = 0;i<10000;i++)
	{
		dc.MoveTo(220,30);
		dc.LineTo(320,130);
		dc.LineTo(320,230);
		dc.LineTo(420,230);
		dc.LineTo(420,130);
	}
	t2 = clock(); 
	float s2 = (float)(t2-t1)/CLOCKS_PER_SEC;

	CString strT;
	strT.Format(_T("10000 次GDI画线 cost time %f秒"),s2);
	dc.TextOut(700,20,strT);

	strT.Format(_T("10000 次GP 画线 cost time %f秒"),s1);
	dc.TextOut(700,40,strT);
}

void CTestGBufferView::OnIdInmovbmp() 
{
	if (!m_bmp_draw)
	{
		m_bmp_draw = true;
		CClientDC dc(this);
		GP_BitMap bmp;
		bmp.LoadFromFile("D:\\ttt.bmp");
		if (flag)
		{
			flag = false;
			for (int j = 30;j>= 0;j--)
			{
				m_pdrawer1->Clear();
				Sleep(10); 
				m_bmp_x += 10;
// 				m_bmp_y += 10;
				m_pdrawer1->DrawBitMap(m_bmp_x,m_bmp_y,bmp,(30-j)*255/30);
				m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
			}
		} 
		else
		{
			flag = true;
			for (int j = 30;j>= 0;j--)
			{
				m_pdrawer1->Clear();
				Sleep(10); 
				m_bmp_x -= 10;
// 				m_bmp_y -= 10;
				m_pdrawer1->DrawBitMap(m_bmp_x,m_bmp_y,bmp,(30-j)*255/30);
				m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
			}
		}
	}
}

void CTestGBufferView::OnIdOutmovbmp() 
{
	if (m_bmp_draw)
	{
		m_bmp_draw = false;
		CClientDC dc(this);
		GP_BitMap bmp;
		bmp.LoadFromFile("D:\\ttt.bmp");
		if (flag)
		{
			flag = false;
			for (int j = 30;j>= 0;j--)
			{
				m_pdrawer1->Clear();
				Sleep(10); 
				m_bmp_x += 10;
// 				m_bmp_y += 10;
				m_pdrawer1->DrawBitMap(m_bmp_x,m_bmp_y,bmp,j*255/30);
				m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
			}
		} 
		else
		{
			flag = true;
			for (int j = 30;j>= 0;j--)
			{
				m_pdrawer1->Clear();
				Sleep(10); 
				m_bmp_x -= 10;
// 				m_bmp_y -= 10;
				m_pdrawer1->DrawBitMap(m_bmp_x,m_bmp_y,bmp,j*255/30);
				m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
			}
		}
	}
}

void CTestGBufferView::OnIdBfbmp() 
{
	CClientDC dc(this);
	GP_BitMap bmp;
#if 1
	bmp.LoadFromFile("D:\\ttt.bmp");
	m_pdrawer1->Clear();
	m_pdrawer1->DrawBitMap(-100,-100,bmp,255);
	m_pdrawer1->DrawBitMap(500,10,50,50,100,100,bmp,100);
	m_pdrawer1->DrawBitMap(500,120,50,50,200,200,bmp);

	GP_HBITMAP  hbmp = m_pbuffer1->GetBitMapHandle();
	m_pdrawer1->StretchBitMap(710,10,100,100,bmp,250);
	m_pdrawer1->StretchBitMap(0,0,hbmp.width,hbmp.hight,bmp,250);
#else
	Dir dir = openStorage("c:\\workspace\\testfile.ddd");
	if (!dir)
	{
		return;
	}
	File fp1 = dir.openFile("pic1.bmp");
	if (!fp1)
	{
		return;
	}
	bmp.LoadFromGPFile(fp1);
	m_pdrawer1->Clear();
	m_pdrawer1->DrawBitMap(-100,-100,bmp,255);
	m_pdrawer1->DrawBitMap(500,10,50,50,100,100,bmp,100);
	m_pdrawer1->DrawBitMap(500,120,50,50,200,200,bmp);
	GP_HBITMAP  hbmp = m_pbuffer1->GetBitMapHandle();
	m_pdrawer1->StretchBitMap(710,10,100,100,bmp,250);
	m_pdrawer1->StretchBitMap(0,0,hbmp.width,hbmp.hight,bmp,250);
#endif

	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
}

void CTestGBufferView::OnIdSfbmp() 
{
	CClientDC dc(this);
	//dc.Attach(::GetWindowDC(::GetDesktopWindow()));
	GP_BitMap bmp;
	bmp.LoadFromFile("D:\\t19.bmp");
	m_pdrawer1->Clear();
	int bmpw = bmp.GetHBITMAP().width;
	int bmph = bmp.GetHBITMAP().hight;

	bmpw*=1.2;
	bmph*=1.2;

	for (int j = 60;j>= 0;j--)
	{
		m_pdrawer1->Clear();
		Sleep(10); 
		int w = sin(j*3.14/120)*bmpw;
		int h = sin(j*3.14/120)*bmph;
		m_pdrawer1->StretchBitMap(400-w/2,300 - h/2,w,h,bmp,sin(j*3.14/120)*255);
		m_pdrawer1->DrawBitMap(600,10,bmp);
		m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	}

	for (j = 0;j<= 60;j++)
	{
		m_pdrawer1->Clear();
		Sleep(10); 
		int w = sin(j*3.14/120)*bmpw;
		int h = sin(j*3.14/120)*bmph;
		m_pdrawer1->StretchBitMap(400-w/2,300 - h/2,w,h,bmp,sin(j*3.14/120)*255);
		m_pdrawer1->DrawBitMap(600,10,bmp);
		m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	}

	for (j = 60;j>= 0;j--)
	{
		m_pdrawer1->Clear();
		Sleep(10); 
		int w = sin(j*3.14/120)*bmpw;
		int h = sin(j*3.14/120)*bmph;
		m_pdrawer1->StretchBitMap(400-w/2,300 - h/2,w,h,bmp,sin(j*3.14/120)*255);
		m_pdrawer1->DrawBitMap(600,10,bmp);
		m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	}
	
	for (j = 0;j<= 60;j++)
	{
		m_pdrawer1->Clear();
		Sleep(10); 
		int w = sin(j*3.14/120)*bmpw;
		int h = sin(j*3.14/120)*bmph;
		m_pdrawer1->StretchBitMap(400-w/2,300 - h/2,w,h,bmp,sin(j*3.14/120)*255);
		m_pdrawer1->DrawBitMap(600,10,bmp);
		m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	}

	for (j = 60;j>= 0;j--)
	{
		m_pdrawer1->Clear();
		int w = (2-sin(j*3.14/120))*bmpw*60/(j+1); 
		int h = (2-sin(j*3.14/120))*bmph*60/(j+1);
		m_pdrawer1->DrawBitMap(600,10,bmp);
		m_pdrawer1->StretchBitMap(400-w/2,300 - h/2,w,h,bmp,sin(j*3.14/120)*255);
		m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	}
}

void CTestGBufferView::OnIdXzbmp() 
{
	CClientDC dc(this);
	GP_BitMap bmp;
	bmp.LoadFromFile("D:\\t19.bmp");
	m_pdrawer1->Clear();
	int bmpw = bmp.GetHBITMAP().width;
	int bmph = bmp.GetHBITMAP().hight;

	for (int i = 0;i<= 60;i++)
	{
		Sleep(10);
		m_pdrawer1->Clear(); 
		m_pdrawer1->DrawBitMap(600,10,bmp);
		m_pdrawer1->StretchBitMapRotate(600,200,bmpw,bmph,bmp,45);
 		m_pdrawer1->StretchBitMapRotate(800,200,bmpw,bmph,bmp,-45,100);
 		m_pdrawer1->StretchBitMapRotate(300,200,bmpw,bmph,bmp,sin(i*3.14159/120)*360,(60-i)*255/60);
 		m_pdrawer1->StretchBitMapRotate(400,400,bmpw,bmph,bmp,sin((60-i)*3.14159/120)*360,i*255/60);
		m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	}

	//m_pdrawer1->StretchBitMapRotate(400,400,bmpw,bmph,bmp,sin((60-i)*3.14159/120)*360,i*255/60);

}

void CTestGBufferView::OnIdXlbmp() 
{
	CClientDC dc(this);
	GP_BitMap bmp;
	bmp.LoadFromFile("D:\\t19.bmp");
	m_pdrawer1->Clear();
	int bmpw = bmp.GetHBITMAP().width;
	int bmph = bmp.GetHBITMAP().hight;

	int t1 = clock(); 
	for (int i = 0;i<= 360;i++)
	{
		m_pdrawer1->StretchBitMapRotate(600,200,bmpw,bmph,bmp,45);
	}
	int t2 = clock(); 
	float s3 = (float)(t2-t1)/CLOCKS_PER_SEC;
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	m_pdrawer1->Clear();

	t1 = clock(); 
	for ( i = 0;i<= 360;i++)
	{
		m_pdrawer1->StretchBitMapRotate(600,200,bmpw,bmph,bmp,45,100);
	}
	t2 = clock(); 
	float s4 = (float)(t2-t1)/CLOCKS_PER_SEC;
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	m_pdrawer1->Clear();


	t1 = clock(); 
	for ( i = 0;i<= 360;i++)
	{
		m_pdrawer1->DrawBitMap(-10,10,bmp);
	}
	t2 = clock(); 
	float s5 = (float)(t2-t1)/CLOCKS_PER_SEC;
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	m_pdrawer1->Clear();

	t1 = clock(); 
	for ( i = 0;i<= 1000;i++)
	{
		m_pdrawer1->DrawBitMap(-10,10,bmp,10);
	}
	t2 = clock(); 
	float s6 = (float)(t2-t1)/CLOCKS_PER_SEC;
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	m_pdrawer1->Clear();

	int wB = bmpw*1.5;
	int hB = bmph*1.5;
	t1 = clock(); 
	for ( i = 0;i<= 1000;i++)
	{
		m_pdrawer1->StretchBitMap(400,300,wB,hB,bmp);
	}
	t2 = clock(); 
	float s7 = (float)(t2-t1)/CLOCKS_PER_SEC;
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	m_pdrawer1->Clear();

	t1 = clock(); 
	for ( i = 0;i<= 1000;i++)
	{
		m_pdrawer1->StretchBitMap(400,300,wB,hB,bmp,100);
	}
	t2 = clock(); 
	float s8 = (float)(t2-t1)/CLOCKS_PER_SEC;
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	m_pdrawer1->Clear();


	int wS = bmpw*0.5;
	int hS = bmph*0.5;
	t1 = clock(); 
	for ( i = 0;i<= 1000;i++)
	{
		m_pdrawer1->StretchBitMap(400,300,wS,hS,bmp);
	}
	t2 = clock(); 
	float s9 = (float)(t2-t1)/CLOCKS_PER_SEC;
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	m_pdrawer1->Clear();

	t1 = clock(); 
	for ( i = 0;i<= 1000;i++)
	{
		m_pdrawer1->StretchBitMap(400,300,wS,hS,bmp,100);
	}
	t2 = clock(); 
	float s10 = (float)(t2-t1)/CLOCKS_PER_SEC;
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	m_pdrawer1->Clear();

	t1 = clock(); 
	for ( i = 0;i<= 1000;i++)
	{
		m_pdrawer1->DrawBitMap(500,10,50,50,100,100,bmp);
	}
	t2 = clock(); 
	float s11 = (float)(t2-t1)/CLOCKS_PER_SEC;
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	m_pdrawer1->Clear();

	t1 = clock(); 
	for (i = 0;i<= 1000;i++)
	{
		m_pdrawer1->DrawBitMap(5,10,50,50,100,100,bmp,10);
	}
	t2 = clock(); 
	float s12 = (float)(t2-t1)/CLOCKS_PER_SEC;
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
//	m_pdrawer1->Clear();

	
	CString strT;
	strT.Format(_T("1000 次绘制位图 cost time %f秒(旋转)"),s3);
	dc.TextOut(700,20,strT);
	strT.Format(_T("1000 次绘制位图 cost time %f秒(旋转,透明)"),s4);
	dc.TextOut(700,40,strT);
	strT.Format(_T("1000 次绘制位图 cost time %f秒"),s5);
	dc.TextOut(700,60,strT);
	strT.Format(_T("1000 次绘制位图 cost time %f秒(透明)"),s6);
	dc.TextOut(700,80,strT);
	strT.Format(_T("1000 次绘制位图 cost time %f秒(放大)"),s7);
	dc.TextOut(700,100,strT);
	strT.Format(_T("1000 次绘制位图 cost time %f秒(放大,透明)"),s8);
	dc.TextOut(700,120,strT);
	strT.Format(_T("1000 次绘制位图 cost time %f秒(缩小)"),s9);
	dc.TextOut(700,140,strT);
	strT.Format(_T("1000 次绘制位图 cost time %f秒(缩小,透明)"),s10);
	dc.TextOut(700,160,strT);
	strT.Format(_T("1000 次绘制位图 cost time %f秒(裁剪)"),s11);
	dc.TextOut(700,180,strT);
	strT.Format(_T("1000 次绘制位图 cost time %f秒(裁剪,透明)"),s12);
	dc.TextOut(700,200,strT);
}

void CTestGBufferView::OnIdHzjz() 
{
	//初始化网格
	for(int i = 0 ; i < 8 ; i ++)
	{
		for (int k = 0 ; k < 8 ; k ++)
		{
			pixeltable[i][k] = 1-abs(rand()%3-1);
		}
	}

	CClientDC dc(this);
	m_pdrawer1->Clear();
	int step = 50; 
	for (i = 0 ; i <= 8 ; i++) 
	{
		m_pdrawer1->DrawLine(50,50+i*step,450,50+i*step,GP_Color(0,0,0));
		m_pdrawer1->DrawLine(50+i*step,50,50+i*step,450,GP_Color(0,0,0));
	}

	for(i = 0 ; i < 8 ; i ++)
	{
		for (int k = 0 ; k < 8 ; k ++)
		{
			if (pixeltable[i][k] != 0)
			{
				m_pdrawer1->FillRect(50+i*step+1,50+k*step+1,100+i*step,100+k*step,GP_Color(255,0,0));
			}
		}
	}

	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
}

void CTestGBufferView::OnIdJsjx() 
{
	int step = 50;
	sList = new SE*[8];
	for (int i = 0 ; i < 8 ; i++)
	{
		sList[i] = NULL;
	}

	SE_Count = 0;

	//列扫描，划分独立区块
	for (int iRow = 0 ; iRow < 8 ; iRow++)
	{
		int line = 0;
		int start = 0;
		int end = 0;
		static flag = true;
		while (line != 9)
		{
			if (flag)
			{
				if (line == 8)
				{
					flag = false;
					end = line;
					SE* senew = new SE;
					senew->start = start;
					senew->end = end;
					senew->next = NULL;
					senew->SE_FID = SE_Count++;
					senew->correntminlen = 9999;
					senew->tested = 0;
					senew->VV = iRow;
					senew->roadpre = NULL;
					SE* p = sList[iRow];
					SE* pre = NULL;
					while(p!=NULL)
					{
						pre = p;
						p = p->next;
					}
					if (pre==NULL)
					{
						sList[iRow] = senew;
					}
					else
					{
						pre->next = senew;
					}
					break;
				}

				if (pixeltable[iRow][line] != 1)
				{
					line ++;
				}
				else
				{
					flag = false;
					end = line;

					if (start == end)
					{
						continue;
					}

					SE* senew = new SE;
					senew->start = start;
					senew->end = end;
					senew->next = NULL;
					senew->SE_FID = SE_Count++;
					senew->correntminlen = 9999;
					senew->tested = 0;
					senew->VV = iRow;
					senew->roadpre = NULL;
					SE* p = sList[iRow];
					SE* pre = NULL;
					while(p!=NULL)
					{
						pre = p;
						p = p->next;
					}
					if (pre==NULL)
					{
						sList[iRow] = senew;
					}
					else
					{
						pre->next = senew;
					}
				}
			}
			else
			{
				if (line == 8)
				{
					break;
				}
				if (pixeltable[iRow][line] == 1)
				{
					line ++;
				}
				else
				{
					flag = true;
					start = line;
				}
			}
		}
	}

	//计算联通性
	for (iRow = 0 ; iRow < 7 ; iRow++)
	{
		SE* p = sList[iRow];
		while(p!=NULL){
			for (int k = p->start ; k < p->end ; k++) 
			{
				if (pixeltable[iRow+1][k] == 0)
				{
					SE* nextrow = sList[iRow+1];
					while(nextrow != NULL){
						if (k>=nextrow->start &&k< nextrow->end)
						{
							//记录连通属性
							if (std::find(p->connect.begin(),p->connect.end(),nextrow) == p->connect.end())
							{
								p->connect.push_back(nextrow);
							}		
							//双向连通记录
							if (std::find(nextrow->connect.begin(),nextrow->connect.end(),p) == nextrow->connect.end())
							{
								nextrow->connect.push_back(p);
							}		
						}
						nextrow = nextrow->next;
					}
				}
			}
			p = p->next;
		}
	}

	//绘制分块
	static int  num = 0;
	for (iRow = 0 ; iRow < 8 ; iRow++)
	{
		GP_Color colorsel(rand()%255,rand()%255,rand()%255);
		SE* p = sList[iRow];
		while(p!=NULL){
			if (p->start != p->end)
			{
				m_pdrawer1->FillRect(50+iRow*step+2,50+p->start*step+2,100+iRow*step-2,100+(p->end-1)*step-2,colorsel);
			}
			p = p->next;
		}
	}
	//绘制连通区域
// 	for (iRow = 0 ; iRow < 8 ; iRow++)
// 	{
// 		SE*p = sList[iRow];
// 		while(p!=NULL){
// 			if (p->start != p->end)
// 			{
// 				std::vector<_SE*>::iterator Iter;
// 				for (Iter = p->connect.begin() ; Iter != p->connect.end() ; Iter++)
// 				{
// 					_SE* connect_item = *Iter;
// 					int connect_startid = p->start < connect_item->start ? connect_item->start : p->start;
// 					int connect_endid = p->end < connect_item->end ? p->end : connect_item->end;
// 					int rr = connect_item->VV;
// 					m_pdrawer1->FillRect(50+rr*step+2+40,50+connect_startid*step+2,100+rr*step-2+10,100+(connect_endid-1)*step-2,GP_Color(0,255,0));
// 				}
// 			}
// 			p = p->next;
// 		}
// 	}

	//清理数据
// 	for (iRow = 0 ; iRow < 8 ; iRow++)
// 	{
// 		SE* p = sList[iRow];
// 		SE* pre = NULL;
// 		while(p!=NULL){
// 			pre = p;
// 			p = p->next;
// 			delete pre;
// 		}
// 		sList[iRow] = NULL;
// 	}
	CClientDC dc(this);
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
}

void CTestGBufferView::OnIdJslx() 
{
	// TODO: Add your command handler code here
	CRoadFindSelDlg dlg;
	int start_H;
	int start_V;
	int end_H;
	int end_V;
	if (dlg.DoModal() == IDOK)
	{
		CString strT;
		start_H = atoi(dlg.m_c1) - 1;
		start_V = atoi(dlg.m_c2) -1;
		end_H = atoi(dlg.m_c3) - 1;
		end_V = atoi(dlg.m_c4) -1;
		strT.Format(_T("起点:%d %d\n终点:%d %d %d"),start_H+1,start_V+1,end_H+1,end_V+1,SE_Count);
 		AfxMessageBox(strT);

		if (start_V > end_V)
		{
			int tempH = end_H;
			int tempV = end_V;
			end_H = start_H;
			end_V = start_V;
			start_H = tempH;
			start_V = tempV;
		}
	}
	else
	{
		return;
	}

	if (pixeltable[start_V][start_H] == 1 || pixeltable[end_V][end_H] == 1)
	{
		AfxMessageBox("此路不通！");
		return;
	}
	int start_SE_FID;
	int end_SE_FID;
	
	//建立邻接表
	SE** adjastList = new SE*[SE_Count];
	for (int i = 0; i <SE_Count ; i++)
	{
		adjastList[i] = NULL;
	}
	for (int iRow = 0 ; iRow < 8 ; iRow++)
	{
		SE* p = sList[iRow];
		while(p!=NULL){
			adjastList[p->SE_FID] = p;
			if (start_V == iRow && start_H<p->end && start_H >= p->start)
			{
				start_SE_FID = p->SE_FID;
			}
			if (end_V == iRow && end_H<p->end && end_H >= p->start)
			{
				end_SE_FID = p->SE_FID;
			}
			p = p->next;
		}
	}

	CString strT;
	strT.Format(_T("起点FID:%d\n终点FID:%d"),start_SE_FID,end_SE_FID);
 	AfxMessageBox(strT);

	//Dijkstra计算最短路径
	adjastList[start_SE_FID]->tested = 2;
	adjastList[start_SE_FID]->correntminlen = 0;

	int testID = start_SE_FID;
	readytotestList.push_back(adjastList[testID]);
	
	do
	{
		//////////////////////////////////////////////////////////////////////////
		//找到需要拓展的节点
		std::vector<_SE*>::iterator Iter2;
		std::vector<_SE*>::iterator Iter_needtotest;
		Iter_needtotest = readytotestList.begin();
		for (Iter2 = readytotestList.begin() ; Iter2 != readytotestList.end() ; Iter2++)
		{
			if ((*Iter2)->correntminlen < (*Iter_needtotest)->correntminlen)
			{
				Iter_needtotest = Iter2;
			}
		}
		testID = (*Iter_needtotest)->SE_FID;
		readytotestList.erase(Iter_needtotest);
		//////////////////////////////////////////////////////////////////////////
		//拓展节点
		std::vector<_SE*>::iterator Iter;
		for (Iter = adjastList[testID]->connect.begin() ; Iter != adjastList[testID]->connect.end() ; Iter++)
		{
			if ((*Iter)->tested == 0)
			{
				if (adjastList[testID]->correntminlen+1 < (*Iter)->correntminlen)
				{
					(*Iter)->roadpre = adjastList[testID];
					(*Iter)->correntminlen = adjastList[testID]->correntminlen+1;
				}
				if ((*Iter)->tested != 1)
				{
					std::vector<_SE*>::iterator Iter3;
					bool alreadyin = false;
					for (Iter3 = readytotestList.begin() ; Iter3 != readytotestList.end() ; Iter3++)
					{
						if (*Iter3 == *Iter)
						{
							alreadyin = true;
						}
					}
					if (!alreadyin)
					{
						readytotestList.push_back(*Iter);
					}
				}
			}
		}
		adjastList[testID]->tested = 1;
	}while (testID != end_SE_FID && readytotestList.size() != 0);

	if (adjastList[end_SE_FID]->tested != 1)
	{
		AfxMessageBox("此路不通！");
		return;
	}


	//绘制结果路线
	int step = 50;

	m_pdrawer1->FillRect(550,50,1000,1000,GP_Color(255,255,255));
	SE* p = adjastList[end_SE_FID];
	while(p!=NULL){
		m_pdrawer1->FillRect(50+p->VV*step+2+500,50+p->start*step+2,100+p->VV*step-2+500,100+(p->end-1)*step-2,GP_Color(100,100,100,100));
		p = p->roadpre;
	}
	
	CClientDC dc(this);
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
}

void CTestGBufferView::OnIdDjhz() 
{
	//////////////////////////////////////////////////////////////////////////
	//对照算法导论 图 24-6
	//
	//           2    4
	//      0
	//           1    3
	//
	for (int i = 0 ; i < 5 ; i ++)
	{
		nodelist[i].inodeID = i;
		nodelist[i].tested = 0;
		nodelist[i].pre_node = -1;
		nodelist[i].correntminlen = 99999;
	}

	EDGE edge;
	//////////////////////////////////////////////////////////////////////////
	//点1的2条出边
	edge.start_node = 0;
	edge.end_node = 1;
	edge.edge_length = 5;
	nodelist[0].connect.push_back(edge);
	edge.start_node = 0;
	edge.end_node = 2;
	edge.edge_length = 10;
	nodelist[0].connect.push_back(edge);
	//////////////////////////////////////////////////////////////////////////
	//点2的3条出边
	edge.start_node = 1;
	edge.end_node = 2;
	edge.edge_length = 3;
	nodelist[1].connect.push_back(edge);
	edge.start_node = 1;
	edge.end_node = 3;
	edge.edge_length = 2;
	nodelist[1].connect.push_back(edge);
	edge.start_node = 1;
	edge.end_node = 4;
	edge.edge_length = 9;
	nodelist[1].connect.push_back(edge);
	//////////////////////////////////////////////////////////////////////////
	//点3的2条出边
	edge.start_node = 2;
	edge.end_node = 1;
	edge.edge_length = 2;
	nodelist[2].connect.push_back(edge);
	edge.start_node = 2;
	edge.end_node = 4;
	edge.edge_length = 1;
	nodelist[2].connect.push_back(edge);
	//////////////////////////////////////////////////////////////////////////
	//点4的2条出边
	edge.start_node = 3;
	edge.end_node = 0;
	edge.edge_length = 7;
	nodelist[3].connect.push_back(edge);
	edge.start_node = 3;
	edge.end_node = 4;
	edge.edge_length = 6;
	nodelist[3].connect.push_back(edge);
	//////////////////////////////////////////////////////////////////////////
	//点5的1条出边
	edge.start_node = 4;
	edge.end_node = 3;
	edge.edge_length = 4;
	nodelist[4].connect.push_back(edge);
	
	
	//初始化点的位置，与算法本身无关
	nodelist[0].x = 200;
	nodelist[0].y = 200;
	nodelist[1].x = 400;
	nodelist[1].y = 300;
	nodelist[2].x = 400;
	nodelist[2].y = 100;
	nodelist[3].x = 600;
	nodelist[3].y = 300;
	nodelist[4].x = 600;
	nodelist[4].y = 100;


	//////////////////////////////////////////////////////////////////////////
	//绘制网络
	m_pdrawer1->Clear();
	for (i = 0 ;  i < 5 ; i ++)
	{
		m_pdrawer1->DrawRect(nodelist[i].x-25,nodelist[i].y-25,nodelist[i].x+25,nodelist[i].y+25,GP_Color(0,0,0));
	}
	for (i = 0 ;  i < 5 ; i ++)
	{
		std::vector<EDGE>::iterator Iter;
		for (Iter = nodelist[i].connect.begin() ;Iter != nodelist[i].connect.end() ;  Iter++ )
		{
			m_pdrawer1->DrawLine(nodelist[i].x,nodelist[i].y,nodelist[(*Iter).end_node].x,nodelist[(*Iter).end_node].y,GP_Color(0,0,0));
		}
	}
	CClientDC dc(this);
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
}

void CTestGBufferView::OnIdDjys() 
{
	//////////////////////////////////////////////////////////////////////////
	//Dijkstra计算

	//从节点0开始找
	//////////////////////////////////////////////////////////////////////////
	//可以修改
	int end_SE_FID = 4;			//目标结点5
	int start_SE_FID = 0;		//起始节点1

	nodelist[start_SE_FID].correntminlen = 0;	//首先将起点当前最优路径长度值置为0

	int testID = start_SE_FID;					

	std::vector<NODE> testList;
	testList.push_back(nodelist[testID]);		//将起点压入拓展队列中
	
	do
	{
		//////////////////////////////////////////////////////////////////////////
		//找到需要拓展的节点
		std::vector<NODE>::iterator Iter2;
		std::vector<NODE>::iterator Iter_needtotest;
		Iter_needtotest = testList.begin();
		for (Iter2 = testList.begin() ; Iter2 != testList.end() ; Iter2++)
		{
			if ((*Iter2).correntminlen < (*Iter_needtotest).correntminlen)
			{
				Iter_needtotest = Iter2;
			}
		}
		testID = (*Iter_needtotest).inodeID;
		testList.erase(Iter_needtotest);
		//////////////////////////////////////////////////////////////////////////
		//拓展节点
		std::vector<EDGE>::iterator Iter;
		for (Iter = nodelist[testID].connect.begin() ; Iter != nodelist[testID].connect.end() ; Iter++)
		{

			if (nodelist[(*Iter).end_node].tested == 0)
			{
				//拓展后更新目标结点
				if (nodelist[testID].correntminlen+(*Iter).edge_length < nodelist[(*Iter).end_node].correntminlen)
				{
					nodelist[(*Iter).end_node].pre_node = testID;
					nodelist[(*Iter).end_node].correntminlen = nodelist[testID].correntminlen+(*Iter).edge_length;
				}
				//将新节点加入拓展队列
				if (nodelist[(*Iter).end_node].tested != 1)
				{
					std::vector<NODE>::iterator Iter3;
					bool alreadyin = false;
					for (Iter3 = testList.begin() ; Iter3 != testList.end() ; Iter3++)
					{
						if ((*Iter3).inodeID == (*Iter).end_node)
						{
							alreadyin = true;
						}
					}
					if (!alreadyin)
					{
						testList.push_back(nodelist[(*Iter).end_node]);
					}
				}
			}
		}
		nodelist[testID].tested = 1;
		//////////////////////////////////////////////////////////////////////////
		//绘制拓展顺序
		m_pdrawer1->FillRect(nodelist[testID].x-25+2,nodelist[testID].y-25+2,nodelist[testID].x+25-2,nodelist[testID].y+25-2,GP_Color(100,100,100,100));
		CClientDC dc(this);
		m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
		Sleep(1000);
		//////////////////////////////////////////////////////////////////////////
	}while (testID != end_SE_FID && testList.size() != 0);
	
	if (nodelist[end_SE_FID].tested != 1)
	{
		AfxMessageBox("此路不通！");
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	//追溯最优路径
	std::stack<int> roadstack;
	int id = end_SE_FID;
	while(id != -1){
		roadstack.push(id);
		id = nodelist[id].pre_node;
	}
	
	char buffer[256] = {0};
	sprintf(buffer,"最优路径为：");
	while (roadstack.size() != 0)
	{
		int nodeid = roadstack.top();
		roadstack.pop();
		char buffer2[10] = {0};
		sprintf(buffer2,"%d ",nodeid);
		strcat(buffer,buffer2);
	}
	CString strT;
	strT.Format(_T("找到！\n%s"),buffer);
	AfxMessageBox(strT);
}

void CTestGBufferView::OnIdPmjq() 
{
	CDC* dc = new CDC;
	dc->Attach(::GetWindowDC(GetWindow(NULL)->GetParentFrame()->GetSafeHwnd()));
	CClientDC mdc(this);

	CRect rt;
	GetWindow(NULL)->GetParentFrame()->GetWindowRect(&rt);

	int x = rt.Width();
	int y = rt.Height();
	mdc.StretchBlt(20,20,600,600*y/x,dc,0,0,x,y,SRCCOPY); 

// 	Sleep(1000);
// 
//  	dc->Attach(::GetWindowDC(::GetDesktopWindow()));
// 	::GetWindowRect(::GetDesktopWindow(),&rt);
// 	x = rt.Width();
// 	y = rt.Height();
// 	mdc.StretchBlt(20,20,600,600*y/x,dc,0,0,x,y,SRCCOPY); 

}

void CTestGBufferView::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == movetimer)
	{
		m_pdrawer1->Clear();
		for (int i = 0; i < 1000 ; i++)
		{
			if (movenodelist[i].type == 0)
			{
				m_pdrawer1->FillRect(movenodelist[i].x,movenodelist[i].y,movenodelist[i].x+movenodelist[i].size,movenodelist[i].y+movenodelist[i].size,movenodelist[i].color);
			}
			else if (movenodelist[i].type == 1)
			{
				m_pdrawer1->FillCircle(movenodelist[i].x,movenodelist[i].y,movenodelist[i].size,movenodelist[i].color);
			}
			else
			{
				GP_Point pt[3];
				pt[0].x = movenodelist[i].x;
				pt[0].y = movenodelist[i].y;
				pt[1].x = movenodelist[i].x+movenodelist[i].size;
				pt[1].y = movenodelist[i].y+movenodelist[i].size;
				pt[2].x = movenodelist[i].x-movenodelist[i].size;
				pt[2].y = movenodelist[i].y+movenodelist[i].size;
				m_pdrawer1->FillPolygonScanLine(&pt[0],3,movenodelist[i].color);
			}
			movenodelist[i].move();
		}
		CClientDC dc(this);
		m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	}
	CView::OnTimer(nIDEvent);
}

void CTestGBufferView::OnIdDmbks() 
{
	movetimer = ::SetTimer (this->GetSafeHwnd(), 1,200, NULL) ;
}

void CTestGBufferView::OnIdDmbjs() 
{
	::KillTimer (this->GetSafeHwnd(), movetimer) ;
}

void CTestGBufferView::DrawMapGP(){
	vector<GP_Polygon>::iterator Iterlist;
	for (Iterlist = polylist.begin();Iterlist!=polylist.end();Iterlist++)
	{
		vector<GP_DPoint>::iterator Iterpoly;
		GP_Polygon poly = *Iterlist;
		GP_Point* ptlist = new GP_Point[poly.size()];
		int i = 0;
		int iBS = 360000;//m_iDispScale;
		for (Iterpoly = poly.begin();Iterpoly!=poly.end();Iterpoly++)
		{
			GP_DPoint dpt = *Iterpoly;
			ptlist[i].x = (dpt.dx+180)*iBS;
			ptlist[i].y = 180*iBS - (dpt.dy+90)*iBS;
			if (i>1 && ptlist[i].y == ptlist[i-1].y && ptlist[i-2].y == ptlist[i-1].y )
			{
				ptlist[i-1].x = ptlist[i].x;
			}
			else if (i>0 && ptlist[i].y == ptlist[i-1].y && ptlist[i].x == ptlist[i-1].x)
			{
				continue;
			}
			else
			{
				i++;
			}
		}
		//GP_GetBaseTools()->gp_rotate_points(180*iBS,90*iBS,(int*)ptlist,i,g_Angle);
		m_pDrawingArea1->GP_WindowToClient((int*)ptlist,i);
		m_pdrawer1->FillPolygonScanLine(ptlist,i,GP_Color(rand()%255,rand()%255,rand()%255));
		//m_pdrawer1->DrawPolygon(ptlist,i,/*GP_Color(rand()%255,rand()%255,rand()%255)*/GP_Color(0,255,0));
		delete[] ptlist; 
	}
}

void CTestGBufferView::DrawMapGGI(){
}



void CTestGBufferView::OnIdDrawmap() 
{
	CClientDC dc(this); 
	m_pdrawer1->Clear();

	static bool flag = true;
	if (flag)
	{
		m_pDrawingArea1->GP_SetWindowArea(0,0,360*360000,180*360000);
		m_iDispScale = 1;
		flag = false;
	}
	gpPerformanceReset();

	int i = 0;
	int n = 1;

	for ( i = 0 ; i < n ; i++)
	{
		DrawMapGP();
	}
	

	m_pdrawer1->Update(dc.GetSafeHdc(),10,10); 

// 	m_pdrawer1->Clear();
// 
// 	for ( i = 0 ; i < n ; i++)
// 	{
// 		DrawMapGGI();
// 	}
// 
// 	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);

	std::string s;
	gpPerformanceStat(s);
	TGP_Performance* p = GetPerformancePtr();
	char tmp[512];
	int step = 20;
	for (TGP_Performance::iterator it = p->begin(); it != p->end(); ++it)
	{
		sprintf(tmp,"%s:%0.2fs(%d)次",(*it).first, (*it).second.tmTime.toSecondsDouble(),(*it).second.iRound);
		dc.TextOut(700,step,tmp);
		step+=20;
	}
}

void CTestGBufferView::OnIdMpzoomin() 
{
	// TODO: Add your command handler code here
	m_iDispScale+=1;
// 	if (m_iDispScale > 10)
// 	{
// 		m_iDispScale = 10;
// 	}
// 	else
// 	{
		//m_pDrawingArea1->GP_Zoom(2);
// 		int x = 116.4211*360000+180*360000;
// 		int y = 90*360000-39.8711*360000;
		int x = 121.4856*360000+180*360000;
		int y = 90*360000-31.0933*360000;
		m_pDrawingArea1->GP_ZoomToWindow(x,y,2);
//	}


	OnIdDrawmap();
}

void CTestGBufferView::OnIdMpzoomout() 
{
	// TODO: Add your command handler code here
	m_iDispScale-=1;
// 	if (m_iDispScale < 0)
// 	{
// 		m_iDispScale = 0;
// 	}
// 	else
// 	{
		m_pDrawingArea1->GP_Zoom(0.5);
//	}
	OnIdDrawmap();
}

void CTestGBufferView::OnIdFilecre() 
{
	// TODO: Add your command handler code here
	bool ret = true;
// 	ret = createStorage("d:\\testfile.ddd");
// 	if (!ret)
// 	{
// 		return;
// 	}
	Dir dir = openStorage("c:\\workspace\\testfile.ddd");
	if (!dir)
	{
		return;
	}
	OffsetFile<File> fp1 = dir.createFile("pic1.bmp");
	if (!fp1)
	{
		return;
	}

	int fid = 0;
	bool breadonly = false;
	ret = os_open("d:\\ttt.bmp",OSMODE_EXISTING,&fid,&breadonly);
	if (!ret)
	{
		return;
	}

	unsigned char buffer[1024] = {0};
	int ncount = 0;
	int offset = 0;
	os_pread(fid,buffer,1024,offset,&ncount);
	while (ncount!=0)
	{
		fp1.write(buffer,ncount);
		offset+=ncount;
		os_pread(fid,buffer,1024,offset,&ncount);
		if (GetLastError() != 0)
		{
			SetLastError(0);
			break;
		}
	}

	dir.commit();
	os_close(fid);

// 	bool ret = createStorage("d:\\testfile.ddd");
// 	if (!ret)
// 	{
// 		return;
// 	}
// 	Dir dir = openStorage("d:\\testfile.ddd");
// 	if (!dir)
// 	{
// 		return;
// 	}
// 	OffsetFile<File> fp1 = dir.createFile("map_world.dat");
// 	if (!fp1)
// 	{
// 		return;
// 	}
// 
// 	int plysize = polylist.size();
// 	fp1.write(&plysize,4);
// 
// 	vector<GP_Polygon>::iterator Iterlist;
// 	for (Iterlist = polylist.begin();Iterlist!=polylist.end();Iterlist++)
// 	{
// 		vector<GP_DPoint>::iterator Iterpoly;
// 		GP_Polygon poly = *Iterlist;
// 		int ptsize = poly.size();
// 		fp1.write(&ptsize,4);
// 		for (Iterpoly = poly.begin();Iterpoly!=poly.end();Iterpoly++)
// 		{
// 			GP_DPoint dpt = *Iterpoly;
// 			int ptX = dpt.dx * 360000;
// 			int ptY = dpt.dy * 360000;
// 			fp1.write(&ptX,4);
// 			fp1.write(&ptY,4);
// 		}
// 	}
// 
// 	dir.commit();
	
}

void CTestGBufferView::OnIdFileout() 
{
	Dir dir = openStorage("c:\\workspace\\testfile.ddd");
	if (!dir)
	{
		return;
	}
	File fp1 = dir.openFile("pic1.bmp");
	if (!fp1)
	{
		return;
	}

	int fid = 0;
	bool breadonly = false;
	os_open("d:\\ttt2.bmp",OSMODE_EXISTING_OR_CREATE,&fid,&breadonly);

	unsigned char buffer[1024] = {0};
	int ncount = 0;
	int offset = 0;
	while((ncount = fp1.pread(buffer,1024,offset))>0){
		os_pwrite(fid,buffer,ncount,offset);
		offset+=ncount;
	}
	os_close(fid);
}

void CTestGBufferView::OnIdRotatebmp() 
{
	g_Angle -= 30;
	while (g_Angle<0) g_Angle+=360;
	while (g_Angle>359) g_Angle-=360;
	OnIdDrawmap();
}

void CTestGBufferView::OnMoveLeft(){
	m_pDrawingArea1->GP_MoveClient(-20,0);
	OnIdDrawmap();
}
void CTestGBufferView::OnMoveRight(){
	m_pDrawingArea1->GP_MoveClient(20,0);
	OnIdDrawmap();
}

void CTestGBufferView::OnMoveUp(){
	m_pDrawingArea1->GP_MoveClient(0,-20);
	OnIdDrawmap();
}
void CTestGBufferView::OnMoveDown(){
	m_pDrawingArea1->GP_MoveClient(0,20);
	OnIdDrawmap();
}

void CTestGBufferView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
// 	if (nChar == 'r' || nChar == 'R')
// 		OnIdRotatebmp();
// 	else if(nChar == VK_ADD)
// 		OnIdMpzoomin();
// 	else if(nChar == VK_SUBTRACT)
// 		OnIdMpzoomout();
// 	else if(nChar == VK_LEFT)
// 		OnMoveLeft();
// 	else if(nChar == VK_RIGHT)
// 		OnMoveRight();
// 	else if(nChar == VK_UP)
// 		OnMoveUp();
// 	else if(nChar == VK_DOWN)
// 		OnMoveDown();
	if (nChar == 'z' || nChar == 'Z')
		for (int i = 0; i < 20;i++)
		{
			if (i<10)
			{
				OnIdMpzoomin();
			} 
			else
			{
				OnIdMpzoomout();
			}
		}


	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CTestGBufferView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if (nChar == 'r' || nChar == 'R')
		OnIdRotatebmp();
	else if(nChar == VK_ADD)
		OnIdMpzoomin();
	else if(nChar == VK_SUBTRACT)
		OnIdMpzoomout();
	else if(nChar == VK_LEFT)
		OnMoveLeft();
	else if(nChar == VK_RIGHT)
		OnMoveRight();
	else if(nChar == VK_UP)
		OnMoveUp();
	else if(nChar == VK_DOWN)
		OnMoveDown();
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTestGBufferView::OnIdFilelist() 
{
	// TODO: Add your command handler code here
	Dir dir = openStorage("c:\\workspace\\testfile.ddd");
	if (!dir)
	{
		return;
	}
	//dir.
	File fp1 = dir.openFile("pic1.bmp");
	if (!fp1)
	{
		return;
	}
}

void CTestGBufferView::OnIdSvbmp() 
{
	GP_HBITMAP handle = m_pbuffer1->GetBitMapHandle();
	bmp_save_to_file("c:\\workspace\\GPBuffer.bmp",handle.width,handle.hight,handle.depth,handle.width*handle.depth/8,handle.pBits,handle.pixelformat);
}

void CTestGBufferView::OnIdMapread() 
{
	CClientDC dc(this); 

	Dir dir = openStorage("D:\\china.emp");
	if (!dir)
	{
		return;
	}

	PFile fpIni = dir.openFile("vec_layers.ini");
	if (!fpIni)
	{
		printf("文件[%s]不是一个有效的map文件!\n","D:\\china.emp"); return ;
	}
	
	
	std::string str;
	//将ini文件一次读入到str中。
	strfread(str, fpIni, fpIni.getSize());
	
	//初始可见范围，旋转角度。
	const char* p;
	p = &str[0];

	static const char magic[] = "GISE_MAP";
	static const char magic_old[] = "GISEMAP";
	static const int GISE_MAP_VERSION = 100; //1.0
	
	GP_RECT bound;
	int level = 0xff;
	int version = GISE_MAP_VERSION;
	if (strncmp(p,magic_old,strlen(magic_old))==0)
	{
		version = 50;		
		p += strlen(magic_old);	
		p+= sizeof(int) * 5; //范围，角度
	}
	else if (strncmp(p,magic,strlen(magic))==0)
	{
		p += strlen(magic);	
		version = strReadVal<int>(p,0); p+=sizeof(int);
		bound.left = strReadVal<int>(p,0); p+=sizeof(int);
		bound.top = strReadVal<int>(p,0); p+=sizeof(int);
		bound.right = strReadVal<int>(p,0); p+=sizeof(int);
		bound.bottom = strReadVal<int>(p,0); p+=sizeof(int);
		level = *p++;
		p+= (sizeof(int) * 5); 
	}
	else
	{
		printf("文件格式错误！\n");
		return ;
	}

	vector<string> layerlist;
	int step = 20;
	char tmp[256] = {0};
	
	const char* p_colon = NULL;
	char lname[128];
	bool b_visible;
	int i = 0;
	while(1)
	{
		int len = *p++;
		memcpy(lname,p,len);
		lname[len] = 0x00;
		p+= len;
		if (*p++ != ':')
		{
			return ;
		}
		
		b_visible = (*p++ == '1');
		
		if (*p++ != '|')
		{
			return ;
		}
		
		if (i == 0)
		{
			printf("序号\t图层名称\n");
			printf("===================\n");
		}
	//	printf(" %d:\t%s\n",i++, lname);
		layerlist.push_back(lname);
// 		memset(tmp,0,256);
// 		sprintf(tmp,"%d:\t%s\n",i++, lname);
// 		dc.TextOut(700,step,tmp);
// 		step+=20;
		
		if (p - &str[0] >= str.size())
			break;
	}
	
// 	memset(tmp,0,256);
// 	sprintf(tmp,"图层范围: left=%d,top=%d,right=%d,bottom=%d\n",bound.left,bound.top,bound.right,bound.bottom);
// 	dc.TextOut(700,step,tmp);
// 	step+=20;

	int ptlist[] = {bound.left+180*360000,90*360000-bound.top
		,bound.right+180*360000,90*360000-bound.top
		,bound.right+180*360000,90*360000-bound.bottom
		,bound.left+180*360000,90*360000-bound.bottom};

	m_pDrawingArea1->GP_WindowToClient((int*)&ptlist[0],4);
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
	
	//printf("\n图层范围: left=%d,top=%d,right=%d,bottom=%d\n",bound.left,bound.top,bound.right,bound.bottom);
}

void CTestGBufferView::OnIdFsbmp() 
{
	CClientDC dc(this);
	m_pdrawer1->FSBitMap();
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
}

void CTestGBufferView::OnIdFsbmp2() 
{
	CClientDC dc(this);
	m_pdrawer1->FSBitMap2();
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
}

void CTestGBufferView::OnIdArea768() 
{
		delete m_pbuffer1;
		delete m_pDrawingArea1;
		delete m_pdrawer1;
		m_pbuffer1 = new GP_PixelBuffer(1024,768);
		m_pDrawingArea1 = new GP_DrawingArea;
		m_pDrawingArea1->GP_SetDrawingArea(0,0,1024,768);
		m_pdrawer1 = new GP_PixelDrawer(m_pbuffer1);
			

		CClientDC dc(this);
		CRect rect;
		GetClientRect(&rect);
		dc.FillRect(&rect,&CBrush(RGB(100,100,100)));
		m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
}

void CTestGBufferView::OnIdArea1200() 
{
		delete m_pbuffer1;
		delete m_pDrawingArea1;
		delete m_pdrawer1;
		m_pbuffer1 = new GP_PixelBuffer(1920,1200);
		m_pDrawingArea1 = new GP_DrawingArea;
		m_pDrawingArea1->GP_SetDrawingArea(0,0,1920,1200);
		m_pdrawer1 = new GP_PixelDrawer(m_pbuffer1);
			

		CClientDC dc(this);
		CRect rect;
		GetClientRect(&rect);
		dc.FillRect(&rect,&CBrush(RGB(100,100,100)));
		m_pdrawer1->Update(dc.GetSafeHdc(),10,10);
}

void CTestGBufferView::OnIdDrbmp() 
{
	const TCHAR filter[] = _T("BMP位图(*.*)");
	
	CFileDialog dlg(TRUE,_T("*.bmp"),NULL,OFN_FILEMUSTEXIST,filter,this);
	if (dlg.DoModal() != IDOK)
		return;
	
	CString str = dlg.GetPathName();
	GP_BitMap bmp;
	bmp.LoadFromFile(str);
	GP_HBITMAP  hbmp = m_pbuffer1->GetBitMapHandle();
	m_pdrawer1->StretchBitMap(0,0,hbmp.width,hbmp.hight,bmp,250);
	CClientDC dc(this);
	m_pdrawer1->Update(dc.GetSafeHdc(),10,10); 
}
