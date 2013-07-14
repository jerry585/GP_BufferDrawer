#ifndef _GP_DRAWINGAREA_H_
#define _GP_DRAWINGAREA_H_

#include "GP_CommonType.h"

class GP_DrawingArea{
public:
	GP_DrawingArea();
	~GP_DrawingArea();

	GP_Status GP_SetDrawingArea(GP_RECT rectClient);
	GP_Status GP_SetDrawingArea(int left,int top,int right,int bottom);

	void GP_GetDrawingArea(GP_RECT &rRect);

	GP_Status GP_SetWindowArea(GP_RECT rectWindow);
	GP_Status GP_SetWindowArea(int left,int top,int right,int bottom);

	GP_Status GP_WindowToClient(int* ptIn,int iCount);

	GP_Status GP_MoveClient(int iOffsetX,int iOffsetY);

	GP_Status GP_Zoom(float fScale);

	GP_Status GP_ZoomToClient(int iX,int iY,float fScale);

	GP_Status GP_ZoomToWindow(int iX,int iY,float fScale);

	GP_Status GP_Rotate(int iAngle);

	GP_Status GP_RotateToAngle(int iAngle);

private:
	//视图区域
	GP_RECT m_DrawingAreaRect;

	//当前视图区域对应用户区域大小（窗口区域范围）
	GP_RECT m_WindowAreaRect;


	int m_iAngle;

	// 
	// 
	// 窗口位置到试图位置坐标变换
	// xv = a * xw + b
	// yv = c * yw + d
	// 其中：
	// a = ( vxr - vxl ) / ( wxr - wxl )
	// b = vxl - a * wxl
	// c = ( vyt - vyb ) / ( wyt - wyb )
	// d = vyb - c * wyb
	// 

	void V2W(GP_RECT rv,GP_RECT &rw);

	void V2W(int ivx,int ivy,int &iwx,int &iwy);


	//////////////////////////////////////////////////////////////////////////
	//

	float m_W2V_a;
	float m_W2V_c;
	GP_INT64 m_W2V_b;
	GP_INT64 m_W2V_d;

	void W2V(GP_RECT rw,GP_RECT &rv);
	
	void W2V(int iwx,int iwy,int &ivx,int &ivy);

	//显示比例尺
	double m_DisplayScale;
};


#endif 