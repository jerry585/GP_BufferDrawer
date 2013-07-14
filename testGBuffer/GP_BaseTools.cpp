#include <math.h>
#include "GP_CommonType.h"
#include "GP_BaseTools.h"

static GP_BaseTools* _TL_instance = 0;

GP_BaseTools* GP_GetBaseTools()
{
	return GP_BaseTools::Instance();
}

GP_BaseTools* GP_BaseTools::Instance()
{
	if (_TL_instance == 0)
	{
		_TL_instance = new GP_BaseTools();
	}
	return _TL_instance;
}


GP_BaseTools::GP_BaseTools()
{
	//////////////////////////////////////////////////////////////////////////
	//初始化SIN COS，加速计算 
	int i = 0;
	for (i = 0 ; i < 91 ; i ++)
	{
		double temp = sin(i*GP_PAI/180.0);
		m_iSIN[i] = temp*65536.0;
		m_dSIN[i] = temp;
	}
}

GP_BaseTools::~GP_BaseTools()
{
}

int GP_BaseTools::format_ang(int iAng)
{
	while (iAng<0) iAng+=360;
	while (iAng>359) iAng-=360;
	return iAng;
}

double GP_BaseTools::gp_sin(int iAng)
{
	iAng = format_ang(iAng);
	double rt_sin = 0.0;
	switch(iAng/90)
	{
	case 0:
		rt_sin = m_dSIN[ iAng ];
		break;
	case 1:
		rt_sin = m_dSIN[ 180 - iAng ];
		break;
	case 2:
		rt_sin = -m_dSIN[ iAng - 180 ];
		break;
	case 3:
		rt_sin = -m_dSIN[ 360 - iAng ];
		break;
	}
	return rt_sin;
}

double GP_BaseTools::gp_cos(int iAng)
{
	iAng = format_ang(iAng);
	double rt_cos = 0.0;
	switch(iAng/90)
	{
	case 0:
		rt_cos = m_dSIN[ 90 - iAng ];
		break;
	case 1:
		rt_cos = -m_dSIN[ iAng - 90 ];
		break;
	case 2:
		rt_cos = -m_dSIN[ 270 - iAng ];
		break;
	case 3:
		rt_cos = m_dSIN[ iAng - 270 ];
		break;
	}
	return rt_cos;
}

int GP_BaseTools::gp_rotate_one_point(int iCenterX,int iCenterY,int x1,int y1,int &x2,int &y2,int iAng)
{
	iAng = format_ang(iAng);

	int rt_cos = 0;
	int rt_sin = 0;
	switch(iAng/90)
	{
	case 0:
		rt_sin = m_iSIN[ iAng ];
		rt_cos = m_iSIN[ 90 - iAng ];
		break;
	case 1:
		rt_sin = m_iSIN[ 180 - iAng ];
		rt_cos = -m_iSIN[ iAng - 90 ];
		break;
	case 2:
		rt_sin = -m_iSIN[ iAng - 180 ];
		rt_cos = -m_iSIN[ 270 - iAng ];
		break;
	case 3:
		rt_sin = -m_iSIN[ 360 - iAng ];
		rt_cos = m_iSIN[ iAng - 270 ];
		break; 
	}

	const int iOverFllowLimit = (1<<14);

	if (x1 < -iOverFllowLimit || x1 > iOverFllowLimit || y1 < -iOverFllowLimit || y1 > iOverFllowLimit)
	{
		GP_INT64 X1 = x1 - iCenterX;
		GP_INT64 Y1 = iCenterY - y1;

		GP_INT64 X2 = X1 * rt_cos - Y1 * rt_sin;
		GP_INT64 Y2 = Y1 * rt_cos + X1 * rt_sin;

		x2 = iCenterX + ((X2+32767)>>16);
		y2 = iCenterY - ((Y2+32767)>>16);
	}
	else
	{
		int X1 = x1 - iCenterX;
		int Y1 = iCenterY - y1;

		int X2 = X1 * rt_cos - Y1 * rt_sin;
		int Y2 = Y1 * rt_cos + X1 * rt_sin;

		x2 = iCenterX + ((X2+32767)>>16);
		y2 = iCenterY - ((Y2+32767)>>16);
	}

	return 1;
}

int GP_BaseTools::gp_rotate_points(int iCenterX,int iCenterY,int* ptIn,int iCount,int iAng){
	iAng = format_ang(iAng);
	
	int rt_cos = 0;
	int rt_sin = 0;
	switch(iAng/90)
	{
	case 0:
		rt_sin = m_iSIN[ iAng ];
		rt_cos = m_iSIN[ 90 - iAng ];
		break;
	case 1:
		rt_sin = m_iSIN[ 180 - iAng ];
		rt_cos = -m_iSIN[ iAng - 90 ];
		break;
	case 2:
		rt_sin = -m_iSIN[ iAng - 180 ];
		rt_cos = -m_iSIN[ 270 - iAng ];
		break;
	case 3:
		rt_sin = -m_iSIN[ 360 - iAng ];
		rt_cos = m_iSIN[ iAng - 270 ];
		break; 
	}
	
	const int iOverFllowLimit = (1<<14);

	int i = 0;
	for (i = 0;i<iCount;i++)
	{
		int x1 = ptIn[2*i];
		int y1 = ptIn[2*i+1];

		if (x1 < -iOverFllowLimit || x1 > iOverFllowLimit || y1 < -iOverFllowLimit || y1 > iOverFllowLimit)
		{
			GP_INT64 X1 = x1 - iCenterX;
			GP_INT64 Y1 = iCenterY - y1;
			
			GP_INT64 X2 = X1 * rt_cos - Y1 * rt_sin;
			GP_INT64 Y2 = Y1 * rt_cos + X1 * rt_sin;
			
			ptIn[2*i] = iCenterX + ((X2+32767)>>16);
			ptIn[2*i+1] = iCenterY - ((Y2+32767)>>16);
		}
		else
		{
			int X1 = x1 - iCenterX;
			int Y1 = iCenterY - y1;
			
			int X2 = X1 * rt_cos - Y1 * rt_sin;
			int Y2 = Y1 * rt_cos + X1 * rt_sin;
			
			ptIn[2*i] = iCenterX + ((X2+32767)>>16);
			ptIn[2*i+1] = iCenterY - ((Y2+32767)>>16);
		}
	}

	return 1;
}