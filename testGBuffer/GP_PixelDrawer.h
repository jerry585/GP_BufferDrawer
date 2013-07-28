#ifndef _GP_PIXELDRAWER_H_
#define _GP_PIXELDRAWER_H_

#include "GP_PixelBuffer.h"
#include "GP_BitMap.h"
#include <windows.h>

#include "GP_CommonType.h"

class GP_PixelDrawer{
public:
	GP_PixelDrawer(GP_PixelBuffer* pbuffer);
	~GP_PixelDrawer();
public:
	GP_Status DrawLine(int x1,int y1,int x2,int y2,GP_Color& color,int width = 1);

	GP_Status DrawLineAnti(int x1,int y1,int x2,int y2,GP_Color& color,int width = 1);

	GP_Status DrawBezierLine2PT(int x1,int y1,int px1,int py1,int x2,int y2,int px2,int py2,GP_Color& color);

	GP_Status DrawBezierLinePTS(GP_Point* pPointList,int iCount,GP_Color& color);

	GP_Status DrawLine(GP_Point p1,GP_Point p2,GP_Color& color,int width = 1);

	GP_Status DrawPolyLine(GP_Point* pPointList,int iCount,GP_Color& color);

	GP_Status DrawPolygon(GP_Point* pPointList,int iCount,GP_Color& color);

	GP_Status FillPolygonScanLine(GP_Point* pPointList,int iCount,GP_Color& color);

	GP_Status DrawRect(int left,int top,int right,int bottom,GP_Color& color);

	GP_Status FillRect(int left,int top,int right,int bottom,RGB555& cl,BYTE alpha = 255);

	GP_Status FillRect(int left,int top,int right,int bottom,GP_Color& color);

	GP_Status DrawCircle(int x,int y,int r,GP_Color& color);

	GP_Status FillCircle(int x,int y,int r,GP_Color& color);

	GP_Status DrawEllispe(int x,int y,int a,int b,GP_Color& color);

	GP_Status DrawBitMap(int x,int y,GP_BitMap& bmp,BYTE alpha = 255);

	GP_Status DrawBitMap(int x,int y,int srcx,int srcy,int w,int h,GP_BitMap& bmp,BYTE alpha = 255);

	GP_Status Clear();

	void Release();

	GP_Status Update(HDC hdc,int x,int y);

	GP_Status DrawPixel(int x,int y,GP_Color& color);

	void DrawPixel(int x,int y,RGB555 cl,BYTE alpha = 255);

	GP_Status DrawPixelScanLine(int x1,int x2,int y,RGB555 cl);

	GP_Status DrawPixelAnti(int x,int y,RGB555 cl,int dst);

	GP_Status StretchBitMap(int x,int y,int w,int h, GP_BitMap& bmp,BYTE alpha = 255);

	GP_Status StretchBitMapRotate(int x,int y,int w,int h, GP_BitMap& bmp,int angle = 0,BYTE alpha = 255);

	GP_Status StretchBitMap(const GP_RECT& rt, GP_BitMap& bmp,BYTE alpha = 255);

	//////////////////////////////////////////////////////////////////////////
	//防射变换
	GP_Status FSBitMap();
	GP_Status FSBitMap2();
private:
	GP_PixelBuffer* m_buffer;
	int m_buffer_w;
	int m_buffer_h;
	unsigned char* m_buffer_pBits;

	//位图行位置表
	int* table;

	float* table_sinF;
	float* table_cosF;
	float* table_3_B_pow2;
	float* table_Bpow3;

	GP_Line* mp_linelist;

	//交换值
	void swap(int* m,int* n);

	//最大最小
	int minINT(int &a,int &b,int &c);
	int minINT(int &a,int &b);
	int maxINT(int &a,int &b,int &c);
	int maxINT(int &a,int &b);

	int minINT(int* piValue,int iCount);
	int maxINT(int* piValue,int iCount);

	GP_RECT GP_GetRangRect(GP_Point* pPointList,int iCount);

	void GP_GetPolygonScanLineCross(GP_Point* pPointList,int iCount,int y, GP_INTLIST** ymap);

	int GP_GetLineScanLineCross(int x1,int y1,int x2,int y2,int x3,int y3,int y, int& x);

	//Alpha混合
	void GP_AlphaBlend(RGB555& src,RGB555& dest,BYTE alpha);

	//矩形求交
	GP_RECT GP_RectCross(GP_RECT rect1,GP_RECT rect2);

	int GP_GetDistenceToLineX10(int x,int y,int x1,int y1,int x2,int y2);

	int GP_GetDistence2PV2(int x1,int y1,int x2,int y2);

	bool GP_PerptToLine(int x,int y,int x1,int y1,int x2,int y2, double *dx, double *dy);
	
	void GP_Draw_Circle_8(int xc,int yc,int x,int y,GP_Color& color);
};

#endif