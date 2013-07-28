


#include "GP_DrawingArea.h"
#include <math.h>
#include <stdio.h>

#define VIEWMAXWIDTH 4000	//绘图最大宽度
#define VIEWMAXHIGHT 4000	//绘图最大高度

GP_DrawingArea::GP_DrawingArea(){
	//默认图形窗口大小为240*320
	m_DrawingAreaRect.left = 0;
	m_DrawingAreaRect.right = 240;
	m_DrawingAreaRect.top = 0;
	m_DrawingAreaRect.bottom = 320;

	m_iAngle = 0;
}

GP_DrawingArea::~GP_DrawingArea(){

}


GP_Status GP_DrawingArea::GP_SetDrawingArea(GP_RECT rectClient)
{
	return GP_SetDrawingArea(rectClient.left,rectClient.top,rectClient.right,rectClient.bottom);
}

GP_Status GP_DrawingArea::GP_SetDrawingArea(int left,int top,int right,int bottom){
	if ( left < 0 || top < 0 || right <= left || bottom <= top || abs(right - left) > VIEWMAXWIDTH || abs(bottom - top) > VIEWMAXHIGHT)
	{
		return Failed;
	}

	m_DrawingAreaRect.left = left;
	m_DrawingAreaRect.right = right;
	m_DrawingAreaRect.top = top;
	m_DrawingAreaRect.bottom = bottom;

	return Ok;
}


GP_Status GP_DrawingArea::GP_WindowToClient(int* ptIn,int iCount){
	if (iCount<=0 || ptIn == NULL)
	{
		return Failed;
	}
	for (int i = 0; i < iCount ; i++)
	{	
		W2V(ptIn[i*2],ptIn[i*2+1],ptIn[i*2],ptIn[i*2+1]);
	}
	return Ok;
}

void GP_DrawingArea::GP_GetDrawingArea(GP_RECT &rRect){
	rRect = m_DrawingAreaRect;
}


GP_Status GP_DrawingArea::GP_Rotate(int iAngle){
	m_iAngle += iAngle;
	while (m_iAngle<0) m_iAngle+=360;
	while (m_iAngle>359) m_iAngle-=360;
	return Ok;
}

GP_Status GP_DrawingArea::GP_RotateToAngle(int iAngle){
	m_iAngle = iAngle;
	while (m_iAngle<0) m_iAngle+=360;
	while (m_iAngle>359) m_iAngle-=360;
	return Ok;
}


GP_Status GP_DrawingArea::GP_SetWindowArea(GP_RECT rectWindow){
	return GP_SetWindowArea(rectWindow.left,rectWindow.top,rectWindow.right,rectWindow.bottom);
}

GP_Status GP_DrawingArea::GP_SetWindowArea(int left,int top,int right,int bottom){
	if ( right <= left || bottom <= top)
	{
		return Failed;
	}
	if(top<m_PhysicalBound.top || bottom>m_PhysicalBound.bottom)
	{
		printf("%d-%d-%d-%d\t",left,top,right,bottom);

		if((top<m_PhysicalBound.top && bottom>m_PhysicalBound.bottom) || (bottom-top)>m_PhysicalBound.Hight())
		{
			top = m_PhysicalBound.top;
			bottom = m_PhysicalBound.bottom;
			int iMidX = (left+right)/2;
			int w = (bottom-top)*(m_DrawingAreaRect.Width()/m_DrawingAreaRect.Hight());
			left = iMidX - w/2;
			right = iMidX + w/2;
		}
		else if(bottom>m_PhysicalBound.bottom)
		{
			//return Failed;
			top -= (bottom - m_PhysicalBound.bottom);
			bottom = m_PhysicalBound.bottom;
			int iMidX = (left+right)/2;
			int w = (bottom-top)*(m_DrawingAreaRect.Width()/m_DrawingAreaRect.Hight());
			printf("w:%d iMidX:%d\n",w,iMidX);
			left = iMidX - w/2;
			right = iMidX + w/2;
		}
		else if(top<m_PhysicalBound.top)
		{
			//return Failed;
			bottom += (m_PhysicalBound.top - top);
			top = m_PhysicalBound.top;
			int iMidX = (left+right)/2;
			int w = (bottom-top)*(m_DrawingAreaRect.Width()/m_DrawingAreaRect.Hight());
			printf("w:%d iMidX:%d\n",w,iMidX);
			left = iMidX - w/2;
			right = iMidX + w/2;
		}
		
		printf("%d-%d-%d-%d\n",left,top,right,bottom);
		//return Failed;
	}


	
	
	//设置的窗口范围比例和视图范围比例可能不一致，规范化（保证窗口范围在视图范围内都能显示）

	int iIMGW = abs(right - left);
	int iIMGH = abs(bottom - top);
	float IMG_W_H = (float)iIMGW/iIMGH;
	int iRTW = m_DrawingAreaRect.Width();
	int iRTH = m_DrawingAreaRect.Hight();
	float RT_W_H = (float)iRTW/iRTH;
	
	GP_RECT rt(left,top,right,bottom);
	GP_RECT rt2 = rt;

	bool firstinit = true;
	if (firstinit)
	{
		firstinit = false;
		if (IMG_W_H>RT_W_H)
		{
			int tempi = iIMGW/RT_W_H - iIMGH;
			tempi/=2;
			rt2.top-=tempi;
			rt2.bottom+=tempi;
		}
		else
		{
			int tempi = iIMGH*RT_W_H - iIMGW;
			tempi/=2;
			rt2.left-=tempi;
			rt2.right+=tempi;
		}
	}

	m_WindowAreaRect = rt2;

	//////////////////////////////////////////////////////////////////////////
	m_W2V_a = (float)m_DrawingAreaRect.Width()*10000 / m_WindowAreaRect.Width();
	m_W2V_b = m_DrawingAreaRect.left*10000 - m_W2V_a * m_WindowAreaRect.left;
	m_W2V_c = (float)m_DrawingAreaRect.Hight()*10000 / m_WindowAreaRect.Hight();
	m_W2V_d = m_DrawingAreaRect.bottom*10000 - m_W2V_c * m_WindowAreaRect.bottom;

	m_DisplayScale = m_W2V_a;
	
	return Ok;
}

GP_Status GP_DrawingArea::GP_MoveClient(int iOffsetX,int iOffsetY){
	GP_RECT rttemp = m_DrawingAreaRect;
	rttemp.left += iOffsetX;
	rttemp.right += iOffsetX;
	rttemp.top += iOffsetY;
	rttemp.bottom += iOffsetY;

	GP_RECT rtW;
	V2W(rttemp,rtW);

	GP_SetWindowArea(rtW);
	//m_WindowAreaRect = rtW;

	return Ok;
}

GP_Status GP_DrawingArea::GP_Zoom(float fScale){
	if (fScale<=0)
	{
		return Failed;
	}
	int iCenterX = m_WindowAreaRect.left + (m_WindowAreaRect.right-m_WindowAreaRect.left)/2;
	int iCenterY = m_WindowAreaRect.top + (m_WindowAreaRect.bottom-m_WindowAreaRect.top)/2;
	return GP_ZoomToWindow(iCenterX,iCenterY,fScale);
}

GP_Status GP_DrawingArea::GP_ZoomToClient(int iX,int iY,float fScale){
	if (fScale<=0)
	{
		return Failed;
	}
	int iwx,iwy;
	V2W(iX,iY,iwx,iwy);
	return GP_ZoomToWindow(iwx,iwy,fScale);
}

GP_Status GP_DrawingArea::GP_ZoomToWindow(int iX,int iY,float fScale){
	if (fScale<=0)
	{
		return Failed;
	}
	int w_width = m_WindowAreaRect.Width();
	int w_hight = m_WindowAreaRect.Hight();

	w_width /= fScale;
	w_hight /= fScale;
	
	w_width /= 2;
	w_hight /= 2;

	return GP_SetWindowArea(iX-w_width,iY-w_hight,iX+w_width,iY+w_hight);

}

void GP_DrawingArea::V2W(GP_RECT rv,GP_RECT &rw){
	rw.left = (rv.left*10000 - m_W2V_b) / m_W2V_a;
	rw.top = (float)(rv.top*10000 - m_W2V_d) / m_W2V_c;
	rw.right = (rv.right*10000 - m_W2V_b) / m_W2V_a;
	rw.bottom = (float)(rv.bottom*10000 - m_W2V_d) / m_W2V_c;
}

void GP_DrawingArea::V2W(int ivx,int ivy,int &iwx,int &iwy){
	iwx = (ivx*10000 - m_W2V_b) / m_W2V_a;
	iwy = (ivy*10000 - m_W2V_d) / m_W2V_c;
}

void GP_DrawingArea::W2V(GP_RECT rw,GP_RECT &rv){
	rv.left = (m_W2V_a/10000) * rw.left + m_W2V_b/10000;
	rv.top = (m_W2V_c/10000) * rw.top + m_W2V_d/10000;
	rv.right = (m_W2V_a/10000) * rw.right + m_W2V_b/10000;
	rv.bottom = (m_W2V_c/10000) * rw.bottom + m_W2V_d/10000;
}


// 窗口位置到试图位置坐标变换
// xv = a * xw + b
// yv = c * yw + d
// 其中：
// a = ( vxr - vxl ) / ( wxr - wxl )
// b = vxl - a * wxl
// c = ( vyt - vyb ) / ( wyt - wyb )
// d = vyb - c * wyb
// 
void GP_DrawingArea::W2V(int iwx,int iwy,int &ivx,int &ivy){
	ivx = (m_W2V_a/10000) * iwx + m_W2V_b/10000;
	ivy = (m_W2V_c/10000) * iwy + m_W2V_d/10000;
}