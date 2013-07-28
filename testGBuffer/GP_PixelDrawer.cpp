#include "GP_PixelDrawer.h"
#include "math.h"

#include "GP_Performance.h"

GP_PixelDrawer::GP_PixelDrawer(GP_PixelBuffer* pbuffer){
	m_buffer = pbuffer;
	m_buffer_h = m_buffer->GetBitMapHandle().hight;
	m_buffer_w = m_buffer->GetBitMapHandle().width;
	m_buffer_pBits = m_buffer->GetBitMapHandle().pBits;
	if (m_buffer_pBits == NULL)
	{
		return;
	}

	table = new int[m_buffer_h];
	memset(table,0,sizeof(int)*m_buffer_h);
	int i = 0;
	for (i = 0; i < m_buffer_h ; i++)
	{
		table[i] = ((i*m_buffer_w)<<1);
	}

	table_sinF = new float[360];
	table_cosF = new float[360];
	for (i = 0; i < 360 ; i++)
	{
		table_sinF[i] = sin(i*0.017459);
		table_cosF[i] = cos(i*0.017459);
	}

	table_3_B_pow2 = new float[101];
	table_Bpow3 = new float[101];
	for (i = 0; i <= 100 ; i++)
	{
		table_3_B_pow2[i] = powf((float)i/100,2)*3*((float)(100-i)/100);
		table_Bpow3[i] = powf((float)i/100,3);
	}
	mp_linelist = new GP_Line[1024];
}

GP_PixelDrawer::~GP_PixelDrawer(){
	delete[] table; 
	delete[] table_sinF;
	delete[] table_cosF;
	delete[] table_3_B_pow2;
	delete[] table_Bpow3;
	delete[] mp_linelist;
}

void GP_PixelDrawer::Release(){
	delete this;
}

void GP_PixelDrawer::swap(int* m,int* n)
{
	int t;
	t=*m;
	*m=*n;
	*n=t;
}

GP_Status GP_PixelDrawer::DrawBezierLinePTS(GP_Point* pPointList,int iCount,GP_Color& color){
	gpPerformanceLog("GP_PixelDrawer::DrawBezierLinePTS");
	if (m_buffer_pBits == NULL)
	{
		return Failed;
	}

	if ((iCount&0x01) != 0)
	{
		return Failed;
	}
	else
	{
		for (int i = 0;i<iCount/2 -1;i++) 
		{
			DrawBezierLine2PT(pPointList[i*2].x,pPointList[i*2].y,pPointList[i*2+1].x,pPointList[i*2+1].y
				,pPointList[i*2+2].x,pPointList[i*2+2].y,2*pPointList[i*2+2].x-pPointList[i*2+3].x,2*pPointList[i*2+2].y-pPointList[i*2+3].y,color);
		}
	}
	return Ok;
}

GP_Status GP_PixelDrawer::DrawBezierLine2PT(int x1,int y1,int px1,int py1,int x2,int y2,int px2,int py2,GP_Color& color){

	if (m_buffer_pBits == NULL)
	{
		return Failed;
	}

	GP_Point pts[101];
	for(int t = 0; t <=100 ; t++)
	{
// 		float b = (float)(100-t)/100;
// 		float a = (float)t/100;
// 		float B3 = powf(b,3);
// 		float B2 = powf(b,2);
// 		float A2 = powf(a,2);
// 		float A3 = powf(a,3);
// 		int pointx = x1*B3 + 3*px1*B2*a + 3*px2*b*A2 + x2*A3 ;
// 		int pointy = y1*B3 + 3*py1*B2*a + 3*py2*b*A2 + y2*A3 ; 

		//查表法简化
		int pointx = x1*table_Bpow3[100-t] + px1*table_3_B_pow2[100-t] + px2*table_3_B_pow2[t] + x2*table_Bpow3[t] ;
		int pointy = y1*table_Bpow3[100-t] + py1*table_3_B_pow2[100-t] + py2*table_3_B_pow2[t] + y2*table_Bpow3[t] ;

		pts[t] = GP_Point(pointx,pointy);
	}
	DrawPolyLine(pts,101,color);
	return Ok;
}

int GP_PixelDrawer::GP_GetDistenceToLineX10(int x,int y,int x1,int y1,int x2,int y2){
	double dx,dy;
	if (GP_PerptToLine(x,y,x1,y1,x2,y2,&dx,&dy))
	{
		return ((dx-x)*(dx-x)+(dy-y)*(dy-y))*10;
	}
	else
	{
		return -1;
	}
}

bool GP_PixelDrawer::GP_PerptToLine(int x,int y,int x1,int y1,int x2,int y2, double *dx, double *dy)
{
	double  mx, my;
	double mx1,my1, mx2, my2;
	
	double minLineX, maxLineX, minLineY, maxLineY;
	//产生线的最小矩形坐标
	minLineX = x1 - x2 < 0 ? x1 : x2;
	minLineY = y1 - y2 < 0 ? y1 : y2;
	maxLineX = x1 - x2 > 0 ? x1 : x2;
	maxLineY = y1 - y2 > 0 ? y1 : y2;
	
	mx = (double)x;
	my = (double)y;
	
	mx1 = (double)x1; my1 = (double)y1;
	mx2 = (double)x2; my2 = (double)y2;
	if (mx1 == mx2)	//铅垂线
	{
		*dy = y ;
		*dx = x1 ;		
		
		if( my < minLineY || my > maxLineY)	//投影点不在线段上
			return false;
	}
	else 
	{
		double m = (my2 - my1) / (mx2 - mx1);
		
		double tempx = (m * m * mx1 + mx + m * (my - my1)) / (m * m + 1 );
		double tempy = (m * m* my + my1 + m * (mx - mx1)) / (m * m + 1);
		
		*dx = tempx;
		*dy = tempy;
		
		if ( (minLineX  > tempx) ||  (maxLineX <  tempx) ||
			(minLineY >  tempy) ||  (maxLineY  <  tempy) )
			return false;		
	}
	return true;
}

GP_Status GP_PixelDrawer::DrawLineAnti(int x1,int y1,int x2,int y2,GP_Color& color,int width){
	gpPerformanceLog("GP_PixelDrawer::DrawLineAnti");
	if (m_buffer_pBits == NULL)
	{
		return Failed;
	}

	if (minINT(x1,x2) > m_buffer_w || minINT(y1,y2) > m_buffer_h || maxINT(x1,x2) < 0 || maxINT(y1,y2) < 0)
	{
		return Failed;
	}

	int x,y,dx,dy;
	int p,same,change;
	float k,e;
	dx = x2-x1;
	dy = y2-y1;

	RGB555 cl = color.Get555Value();
	if (dx == 0) 
	{
		if (dy<0)
		{
			swap(&y1,&y2);
		}
		x = x1;
		y = y1;
		for (;y<=y2;y++)
		{
			DrawPixelAnti(x,y,cl,GP_GetDistenceToLineX10(x,y,x1,y1,x2,y2)*2/width);
			DrawPixelAnti(x,y+1,cl,GP_GetDistenceToLineX10(x,y+1,x1,y1,x2,y2)*2/width);
			DrawPixelAnti(x,y-1,cl,GP_GetDistenceToLineX10(x,y-1,x1,y1,x2,y2)*2/width);
		}
		return Ok;
	}

	if(x1>x2)
	{
		swap(&x1,&x2);
		swap(&y1,&y2);
	}

	dx=x2-x1;
	dy=y2-y1;
	
	if(0<=dy&&dy<=dx)
	{
		p=(dy<<1)-dx;same=dy<<1;change=(dy-dx)<<1;x=x1;y=y1;
		for(;x<=x2;x++)
		{
			DrawPixelAnti(x,y,cl,GP_GetDistenceToLineX10(x,y,x1,y1,x2,y2));
			DrawPixelAnti(x,y+1,cl,GP_GetDistenceToLineX10(x,y+1,x1,y1,x2,y2));
			DrawPixelAnti(x,y-1,cl,GP_GetDistenceToLineX10(x,y-1,x1,y1,x2,y2));
			if(p<0)
			{
				p+=same;
			}
			else
			{
				p+=change;
				y++;
			}
		}
		return Ok;
	}
	else if(-dx<=dy&&dy<0)
	{
		p=-dx-dy<<1;same=-dy<<1;change=(-dy-dx)<<1;x=x1;y=y1;
		for(;x<=x2;x++)
		{
			DrawPixelAnti(x,y,cl,GP_GetDistenceToLineX10(x,y,x1,y1,x2,y2));
			DrawPixelAnti(x,y+1,cl,GP_GetDistenceToLineX10(x,y+1,x1,y1,x2,y2));
			DrawPixelAnti(x,y-1,cl,GP_GetDistenceToLineX10(x,y-1,x1,y1,x2,y2));
			if(p<0)
				p+=same;
			else
			{
				p+=change;
				y--;
			}
		}
		return Ok;
	}
	else if(dx<dy)
	{
		if(y1>y2)
		{
			swap(&x1,&x2);
			swap(&y1,&y2);
		}
		dx=x2-x1;
		dy=y2-y1;
		p=(dx<<1)-dy;same=dx<<1;change=(dx-dy)<<1;x=x1;y=y1;
		for(;y<=y2;y++)
		{
			DrawPixelAnti(x,y,cl,GP_GetDistenceToLineX10(x,y,x1,y1,x2,y2));
			DrawPixelAnti(x,y+1,cl,GP_GetDistenceToLineX10(x,y+1,x1,y1,x2,y2));
			DrawPixelAnti(x,y-1,cl,GP_GetDistenceToLineX10(x,y-1,x1,y1,x2,y2));
			if(p<0)
				p+=same;
			else
			{
				p+=change;
				x++;
			}
		}
		return Ok;
	}
	else
	{
		if(y1>y2)
		{
			swap(&x1,&x2);
			swap(&y1,&y2);
		}
		dx=x2-x1;
		dy=y2-y1;
		p=-(dx<<1)-dy,same=-(dx<<1),change=-((dx+dy)<<1),x=x1,y=y1;
		for(;y<=y2;y++)
		{
			DrawPixelAnti(x,y,cl,GP_GetDistenceToLineX10(x,y,x1,y1,x2,y2));
			DrawPixelAnti(x,y+1,cl,GP_GetDistenceToLineX10(x,y+1,x1,y1,x2,y2));
			DrawPixelAnti(x,y-1,cl,GP_GetDistenceToLineX10(x,y-1,x1,y1,x2,y2));
			if(p<0)
				p+=same;
			else
			{
				p+=change;
				x--;
			}
		}
		return Ok;
	}

	return Ok; 

}

GP_Status GP_PixelDrawer::DrawLine(int x1,int y1,int x2,int y2,GP_Color& color,int width){
	if (m_buffer_pBits == NULL)
	{
		return Failed;
	}

	if (minINT(x1,x2) > m_buffer_w || minINT(y1,y2) > m_buffer_h || maxINT(x1,x2) < 0 || maxINT(y1,y2) < 0)
	{
		return Failed;
	}
	RGB555 cl = color.Get555Value();
	BYTE alpha = color.GetAlpha();
	if(width==1)
	{
		int x,y,dx,dy;
		int p,same,change;
		float k,e;
		dx = x2-x1;
		dy = y2-y1;

		//RGB555 cl = color.Get555Value();
		if (dx == 0) 
		{
			if (dy<0)
			{
				swap(&y1,&y2);
			}
			x = x1;
			y = y1;
			for (;y<=y2;y++)
			{
				DrawPixel(x,y,cl,alpha);
			}
			return Ok;
		}

		if(x1>x2)
		{
			swap(&x1,&x2);
			swap(&y1,&y2);
		}

		dx=x2-x1;
		dy=y2-y1;
		
		if(0<=dy&&dy<=dx)
		{
			p=(dy<<1)-dx;same=dy<<1;change=(dy-dx)<<1;x=x1;y=y1;
			for(;x<=x2;x++)
			{
				DrawPixel(x,y,cl,alpha);
				if(p<0)
				{
					p+=same;
				}
				else
				{
					p+=change;
					y++;
				}
			}
			return Ok;
		}
		else if(-dx<=dy&&dy<0)
		{
			p=-dx-dy<<1;same=-dy<<1;change=(-dy-dx)<<1;x=x1;y=y1;
			for(;x<=x2;x++)
			{
				DrawPixel(x,y,cl,alpha);
				if(p<0)
					p+=same;
				else
				{
					p+=change;
					y--;
				}
			}
			return Ok;
		}
		else if(dx<dy)
		{
			if(y1>y2)
			{
				swap(&x1,&x2);
				swap(&y1,&y2);
			}
			dx=x2-x1;
			dy=y2-y1;
			p=(dx<<1)-dy;same=dx<<1;change=(dx-dy)<<1;x=x1;y=y1;
			for(;y<=y2;y++)
			{
				DrawPixel(x,y,cl,alpha);
				if(p<0)
					p+=same;
				else
				{
					p+=change;
					x++;
				}
			}
			return Ok;
		}
		else
		{
			if(y1>y2)
			{
				swap(&x1,&x2);
				swap(&y1,&y2);
			}
			dx=x2-x1;
			dy=y2-y1;
			p=-(dx<<1)-dy,same=-(dx<<1),change=-((dx+dy)<<1),x=x1,y=y1;
			for(;y<=y2;y++)
			{
				DrawPixel(x,y,cl,alpha);
				if(p<0)
					p+=same;
				else
				{
					p+=change;
					x--;
				}
			}
			return Ok;
		}
		return Ok; 
	}
	else if(width>1&&width%2==0)
	{
		if(y1 == y2)
		{
			int y11=y1,y12=y1;
			int iAdd1 = (width-1)/2;
			y11-=iAdd1;y12+=iAdd1;
			for(int k = y11;k<=y12;k++)
				DrawLine(x1,k,x2,k,color);   
//			DrawLine(x1,k,x2,k,color); 
			GP_Color color2(100,color.GetRed(),color.GetGreen(),color.GetBlue());
			DrawLine(x1,y11-1,x2,y11-1,color2);
			DrawLine(x1,y12+1,x2,y12+1,color2);  
		}
		else if(x1 == x2)
		{
			int x11=x1,x12=x1;
			int iAdd1 = (width-1)/2;
			x11-=iAdd1;x12+=iAdd1;
			for(int k = x11;k<=x12;k++)
				DrawLine(k,y1,k,y2,color);
// 			DrawLine(k,y1,k,y2,color);
			GP_Color color2(100,color.GetRed(),color.GetGreen(),color.GetBlue());
			DrawLine(x11-1,y1,x11-1,y2,color2);
			DrawLine(x12+1,y1,x12+1,y2,color2);
		}
		else
		{
			double ddy = (double)width/(2.0*sqrt((double)(1+((y2-y1)*(y2-y1))/((x2-x1)*(x2-x1)))));
			double ddx = abs(ddy*(y2-y1)/(x2-x1));
			//ddy = (int)ddy; ddx = (int)ddx;
// 			printf("\nOOO***************\n%f-%f\n",ddx,ddy);
			GP_Point ptlst[4] = {GP_Point((x1-ddx),(y1+ddy)),GP_Point((x1+ddx),(y1-ddy))
				,GP_Point((x2+ddx),(y2-ddy)),GP_Point((x2-ddx),(y2+ddy))}; 
			ddy+=0.5;ddx+=0.5;
			GP_Point ptlst2[4] = {GP_Point((x1-ddx),(y1+ddy)),GP_Point((x1+ddx),(y1-ddy))
				,GP_Point((x2+ddx),(y2-ddy)),GP_Point((x2-ddx),(y2+ddy))}; 
// 			for(int i = 0 ; i<4;i++)
// 			{
// 				printf("%d-%d\t",ptlst[i].x,ptlst[i].y);
// 			}
			GP_Color color2(128,color.GetRed(),color.GetGreen(),color.GetBlue());
			FillPolygonScanLine(ptlst,4,color);
			FillPolygonScanLine(ptlst2,4,color2);
// 			ddx = GP_ROUND(ddx+0.5); ddy = GP_ROUND(ddy+0.5); 
// 			GP_Point ptlst2[4] = {GP_Point(GP_ROUND(x1-ddx),GP_ROUND(y1+ddy)),GP_Point(GP_ROUND(x1+ddx),GP_ROUND(y1-ddy))
// 				,GP_Point(GP_ROUND(x2+ddx),GP_ROUND(y2-ddy)),GP_Point(GP_ROUND(x2-ddx),GP_ROUND(y2+ddy))}; 
// 			GP_Color color2(255,color.GetRed(),color.GetGreen(),color.GetBlue());
// 			FillPolygonScanLine(ptlst2,4,color2); 
		}
	}
	else
	{
		if(y1 == y2)
		{
			int y11=y1,y12=y1;
			int iAdd1 = (width-1)/2;
			y11-=iAdd1;y12+=iAdd1;
			for(int k = y11;k<=y12;k++)
				DrawLine(x1,k,x2,k,color);
		}
		else if(x1 == x2)
		{
			int x11=x1,x12=x1;
			int iAdd1 = (width-1)/2;
			x11-=iAdd1;x12+=iAdd1;
			for(int k = x11;k<=x12;k++)
				DrawLine(k,y1,k,y2,color);
		}
		else
		{
			double ddy = (double)width/(2.0*sqrt(double(1+((y2-y1)*(y2-y1))/((x2-x1)*(x2-x1)))));
			double ddx = ddy*(y2-y1)/(x2-x1);
			GP_Point ptlst[4] = {GP_Point(GP_ROUND(x1-ddx),GP_ROUND(y1+ddy)),GP_Point(GP_ROUND(x1+ddx),GP_ROUND(y1-ddy))
				,GP_Point(GP_ROUND(x2+ddx),GP_ROUND(y2-ddy)),GP_Point(GP_ROUND(x2-ddx),GP_ROUND(y2+ddy))}; 
			FillPolygonScanLine(ptlst,4,color);
		}
	}
	return Ok; 
}

GP_Status GP_PixelDrawer::DrawLine(GP_Point p1,GP_Point p2,GP_Color& color,int width){

	if (m_buffer_pBits == NULL)
	{
		return Failed;
	}

	if (minINT(p1.x,p2.x) > m_buffer_w || minINT(p1.y,p2.y) > m_buffer_w || maxINT(p1.x,p2.x) < 0 || maxINT(p1.y,p2.y) < 0)
	{
		return Failed;
	}

//	return DrawLineAnti(p1.x,p1.y,p2.x,p2.y,color,width);

	return DrawLine(p1.x,p1.y,p2.x,p2.y,color,width);
}


GP_Status GP_PixelDrawer::DrawPolygon(GP_Point* pPointList,int iCount,GP_Color& color){
	gpPerformanceLog("GP_PixelDrawer::DrawPolygon");
	if (m_buffer_pBits == NULL)
	{
		return Failed;
	}
	
	if (iCount<2)
	{
		return Failed;
	}
	int i = 0;
	for (i = 0;i<iCount-1;i++)
	{
		DrawLine(pPointList[i],pPointList[i+1],color,3);
	}

	DrawLine(pPointList[0],pPointList[i],color,3); 

	return Ok;
}

GP_Status GP_PixelDrawer::FillPolygonScanLine(GP_Point* pPointList,int iCount,GP_Color& color){
	//return Failed;
	gpPerformanceLog("GP_PixelDrawer::FillPolygonScanLine");
	if (iCount<3)
	{
		return Failed;
	}
	GP_RECT rangrect = GP_GetRangRect(pPointList,iCount);
	if (rangrect.left>m_buffer_w || rangrect.right<0 || rangrect.top >m_buffer_h || rangrect.bottom<0)
	{
		return Failed;
	}
	
	int top = rangrect.top >=0 ? rangrect.top : 0;
	int bottom = rangrect.bottom >m_buffer_h ? m_buffer_h : rangrect.bottom;
	GP_INTLIST** ymap;
	ymap = new GP_INTLIST* [m_buffer_h];
	int i = 0;
	for (i = 0 ; i < m_buffer_h ; i ++)
	{
		ymap[i] = NULL;
	}

	int iCountNew = 0;
	for(i = 0 ; i < iCount ; i ++)
	{
		pPointList[iCountNew].x = pPointList[i].x;
		pPointList[iCountNew].y = pPointList[i].y;
		if (iCountNew>1 && pPointList[iCountNew].y == pPointList[iCountNew-1].y && pPointList[iCountNew-2].y == pPointList[iCountNew-1].y )
		{
			pPointList[iCountNew-1].x = pPointList[iCountNew].x; 
		}
		else if (iCountNew>0 && pPointList[iCountNew].y == pPointList[iCountNew-1].y && pPointList[iCountNew].x == pPointList[iCountNew-1].x)
		{
			continue;
		}
		else
		{
			iCountNew++;
		}
	}
	int y = 0;
	for (y = top+1 ; y < bottom ; y ++)
	{
		GP_GetPolygonScanLineCross(pPointList,iCountNew/*iCount*/,y,ymap);
	}
	
	BYTE alpha = color.GetAlpha();
	RGB555 cl = color.Get555Value();
	
	for (y = top+1 ; y < bottom ; y ++)
	{
		GP_INTLIST* p = ymap[y];
		while(p != NULL && p->next != NULL){ 
			int start = p->x;
			int end = p->next->x;
			if (start>end || end < 0 || start > m_buffer_w || y <0 || y > m_buffer_h)
			{
				p = p->next->next;
				continue;
			}
			if (start < 0) 
			{
				start = 0;
			}
			if (end >= m_buffer_w)
			{
				end = m_buffer_w-1;
			}
			if (alpha == 255)
			{
				DrawPixelScanLine(start,end,y,cl);
// 				for (int k = start ; k < end ; k ++)
// 				{
// 					DrawPixel(k,y,cl);
// 				}
			}
			else
			{
				for (int k = start ; k < end ; k ++)
				{
					DrawPixel(k,y,color);
				}
			}
// 			DrawLine(p->x+1,y,p->next->x-1,y,color);
			p = p->next->next;
		}
	} 
	 
	for (y = top+1 ; y < bottom ; y ++)
	{
		GP_INTLIST* p = ymap[y];
		while(p != NULL){
			GP_INTLIST* r = p->next;
			delete p;
			p = r;
		}
		ymap[y] = NULL;
	}
	
	delete[] ymap;
	
	return Ok;
}

void GP_PixelDrawer::GP_GetPolygonScanLineCross(GP_Point* pPointList,int iCount,int y, GP_INTLIST** ymap){
	//建立边表
	GP_Line* pLineList = NULL;
	if (iCount<=1024)
	{
		pLineList = mp_linelist;
	}
	else
	{
		pLineList = new GP_Line[iCount];
	}
	int iLineCount = 0;
	for (int k = 0 ; k < iCount ;k++)
	{
		if (pPointList[k].y != pPointList[(k+1)%iCount].y)
		{
			pLineList[iLineCount].x1 = pPointList[k].x;
			pLineList[iLineCount].y1 = pPointList[k].y;
			pLineList[iLineCount].x2 = pPointList[(k+1)%iCount].x;
			pLineList[iLineCount].y2 = pPointList[(k+1)%iCount].y;
			iLineCount++;
		}
	}

	for (int i = 0 ; i < iLineCount ; i ++)
	{
		int x = -1;
		int ret = GP_GetLineScanLineCross(pLineList[i].x1,pLineList[i].y1,pLineList[i].x2,pLineList[i].y2,pLineList[(i+1)%iLineCount].x2,pLineList[(i+1)%iLineCount].y2,y,x);
		if (ret == 1/* || ret == 0*/)
		{
			GP_INTLIST* m = new GP_INTLIST;
			m->x = x;
			m->px = ret == 1 ? false:true;
			m->next = NULL;

			if (ymap[y] == NULL)
			{
				ymap[y] = m;
			}
			else
			{
				GP_INTLIST *pre,*next;
				pre = NULL;
				next = ymap[y];
				while (next != NULL)
				{
					if (x < next->x)
					{
						break;
					}
					else
					{
						pre = next;
						next = next->next;
					}
				}
				if (pre == NULL)
				{
					ymap[y] = m;
					m->next = next;
				}
				else
				{
					pre->next = m;
					m->next = next;
				}
			}
		}
	}
	if (iCount>1024)
	{
		delete[] pLineList; 
	}
// 	return;
// 	GP_INTLIST* p = ymap[y];
// 	GP_INTLIST* tempre = p;
// 	while(p != NULL){
// 		bool flag = false;
// 		GP_INTLIST* temp;
// 		
// 		while (p != NULL && p->px)
// 		{
// 			if (p == ymap[y])
// 			{
// 				temp = p->next;
// 				delete p;
// 				p = temp;
// 				ymap[y] = p;
// 				tempre = p;
// 			}
// 			else
// 			{
// 				temp = p->next;
// 				delete p;
// 				p = temp;
// 				tempre->next = p;
// 			}
// 
// 			flag = true;
// 		}
// 		if (flag && p != NULL)
// 		{
// 			if (p == ymap[y])
// 			{
// 				temp = p->next;
// 				delete p;
// 				p = temp;
// 				ymap[y] = p;
// 				tempre = p;
// 			}
// 			else
// 			{
// 				temp = p->next;
// 				delete p;
// 				p = temp;
// 				tempre->next = p;
// 			}
// 		}
// 		if (p != NULL)
// 		{
// 			tempre = p;
// 			p = p->next;
// 		}
// 	}
}

int GP_PixelDrawer::GP_GetLineScanLineCross(int x1,int y1,int x2,int y2,int x3,int y3,int y, int& x){
	if (y < minINT(y1,y2) || y > maxINT(y1,y2)/* || y == y2*/)
	{
		return -1;
	}
	else if (y1 == y2)
	{
		x = (x1+x2)/2;
		return -1;
	}
	else if (y1 == y /*&& y1<y2 && y2<y3*/) 
	{
		return -1;
	}
	else if (y2 == y && y1>y2 && y2>y3) 
	{
		x = x2;
		return 1;
	}
	else if (y2 == y && y1<y2 && y2<y3) 
	{
		x = x2;
		return 1;
	}
	else if (y2 == y) 
	{
		return -1;
	}
	else
	{
		x = (y-y1)*(x2-x1)/(y2-y1) + x1;
		return 1;
	}
}

GP_Status GP_PixelDrawer::DrawPolyLine(GP_Point* pPointList,int iCount,GP_Color& color){

	if (m_buffer_pBits == NULL)
	{
		return Failed;
	}

	if (iCount<2)
	{
		return Failed;
	}

	for (int i = 0;i<iCount-1;i++)
	{
		DrawLine(pPointList[i],pPointList[i+1],color);
	}

	return Ok;
}

GP_Status GP_PixelDrawer::DrawRect(int left,int top,int right,int bottom,GP_Color& color){
	if (m_buffer_pBits == NULL)
	{
		return Failed;
	}

	DrawLine(left,top,right,top,color);
	DrawLine(right,top,right,bottom,color);
	DrawLine(right,bottom,left,bottom,color);
	DrawLine(left,bottom,left,top,color);

	return Ok;
}

GP_Status GP_PixelDrawer::FillRect(int left,int top,int right,int bottom,RGB555& cl,BYTE alpha)
{
		if (m_buffer_pBits == NULL)
	{
		return Failed;
	}

	if (left>right || top>bottom || left>m_buffer_w || top>m_buffer_h || right<0 || bottom<0)
	{
		return Failed;
	}

	RGB555 src,dest;
	dest = cl;

	//消除超越目标缓冲区范围
	left = left<0 ? 0:left;
	top = top<0 ? 0:top;
	right = right>m_buffer_w ? m_buffer_w : right;
	bottom = bottom>m_buffer_h ? m_buffer_h : bottom;

	if (alpha != 255)
	{
		for (int y = top ; y<bottom ; y++)
		{
			for (int x = left ; x<right ; x++)
			{
				//像素(x,y)在目标缓冲区内存位置
				int index = table[y] + (x<<1);
				//如果有透明度，则计算透明后的颜色
				memcpy(&src,&m_buffer_pBits[index],2);
				GP_AlphaBlend(src,dest,alpha);
				memcpy(&m_buffer_pBits[index],&src,2);
			}
		}
	}
	else
	{
		//可填充矩形字节宽
		int rect_Width = right - left;
		int rect_Width_b = rect_Width<<1;
		//准备一行像素
		unsigned char* destbits = new unsigned char[rect_Width_b];
		for (int x = 0 ; x<rect_Width ; x++)
		{
			memcpy(&destbits[x<<1],&dest,2);
		}
		//直接按行填充颜色
		for (int y = top ; y<bottom ; y++)
		{
			memcpy(&m_buffer_pBits[table[y]+(left<<1)],destbits,rect_Width_b);
		}
		delete[] destbits;
	}
	return Ok;
}

GP_Status GP_PixelDrawer::FillRect(int left,int top,int right,int bottom,GP_Color& color){
	if (m_buffer_pBits == NULL)
	{
		return Failed;
	}

	if (left>right || top>bottom || left>m_buffer_w || top>m_buffer_h || right<0 || bottom<0)
	{
		return Failed;
	}

	RGB555 src,dest;
	BYTE alpha = color.GetAlpha();
	dest = color.Get555Value();

	//消除超越目标缓冲区范围
	left = left<0 ? 0:left;
	top = top<0 ? 0:top;
	right = right>m_buffer_w ? m_buffer_w : right;
	bottom = bottom>m_buffer_h ? m_buffer_h : bottom;

	if (alpha != 255)
	{
		for (int y = top ; y<bottom ; y++)
		{
			for (int x = left ; x<right ; x++)
			{
				//像素(x,y)在目标缓冲区内存位置
				int index = table[y] + (x<<1);
				//如果有透明度，则计算透明后的颜色
				memcpy(&src,&m_buffer_pBits[index],2);
				GP_AlphaBlend(src,dest,alpha);
				memcpy(&m_buffer_pBits[index],&src,2);
			}
		}
	}
	else
	{
		//可填充矩形字节宽
		int rect_Width = right - left;
		int rect_Width_b = rect_Width<<1;
		//准备一行像素
		unsigned char* destbits = new unsigned char[rect_Width_b];
		for (int x = 0 ; x<rect_Width ; x++)
		{
			memcpy(&destbits[x<<1],&dest,2);
		}
		//直接按行填充颜色
		for (int y = top ; y<bottom ; y++)
		{
			memcpy(&m_buffer_pBits[table[y]+(left<<1)],destbits,rect_Width_b);
		}
		delete[] destbits;
	}
	return Ok;
}

void GP_PixelDrawer::DrawPixel(int x,int y,RGB555 cl,BYTE alpha){
	if (x<0||y<0||x>=m_buffer_w||y>=m_buffer_h)
	{
		return ; 
	}
	int index = table[y] + (x<<1);
	if(alpha == 255)
	{
		//如果没有透明度则，直接填充颜色
		memcpy(&m_buffer_pBits[index],&cl,2);
	}
	else
	{
		//如果有透明度，则计算透明后的颜色
		RGB555 src;
		memcpy(&src,&m_buffer_pBits[index],2); 
		GP_AlphaBlend(src,cl,alpha);
		memcpy(&m_buffer_pBits[index],&src,2);
	}
	return ;

}

GP_Status GP_PixelDrawer::DrawPixelScanLine(int x1,int x2,int y,RGB555 cl){
	if (x1<0||y<0||x1>=m_buffer_w||y>=m_buffer_h||x1>x2||x2<0||x2>=m_buffer_w)
	{
		return Failed; 
	}
	int index = table[y] + (x1<<1);
	//如果没有透明度则，直接填充颜色
	//memcpy(&m_buffer_pBits[index],&cl,2);
	wmemset((wchar_t *)&m_buffer_pBits[index],cl,x2-x1);
	return Ok;
}

GP_Status GP_PixelDrawer::DrawPixelAnti(int x,int y,RGB555 cl,int dst){
	if (x<0||y<0||x>=m_buffer_w||y>=m_buffer_h)
	{
		return Failed; 
	}
	int index = table[y] + (x<<1);

	if (dst == -1)
	{
		return Failed;
	}
	else if (dst == 0)
	{
		//使用本色
	}
	else if (dst == 1)
	{
		RGB555 src;
		memcpy(&src,&m_buffer_pBits[index],2);
		GP_AlphaBlend(src,cl,250);
		cl = src;
	}
	else if (dst == 2)
	{
		RGB555 src;
		memcpy(&src,&m_buffer_pBits[index],2);
		GP_AlphaBlend(src,cl,200);
		cl = src;
	}
	else if (dst == 3)
	{
		RGB555 src;
		memcpy(&src,&m_buffer_pBits[index],2);
		GP_AlphaBlend(src,cl,150);
		cl = src;
	}
	else if (dst == 4)
	{
		RGB555 src;
		memcpy(&src,&m_buffer_pBits[index],2);
		GP_AlphaBlend(src,cl,100);
		cl = src;
	}
	else if (dst == 5)
	{
		RGB555 src;
		memcpy(&src,&m_buffer_pBits[index],2);
		GP_AlphaBlend(src,cl,100);
		cl = src;
	}
	else if (dst == 6)
	{
		RGB555 src;
		memcpy(&src,&m_buffer_pBits[index],2);
		GP_AlphaBlend(src,cl,50);
		cl = src;
	}
	else if (dst == 7)
	{
		RGB555 src;
		memcpy(&src,&m_buffer_pBits[index],2);
		GP_AlphaBlend(src,cl,50);
		cl = src;
	}
	else if (dst == 8)
	{
		RGB555 src;
		memcpy(&src,&m_buffer_pBits[index],2);
		GP_AlphaBlend(src,cl,50);
		cl = src;
	}
	else if (dst == 9)
	{
		RGB555 src;
		memcpy(&src,&m_buffer_pBits[index],2);
		GP_AlphaBlend(src,cl,50);
		cl = src;
	}
	else if (dst == 10)
	{
		RGB555 src;
		memcpy(&src,&m_buffer_pBits[index],2);
		GP_AlphaBlend(src,cl,50);
		cl = src;
	}
	else{
		return Ok;
	}
	//如果没有透明度则，直接填充颜色
	memcpy(&m_buffer_pBits[index],&cl,2);
	return Ok;
}

GP_Status GP_PixelDrawer::DrawPixel(int x,int y,GP_Color& color){

	if (x<0||y<0||x>=m_buffer_w||y>=m_buffer_h)
	{
		return Failed; 
	}

	RGB555 src,result;
	int index = ((y*m_buffer_w)<<1) + (x<<1);
	RGB555 cl = color.Get555Value();
	BYTE alpha = color.GetAlpha();
	if (alpha != 255)
	{
		//如果有透明度，则计算透明后的颜色
		memcpy(&src,&m_buffer_pBits[index],2); 
		GP_AlphaBlend(src,cl,alpha);
		memcpy(&m_buffer_pBits[index],&src,2);
	} 
	else
	{
		//如果没有透明度则，直接填充颜色
		memcpy(&m_buffer_pBits[index],&cl,2);
	}
	return Ok;
}

void GP_PixelDrawer::GP_Draw_Circle_8(int xc,int yc,int x,int y,GP_Color& color){
	DrawPixel(xc+x,yc+y,color);
	DrawPixel(xc-x,yc+y,color);
	DrawPixel(xc+x,yc-y,color);
	DrawPixel(xc-x,yc-y,color);
	DrawPixel(xc+y,yc+x,color);
	DrawPixel(xc-y,yc+x,color);
	DrawPixel(xc+y,yc-x,color);
	DrawPixel(xc-y,yc-x,color);

// 	DrawPixelAnti(xc+x,yc+y,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x,yc+y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x,yc+y,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x,yc+y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+x,yc-y,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x,yc-y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x,yc-y,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x,yc-y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y,yc+x,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y,yc+x,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y,yc+x,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y,yc+x,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y,yc-x,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y,yc-x,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y,yc-x,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y,yc-x,xc,yc) - x*x*10 -y*y*10));
// 
// 	DrawPixelAnti(xc+x,yc+y+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x,yc+y+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x,yc+y+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x,yc+y+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+x,yc-y+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x,yc-y+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x,yc-y+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x,yc-y+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y,yc+x+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y,yc+x+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y,yc+x+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y,yc+x+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y,yc-x+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y,yc-x+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y,yc-x+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y,yc-x+1,xc,yc) - x*x*10 -y*y*10));
// 
// 	DrawPixelAnti(xc+x+1,yc+y,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x+1,yc+y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x+1,yc+y,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x+1,yc+y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+x+1,yc-y,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x+1,yc-y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x+1,yc-y,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x+1,yc-y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y+1,yc+x,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y+1,yc+x,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y+1,yc+x,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y+1,yc+x,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y+1,yc-x,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y+1,yc-x,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y+1,yc-x,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y+1,yc-x,xc,yc) - x*x*10 -y*y*10));
// 
// 	DrawPixelAnti(xc+x+1,yc+y+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x+1,yc+y+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x+1,yc+y+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x+1,yc+y+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+x+1,yc-y+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x+1,yc-y+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x+1,yc-y+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x+1,yc-y+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y+1,yc+x+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y+1,yc+x+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y+1,yc+x+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y+1,yc+x+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y+1,yc-x+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y+1,yc-x+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y+1,yc-x+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y+1,yc-x+1,xc,yc) - x*x*10 -y*y*10));
// 
// 	DrawPixelAnti(xc+x-1,yc+y,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x-1,yc+y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x-1,yc+y,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x-1,yc+y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+x-1,yc-y,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x-1,yc-y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x-1,yc-y,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x-1,yc-y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y-1,yc+x,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y-1,yc+x,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y-1,yc+x,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y-1,yc+x,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y-1,yc-x,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y-1,yc-x,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y-1,yc-x,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y-1,yc-x,xc,yc) - x*x*10 -y*y*10));
// 
// 	DrawPixelAnti(xc+x,yc+y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x,yc+y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x,yc+y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x,yc+y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+x,yc-y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x,yc-y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x,yc-y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x,yc-y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y,yc+x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y,yc+x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y,yc+x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y,yc+x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y,yc-x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y,yc-x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y,yc-x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y,yc-x-1,xc,yc) - x*x*10 -y*y*10));
// 
// 	DrawPixelAnti(xc+x-1,yc+y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x-1,yc+y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x-1,yc+y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x-1,yc+y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+x-1,yc-y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x-1,yc-y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x-1,yc-y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x-1,yc-y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y-1,yc+x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y-1,yc+x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y-1,yc+x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y-1,yc+x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y-1,yc-x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y-1,yc-x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y-1,yc-x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y-1,yc-x-1,xc,yc) - x*x*10 -y*y*10));
// 
// 	DrawPixelAnti(xc+x+1,yc+y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x+1,yc+y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x+1,yc+y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x+1,yc+y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+x+1,yc-y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x+1,yc-y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x+1,yc-y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x+1,yc-y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y+1,yc+x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y+1,yc+x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y+1,yc+x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y+1,yc+x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y+1,yc-x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y+1,yc-x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y+1,yc-x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y+1,yc-x-1,xc,yc) - x*x*10 -y*y*10));
// 
// 	DrawPixelAnti(xc+x-1,yc+y+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x-1,yc+y+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x-1,yc+y+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x-1,yc+y+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+x-1,yc-y+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x-1,yc-y+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x-1,yc-y+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x-1,yc-y+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y-1,yc+x+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y-1,yc+x+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y-1,yc+x+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y-1,yc+x+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y-1,yc-x+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y-1,yc-x+1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y-1,yc-x+1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y-1,yc-x+1,xc,yc) - x*x*10 -y*y*10));
// 
// 
// 	DrawPixelAnti(xc+x,yc+y+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x,yc+y+2,xc,yc) - x*x*10 -y*y*10)/2);
// 	DrawPixelAnti(xc-x,yc+y+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x,yc+y+2,xc,yc) - x*x*10 -y*y*10)/2);
// 	DrawPixelAnti(xc+x,yc-y+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x,yc-y+2,xc,yc) - x*x*10 -y*y*10)/2);
// 	DrawPixelAnti(xc-x,yc-y+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x,yc-y+2,xc,yc) - x*x*10 -y*y*10)/2);
// 	DrawPixelAnti(xc+y,yc+x+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y,yc+x+2,xc,yc) - x*x*10 -y*y*10)/2);
// 	DrawPixelAnti(xc-y,yc+x+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y,yc+x+2,xc,yc) - x*x*10 -y*y*10)/2);
// 	DrawPixelAnti(xc+y,yc-x+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y,yc-x+2,xc,yc) - x*x*10 -y*y*10)/2);
// 	DrawPixelAnti(xc-y,yc-x+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y,yc-x+2,xc,yc) - x*x*10 -y*y*10)/2);
// 	
// 	DrawPixelAnti(xc+x+2,yc+y,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x+2,yc+y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x+2,yc+y,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x+2,yc+y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+x+2,yc-y,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x+2,yc-y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x+2,yc-y,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x+2,yc-y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y+2,yc+x,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y+2,yc+x,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y+2,yc+x,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y+2,yc+x,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y+2,yc-x,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y+2,yc-x,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y+2,yc-x,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y+2,yc-x,xc,yc) - x*x*10 -y*y*10));
// 	
// 	DrawPixelAnti(xc+x+2,yc+y+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x+2,yc+y+2,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x+2,yc+y+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x+2,yc+y+2,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+x+2,yc-y+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x+2,yc-y+2,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x+2,yc-y+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x+2,yc-y+2,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y+2,yc+x+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y+2,yc+x+2,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y+2,yc+x+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y+2,yc+x+2,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y+2,yc-x+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y+2,yc-x+2,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y+2,yc-x+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y+2,yc-x+2,xc,yc) - x*x*10 -y*y*10));
// 	
// 	DrawPixelAnti(xc+x-1,yc+y,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x-1,yc+y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x-1,yc+y,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x-1,yc+y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+x-1,yc-y,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x-1,yc-y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x-1,yc-y,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x-1,yc-y,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y-1,yc+x,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y-1,yc+x,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y-1,yc+x,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y-1,yc+x,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y-1,yc-x,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y-1,yc-x,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y-1,yc-x,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y-1,yc-x,xc,yc) - x*x*10 -y*y*10));
// 	
// 	DrawPixelAnti(xc+x,yc+y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x,yc+y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x,yc+y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x,yc+y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+x,yc-y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x,yc-y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x,yc-y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x,yc-y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y,yc+x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y,yc+x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y,yc+x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y,yc+x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y,yc-x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y,yc-x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y,yc-x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y,yc-x-1,xc,yc) - x*x*10 -y*y*10));
// 	
// 	DrawPixelAnti(xc+x-1,yc+y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x-1,yc+y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x-1,yc+y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x-1,yc+y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+x-1,yc-y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x-1,yc-y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x-1,yc-y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x-1,yc-y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y-1,yc+x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y-1,yc+x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y-1,yc+x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y-1,yc+x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y-1,yc-x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y-1,yc-x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y-1,yc-x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y-1,yc-x-1,xc,yc) - x*x*10 -y*y*10));
// 	
// 	DrawPixelAnti(xc+x+2,yc+y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x+2,yc+y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x+2,yc+y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x+2,yc+y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+x+2,yc-y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x+2,yc-y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x+2,yc-y-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x+2,yc-y-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y+2,yc+x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y+2,yc+x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y+2,yc+x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y+2,yc+x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y+2,yc-x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y+2,yc-x-1,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y+2,yc-x-1,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y+2,yc-x-1,xc,yc) - x*x*10 -y*y*10));
// 	
// 	DrawPixelAnti(xc+x-1,yc+y+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x-1,yc+y+2,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x-1,yc+y+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x-1,yc+y+2,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+x-1,yc-y+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc+x-1,yc-y+2,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-x-1,yc-y+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc-x-1,yc-y+2,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y-1,yc+x+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y-1,yc+x+2,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y-1,yc+x+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y-1,yc+x+2,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc+y-1,yc-x+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc+y-1,yc-x+2,xc,yc) - x*x*10 -y*y*10));
// 	DrawPixelAnti(xc-y-1,yc-x+2,color.Get555Value(),abs(GP_GetDistence2PV2(xc-y-1,yc-x+2,xc,yc) - x*x*10 -y*y*10));
}

int GP_PixelDrawer::GP_GetDistence2PV2(int x1,int y1,int x2,int y2){
	return ((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2))*10;
}

GP_Status GP_PixelDrawer::DrawEllispe(int x,int y,int a,int b,GP_Color& color){
	int a2 = a * a;
	int b2 = b * b;
	int d = b2*4 + a2*(1 - 4*b);
	int tx = 0;
	int ty = b;
	RGB555 cl = color.Get555Value();
	while(2*b2*(tx+1)<a2*(2*ty-1)){
		DrawPixel(tx+x,ty+y,cl);
		DrawPixel(tx+x,-ty+y,cl);
		DrawPixel(-tx+x,ty+y,cl);
		DrawPixel(-tx+x,-ty+y,cl);
		if (d <= 0)
		{
			d += b2*4*(2*tx + 3);
			tx ++;
		} 
		else
		{
			d += b2*4*(2*tx + 3) + a2*4*(2 - 2*ty);
			tx ++;
			ty --;
		}
	} 
	d = b2*(2*tx+1)*(2*tx+1) + 4*a2*(ty-1)*(ty-1) - a2*b2*4;
	while(ty>0){
		DrawPixel(tx+x,ty+y,cl);
		DrawPixel(tx+x,-ty+y,cl);
		DrawPixel(-tx+x,ty+y,cl);
		DrawPixel(-tx+x,-ty+y,cl);
		if (d <= 0)
		{
			d += b2*4*(2*tx + 2) + a2*4*(3 - 2*ty);
			tx ++;
			ty --;
		} 
		else
		{
			d += a2*4*(3 - 2*ty);
			ty --;
		}
	}
	return Ok;
}

int GP_PixelDrawer::minINT(int* piValue,int iCount){
	if (iCount<=0)
	{
		return -1;
	}
	int minint = -1;
	for(int i = 0 ; i < iCount ; i ++)
	{
		minint = minint<piValue[i] ? minint : piValue[i];
	}
	return minint;
}

int GP_PixelDrawer::maxINT(int* piValue,int iCount){
	if (iCount<=0)
	{
		return -1;
	}
	int maxint = -1;
	for(int i = 0 ; i < iCount ; i ++)
	{
		maxint = maxint>piValue[i] ? maxint : piValue[i];
	}
	return maxint;
}

GP_RECT GP_PixelDrawer::GP_GetRangRect(GP_Point* pPointList,int iCount){
	if (iCount<=0)
	{
		return GP_RECT(0,0,0,0);
	}
	int left = pPointList[0].x;
	int top = pPointList[0].y;
	int right= pPointList[0].x;
	int bottom = pPointList[0].y;
	for(int i = 0 ; i < iCount ; i ++)
	{
		left = left < pPointList[i].x ? left : pPointList[i].x;
		right = right > pPointList[i].x ? right : pPointList[i].x;
		top = top < pPointList[i].y ? top : pPointList[i].y;
		bottom = bottom > pPointList[i].y ? bottom : pPointList[i].y;
	}
	return GP_RECT(left,top,right,bottom);
}

GP_Status GP_PixelDrawer::DrawCircle(int x,int y,int r,GP_Color& color){
	if (m_buffer_pBits == NULL)
	{
		return Failed;
	}
	int x0 = 0, y0 = r, yi , d;
	d = 3-(r<<1);

	while(x0<=y0)
	{
		GP_Draw_Circle_8(x,y,x0,y0,color);
		if (d<0)
		{
			d +=(x0<<2) +6;
		} 
		else
		{
			d += ((x0-y0)<<2) +10;
			y0 --;
		}
		x0++;
	}
	return Ok;
}

GP_Status GP_PixelDrawer::FillCircle(int x,int y,int r,GP_Color& color){
	if (m_buffer_pBits == NULL)
	{
		return Failed;
	}
	if (x+r<0 || x-r>= m_buffer_w || y+r<0 || y-r>m_buffer_h)
	{
		return Failed;
	}
	
	int x0 = 0, y0 = r, yi , d;
	d = 3-(r<<1);
	
	while(x0<=y0)
	{
		for (int yi = x0 ; yi < y0 ; yi++)
		{
			GP_Draw_Circle_8(x,y,x0,yi,color); 
		}
		if (d<0)
		{
			d +=(x0<<2) +6;
		} 
		else
		{
			d += ((x0-y0)<<2) +10;
			y0 --;
		}
		x0++;
	}
}

GP_Status GP_PixelDrawer::DrawBitMap(int x,int y,GP_BitMap& bmp,BYTE alpha){
	gpPerformanceLog("GP_PixelDrawer::DrawBitMap");
	if (m_buffer_pBits == NULL)
	{
		return Failed;
	}

	GP_HBITMAP bp = bmp.GetHBITMAP();
	int bmp_w = bp.width;
	int bmp_h = bp.hight;
	unsigned char* bmp_pBit = bp.pBits;
	
	//图片全透明，或图片不在缓冲区范围内
	if ( alpha == 0 || (x+bmp_w)<=0 || (y+bmp_h)<=0 || x>=m_buffer_w || y>=m_buffer_h)
	{
		return Failed;
	}

	//计算源位图需要绘图的位置o
	int bmp_x = x > 0 ? 0 : -x;
	int bmp_y = y > 0 ? 0 : -y;

	//计算需要绘制的位图宽
	GP_RECT draw_rect = GP_RectCross(GP_RECT(x,y,x+bmp_w,y+bmp_h),GP_RECT(0,0,m_buffer_w,m_buffer_h));
	int draw_rect_w = draw_rect.Width();
	int draw_rect_h = draw_rect.Hight();

	if(bp.pixelformat == PIXEL_16BPP_RGB555)
	{
		int h_1 = bmp_h-1-bmp_y+draw_rect.top;	//源图片绘制起始行（从最后一行向上绘制）
		int bmp_x_1 = bmp_x << 1;				//源图片内行偏移
		int line_len = draw_rect_w << 1;		//每行绘制长度

		if (alpha == 255)
		{
			//从计算出的绘图区域第一行绘制到最后一行
			for (int i = draw_rect.top ; i < draw_rect.bottom ; i++)
			{
				/* 
				 * h_1 - i （源图片需要绘制的行i，从下往上转换）
				 * ((h_1 - i)*bmp_w<<1) 表示这行第一个像素位置
				 * ((h_1 - i)*bmp_w<<1) + bmp_x_1 加上行内偏移，表示这行需要绘制的起点位置
				 */
 				int index = ((h_1 - i)*bmp_w<<1) + bmp_x_1;

				/*
				 * table[i] 第i行第一个像素点位置
				 * table[i] + (draw_rect.left<<1) 第i行需要绘制起点位置
				 */
 				int index2 = table[i] + (draw_rect.left<<1);

				memcpy(&m_buffer_pBits[index2],&bmp_pBit[index],line_len);
			}
		}
		else
		{
			RGB555 src,dest;
			int bmp_x_1_1 = bmp_x_1 - (draw_rect.left<<1);
			for (int i = draw_rect.top ; i < draw_rect.bottom; i++)
			{
				for (int j = draw_rect.left ; j < draw_rect.right; j++)
				{
					int index = ((h_1 - i)*bmp_w<<1) + bmp_x_1_1 + (j<<1);
					int index2 = table[i] + (j<<1);
					memcpy(&src,&m_buffer_pBits[index2],2);
					memcpy(&dest,&bmp_pBit[index],2); 
					GP_AlphaBlend(src,dest,alpha);
					memcpy(&m_buffer_pBits[index2],&src,2);
				}
			}
		}
	}
	return Ok;
}

GP_Status GP_PixelDrawer::DrawBitMap(int x,int y,int srcx,int srcy,int w,int h,GP_BitMap& bmp,BYTE alpha){

	if (m_buffer_pBits == NULL)
	{
		return Failed;
	}

	GP_HBITMAP bp1 = bmp.GetHBITMAP();
	int w1 = bp1.width;
	int h1 = bp1.hight;
	unsigned char* ppBit1 = bp1.pBits;

	w = w > w1 ? w1 : w;
	h = h > h1 ? h1 : h;

	if (srcx<0 || srcy<0 || alpha ==0 || (x+w)<=0 || (y+h)<=0 || x>=m_buffer_w || y>=m_buffer_h || w<=0 || h<=0 )
	{
		return Failed;
	}

	//计算源位图需要绘图的位置
	int bmp_x = x > 0 ? 0 : -x;
	int bmp_y = y > 0 ? 0 : -y;
	
	//计算需要绘制的位图宽
	GP_RECT draw_rect = GP_RectCross(GP_RECT(x,y,x+w,y+h),GP_RECT(0,0,m_buffer_w,m_buffer_h));
	int draw_rect_w = draw_rect.Width();
	int draw_rect_h = draw_rect.Hight();

	if(bp1.pixelformat == PIXEL_16BPP_RGB555)
	{
		int h1_1_srcy = h1-1-srcy;
		if (alpha == 255)
		{
			for (int i = 0;i < h; i++)
			{
				int k = i+y;
				if ((k<m_buffer_h) && (k>=0) && (srcy+i)<h1)
				{
					int index = ((h1_1_srcy - i)*w1<<1) +(srcx<<1);
					int index2 = table[k] + (x<<1);
					int sizecount = x+w-m_buffer_w;
					sizecount = sizecount>0? m_buffer_w-x:w;
					memcpy(&m_buffer_pBits[index2],&ppBit1[index],sizecount<<1);
				}
			}
		}
		else
		{
			RGB555 src,dest;
			for (int i = 0;i < h; i++)
			{
				if (((i+y)<m_buffer_h) && ((i+y)>=0))
				{
					int k = i+y;
					int k2 = h1_1_srcy - i;
					for (int j = 0 ;j < w; j++)
					{
						if (((j+x)<m_buffer_w) && ((j+x)>=0))
						{
							int index = (k2*w1<<1) + (j<<1) + (srcx<<1);
							int index2 = table[k] + (x<<1) + (j<<1);
							memcpy(&src,&m_buffer_pBits[index2],2);
							memcpy(&dest,&ppBit1[index],2);
							GP_AlphaBlend(src,dest,alpha);
							memcpy(&m_buffer_pBits[index2],&src,2);
						}
					}
				}
			}
		}
	}
	return Ok;
}

GP_Status GP_PixelDrawer::FSBitMap(){
	GP_HBITMAP bp0 = m_buffer->GetBitMapHandle();
	int w0 = bp0.width;
	int h0 = bp0.hight;
	int h1_1 = h0-1;
	unsigned char* ppBit1 = new unsigned char[w0*h0*2];
	unsigned char* ppBit0 = bp0.pBits;
	memcpy(ppBit1,ppBit0,w0*h0*2);
	Clear();

	for (int i = 0;i < h0; i++)
	{
		for (int j = 0 ;j < w0; j++)
		{
			int index = (i*w0<<1) + (j<<1);
			int x = i*(cos((1.0-(double)j/w0)*3.14159)+1) + h0-i;
			int y = i*sin((double)j/w0*3.14159);
			//x += (w0-/*2**/h0)/2;
			int index2 = (y*w0<<1) + (x<<1);
			if (index2>w0*h0*2||index2<0)
			{
				continue;
			}
			memcpy(&ppBit0[index2],&ppBit1[index],2);
		}
	}
	return Ok;
}

GP_Status GP_PixelDrawer::FSBitMap2(){
	GP_HBITMAP bp0 = m_buffer->GetBitMapHandle();
	int w0 = bp0.width;
	int h0 = bp0.hight;
	int h1_1 = h0-1;
	unsigned char* ppBit1 = new unsigned char[w0*h0*2];
	unsigned char* ppBit0 = bp0.pBits;
	memcpy(ppBit1,ppBit0,w0*h0*2);
	Clear();
	if (w0<h0)
	{
		return Failed;
	}
#if 0
	for (int i = 0;i < h0; i++)
	{
		for (int j = 0 ;j < w0; j++)
		{
			int index = (i*w0<<1) + (j<<1);
			int x = i*(cos((1.0-(double)j/w0)*3.14159)+1) + h0-i;
			int y = i*sin((double)j/w0*3.14159);
			x/=2;
			x += (w0-h0)/2;
			y/=2;
			y+=h0/2;
			int index2 = (y*w0<<1) + (x<<1);
			if (index2>w0*h0*2||index2<0)
			{
				continue;
			}
			memcpy(&ppBit0[index2],&ppBit1[index],2);
			y = h0-y;
			x = w0-x;
			index2 = ((y)*w0<<1) + (x<<1);
			memcpy(&ppBit0[index2],&ppBit1[index],2);
		}
	}
	delete[] ppBit1;
	return Ok; 
#else
	int iR = h0/2-1;	//圆半径
	int iX0 = w0/2-1;
	int iY0 = h0/2-1;
	for (int i = 0;i < h0; i++)
	{
		for (int j = 0 ;j < w0; j++)
		{
			double y = sqrt(pow(double(j-iX0),2)+pow(double(i-iY0),2))*2;
			double x = (w0-1)*acos(double(j-iX0)*2/y)/GP_PAI; 
			int xx = GP_ROUND(x);
			int yy = GP_ROUND(y);
			if (xx<0||xx>=w0||yy<0||yy>=h0)
			{
				continue;
			}
			if (i > iR)
			{
				xx = w0-xx; 
			}
			int index = (i*w0<<1) + (j<<1);
#if 1
			int index2 = (yy*w0<<1) + (xx<<1);
			memcpy(&ppBit0[index],&ppBit1[index2],2);
#else
			RGB555 src1,src2,dest;
			int index2_1 = ((int)y*w0<<1) + ((int)x<<1);
			int index2_2 = (yy*w0<<1) + (xx<<1);
			memcpy(&src1,&ppBit1[index2_1],2);
			memcpy(&src2,&ppBit1[index2_2],2);
			GP_Color clr((GetRValue_555(src1)+GetRValue_555(src2))/2,(GetGValue_555(src1)+GetGValue_555(src2))/2,(GetGValue_555(src1)+GetGValue_555(src2))/2);
			dest = clr.Get555Value();
			memcpy(&ppBit0[index],&dest,2);
#endif
		}
	}
	delete[] ppBit1;
	return Ok;
#endif
}

GP_Status GP_PixelDrawer::StretchBitMap(int x,int y,int w,int h, GP_BitMap& bmp,BYTE alpha){

	if (m_buffer_pBits == NULL)
	{
		return Failed;
	}

	if (w ==0 || h ==0)
	{
		return Failed;
	}

	GP_HBITMAP bp1 = bmp.GetHBITMAP();
	int w1 = bp1.width;
	int h1 = bp1.hight;
	unsigned char* ppBit1 = bp1.pBits;
	
	GP_HBITMAP bp0 = m_buffer->GetBitMapHandle();
	int w0 = bp0.width;
	int h0 = bp0.hight;
	unsigned char* ppBit0 = bp0.pBits;

	if(bp1.pixelformat == PIXEL_16BPP_RGB555)
	{

		int startI = y<0? 0:y;
		int startJ = x<0? 0:x;
		int endI = y+h>h0?h0:y+h;
		int endJ = x+w>w0?w0:x+w; 
// 		int tx = minW-x;
// 		int ty = minH-y; 
		int h1_1 = h1-1;

		//函数能够处理的最大图片尺寸65536*65536
		unsigned long xrIntFloat_16=(unsigned long)((unsigned long)w1<<16)/(unsigned long)w+1; //16.16格式定点数 
		unsigned long yrIntFloat_16=(unsigned long)((unsigned long)h1<<16)/(unsigned long)h+1; //16.16格式定点数 (j*xrIntFloat_16)>>16 (i*yrIntFloat_16)>>16

		if (alpha == 255)
		{
			for (int i = startI;i < endI; i++)
			{
				unsigned long srcy = (((unsigned long)(i-y))*yrIntFloat_16)>>16;
				//if (((i+y)<h0) && ((i+y)>=0))
				{
					for (int j = startJ ;j < endJ; j++)
					{
						//if (((j+x)<w0) && ((j+x)>=0))
						{
							unsigned long srcx = (((unsigned long)(j-x))*xrIntFloat_16)>>16;
							int index = ((h1_1 - srcy)*w1<<1) + (srcx<<1);
							int index2 = table[i] + (j<<1);
							memcpy(&ppBit0[index2],&ppBit1[index],2);
						}
					}
				}
			}
		}
		else
		{
			RGB555 src,dest;
			
			for (int i = startI;i < endI; i++)
			{
				unsigned long srcy = (((unsigned long)(i-y))*yrIntFloat_16)>>16;
				//if (((i+y)<h0) && ((i+y)>=0))
				{
					for (int j = startJ ;j < endJ; j++)
					{
						//if (((j+x)<w0) && ((j+x)>=0))
						{
							unsigned long srcx = (((unsigned long)(j-x))*xrIntFloat_16)>>16;
							int index = ((h1_1 - srcy)*w1<<1) + (srcx<<1);
							int index2 = table[i] + (j<<1);
							
							memcpy(&src,&ppBit0[index2],2);
							memcpy(&dest,&ppBit1[index],2);

							GP_AlphaBlend(src,dest,alpha);
							
							memcpy(&ppBit0[index2],&src,2);
						}
					}
				}
			}
		}
	}
	return Ok;
}

int GP_PixelDrawer::minINT(int &a,int &b,int &c){
	int ret = (a>b) ? b:a;
	ret = (ret>c) ? c:ret;
	return ret;
}

int GP_PixelDrawer::minINT(int &a,int &b){
	return a > b ? b : a;
}

int GP_PixelDrawer::maxINT(int &a,int &b,int &c){
	int ret = (a>b) ? a:b;
	ret = (ret>c) ? ret:c;
	return ret;
}

int GP_PixelDrawer::maxINT(int &a,int &b){
	return a > b ? a : b;
}

GP_Status GP_PixelDrawer::StretchBitMapRotate(int x,int y,int w,int h, GP_BitMap& bmp,int angle,BYTE alpha){

	if (m_buffer_pBits == NULL)
	{
		return Failed;
	}

	GP_HBITMAP bp1 = bmp.GetHBITMAP();
	int w1 = bp1.width;
	int h1 = bp1.hight;
	unsigned char* ppBit1 = bp1.pBits;
	
	GP_HBITMAP bp0 = m_buffer->GetBitMapHandle();
	int w0 = bp0.width;
	int h0 = bp0.hight;
	unsigned char* ppBit0 = bp0.pBits;

	if (angle<0)
	{
		while (angle<0)
		{
			angle += 360;
		}
	}
	else if (angle>360)
	{
		while (angle<0)
		{
			angle -= 360;
		}
	}

	int x1,x2,x3,y1,y2,y3,maxW,maxH,minW,minH,srcX,srcY,dstW,dstH;

	float sinA,cosA;
	sinA = table_sinF[angle];//sin(angle*0.017459);
	cosA = table_cosF[angle];//cos(angle*0.017459);
	x1 = -h*sinA;
	y1 = h*cosA;
	x2 = w*cosA-h*sinA;
	y2 = h*cosA+w*sinA;
	x3 = w*cosA; 
	y3 = w*sinA;

	unsigned long sinAL,cosAL;
	sinAL = ((unsigned long)(sinA*65535));
	cosAL = ((unsigned long)(cosA*65535));
// 	sinAL = table_sinL[angle];
// 	cosAL = table_cosL[angle];
// 	x1 = -(h*sinAL>>16);
// 	y1 = (h*cosAL)>>16;
// 	int tempint = (w*cosAL-h*sinAL);
// 	x2 = tempint>0? tempint>>16 : -((-tempint)>>16);
// 	y2 = (h*cosAL+w*sinAL)>>16;
// 	x3 = (w*cosAL)>>16;
// 	y3 = (w*sinAL)>>16;

	minW = minINT(x1,x2,x3);
	minW = minW>0?0:minW;
	minH = minINT(y1,y2,y3);
	minH = minH>0?0:minH;
	maxW = maxINT(x1,x2,x3);
	maxW = maxW>0?maxW:0;
	maxH = maxINT(y1,y2,y3);
	maxH = maxH>0?maxH:0;
	dstW = maxW - minW;
	dstH = maxH - minH;
	
	y+=minH;
	x+=minW;
	
	if(bp1.pixelformat == PIXEL_16BPP_RGB555)
	{
		int startI = y<0? 0:y;
		int startJ = x<0? 0:x;
		int endI = y+dstH>h0?h0:y+dstH;
		int endJ = x+dstW>w0?w0:x+dstW; 
		int tx = minW-x;
		int ty = minH-y; 
		int h1_1 = h1-1;
		for (int i = startI;i < endI; i++)	//(int i = 0;i < dstH; i++)
		{
			for (int j = startJ ;j < endJ; j++)	//(int j = 0 ;j < dstW; j++)
			{
 				int srcX = (((j+tx)*cosAL + (i+ty)*sinAL)>>16);
				int srcY = (((i+ty)*cosAL - (j+tx)*sinAL)>>16);
				if (srcX>=0 && srcY>=0 && srcX<w && srcY<h)
				{
					int index = ((h1_1 - srcY)*w1<<1) + (srcX<<1);   
					int index2 = table[i] + (j<<1);//((i*w0)<<1) table[y+i]  (x<<1) + (j<<1)
					if (alpha == 255)
					{
						memcpy(&ppBit0[index2],&ppBit1[index],2);
					}
					else 
					{
						RGB555 src,dest;
						memcpy(&src,&ppBit0[index2],2);
						memcpy(&dest,&ppBit1[index],2);
						GP_AlphaBlend(src,dest,alpha);
						memcpy(&ppBit0[index2],&src,2);
					}
				}
			}
		}	
	}
	return Ok;
}

void GP_PixelDrawer::GP_AlphaBlend(RGB555& src,RGB555& dest,BYTE alpha){
	//易读方法
// 	BYTE r_src,g_src,b_src,r_dest,g_dest,b_dest,r_result,g_result,b_result;
// 	
// 	r_src = (src&0x7FFF)>>10;
// 	g_src = (src&0x03FF)>>5;
// 	b_src = src&0x001F;
// 	
// 	r_dest = (dest&0x7FFF)>>10;
// 	g_dest = (dest&0x03FF)>>5;
// 	b_dest = dest&0x001F;
// 	
// 	r_result = r_src+((r_dest-r_src)*alpha>>8);
// 	g_result = g_src+((g_dest-g_src)*alpha>>8);
// 	b_result = b_src+((b_dest-b_src)*alpha>>8);
// 	
// 	src = (r_result<<10) | (g_result<<5) | (b_result);
 
	//简化为，减少局部变量，调用次数频繁,高精度混合
// 	src = (((((src&0x7FFF)>>10)+((((dest&0x7FFF)>>10)-((src&0x7FFF)>>10))*alpha>>8)))<<10) 
// 		| ((((src&0x03FF)>>5)+((((dest&0x03FF)>>5)-((src&0x03FF)>>5))*alpha>>8))<<5) 
// 		| ((src&0x001F)+(((dest&0x001F)-(src&0x001F))*alpha>>8));

	//简化为，减少局部变量，调用次数频繁,低精度混合
// 	src = (((((src&0x7FFF)>>10)+((((dest&0x7FFF)>>10)-((src&0x7FFF)>>10))*(alpha>>3)>>5)))<<10) 
// 		| ((((src&0x03FF)>>5)+((((dest&0x03FF)>>5)-((src&0x03FF)>>5))*(alpha>>3)>>5))<<5) 
// 		| ((src&0x001F)+(((dest&0x001F)-(src&0x001F))*(alpha>>3)>>5));

	//简化为，减少局部变量，调用次数频繁,低精度混合,加速法 
// 	RGB555 temp_src_RB = src & 0x7C1F;
// 	RGB555 temp_dest_RB = dest & 0x7C1F;
// 	RGB555 temp_src_G = src & 0x03E0;
// 	RGB555 temp_dest_G = dest & 0x03E0;
// 	RGB555 temp_result_RB = (temp_src_RB + ((temp_dest_RB - temp_src_RB)*(alpha>>3)>>5)) & 0x7C1F;
// 	RGB555 temp_result_G = (temp_src_G + ((temp_dest_G - temp_src_G)*(alpha>>3)>>5)) & 0x03E0;
// 	src = temp_result_RB | temp_result_G;

	src = (((src & 0x7C1F) + (((dest & 0x7C1F) - (src & 0x7C1F))*(alpha>>3)>>5)) & 0x7C1F) | (((src & 0x03E0) + (((dest & 0x03E0) - (src & 0x03E0))*(alpha>>3)>>5)) & 0x03E0);
}

GP_Status GP_PixelDrawer::Clear(){
	m_buffer->Clear();
	return Ok;
}

GP_RECT GP_PixelDrawer::GP_RectCross(GP_RECT rect1,GP_RECT rect2){
	//矩形无交集
	if (rect1.right < rect2.left || rect1.left > rect2.right || rect1.top > rect2.bottom || rect1.bottom < rect2.top)
	{
		return GP_RECT(0,0,0,0);
	}
 	int rect_left = maxINT(rect1.left,rect2.left);
	int rect_top = maxINT(rect1.top,rect2.top);
	int rect_right = minINT(rect1.right,rect2.right);
	int rect_bottom = minINT(rect1.bottom,rect2.bottom);
	return GP_RECT(rect_left,rect_top,rect_right,rect_bottom);
}

GP_Status GP_PixelDrawer::Update(HDC hdc,int x,int y){
	if (m_buffer_pBits == NULL)
	{
		return Failed;
	}
#ifdef WIN32
	BITMAPINFO hDib;
	memset(&hDib, 0, sizeof(BITMAPINFO));
	
	hDib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	hDib.bmiHeader.biPlanes = 1;

	hDib.bmiHeader.biCompression = BI_RGB; 
	hDib.bmiHeader.biClrUsed = 0;
	hDib.bmiHeader.biClrImportant = 0;

	GP_HBITMAP bmp = m_buffer->GetBitMapHandle();

	//更新时需要重新设置绘制区域宽高和图片大小
	hDib.bmiHeader.biWidth = bmp.width;
	
	hDib.bmiHeader.biHeight = -bmp.hight;
	
	int bitcount = 0;
	if (bmp.pixelformat == PIXEL_16BPP_RGB565 || bmp.pixelformat == PIXEL_16BPP_RGB555)
	{
		bitcount = 16;
	} 
	else if(bmp.pixelformat == PIXEL_24BPP_RGB888)
	{
		bitcount = 24;
	}
	else if (bmp.pixelformat == PIXEL_32BPP_ARGB8888)
	{
		bitcount = 32;
	}

	hDib.bmiHeader.biBitCount = bitcount;
	hDib.bmiHeader.biSizeImage = bmp.hight * bmp.width * bitcount;

	SetDIBitsToDevice(hdc, x, y, bmp.width, bmp.hight,
		0, 0, 0, bmp.hight,
		bmp.pBits, &hDib, DIB_RGB_COLORS);
#elif defined _VXWORKS
	
#define GetRValue_555(w)    ((BYTE)(((w) & 0x7C00) >> 7))
#define GetGValue_555(w)    ((BYTE)(((w) & 0x03E0) >> 2))
#define GetBValue_555(w)    ((BYTE)(((w) & 0x001F) << 3))
	
	{
		if (hdc == 0)
			return Failed;
	}
	
	const int PIXELSIZE_SRC = 2;
	int PIXELSIZE_DST = 3;
	DFBSurfacePixelFormat pf;
	hdc->GetPixelFormat(hdc,&pf);
	if (pf == DSPF_RGB24)
		PIXELSIZE_DST = 3;
	else if(pf == DSPF_RGB32)
		PIXELSIZE_DST = 4;
	else if (pf == DSPF_RGB16)
		PIXELSIZE_DST = 2;
	else
	{
		printf("FATAL ERROR: SetDIBitsToDevice, unsupported pixelformat[(%d)]! change ReWorks bpp to 16/24/32!\n",pf);
		return Failed;
	}
	
	GP_HBITMAP bmp = m_buffer->GetBitMapHandle();
	int bitcount = 0;
	if (bmp.pixelformat == PIXEL_16BPP_RGB565 || bmp.pixelformat == PIXEL_16BPP_RGB555)
	{
		bitcount = 16;
	} 
	else if(bmp.pixelformat == PIXEL_24BPP_RGB888)
	{
		bitcount = 24;
	}
	else if (bmp.pixelformat == PIXEL_32BPP_ARGB8888)
	{
		bitcount = 32;
	}
	
	static unsigned char* dib = NULL;
	if(dib == NULL){
		dib = new unsigned char[bmp.hight * bmp.width * PIXELSIZE_DST];
	}
	
	BYTE* pDst = (BYTE*)&dib[0];
	BYTE* pSrc = (BYTE*)bmp.pBits;
	
	//GISE bitmapbuffer格式RGB565
	//DFB格式：RGB32
	for (int i = 0; i < bmp.hight; i++)
	{		
		pSrc = (BYTE*)bmp.pBits + i*bmp.width*bitcount/8;
		pDst = (BYTE*)&dib[0] + i*bmp.width*PIXELSIZE_DST;
		if (pf == DSPF_RGB24 || pf == DSPF_RGB32)
		{
			for (int j = 0; j < bmp.width; j++)
			{
				unsigned short clr = *(unsigned short*)pSrc;
				*(pDst+2) = GetRValue_555(clr);
				*(pDst+1) = GetGValue_555(clr);
				*(pDst)	  = GetBValue_555(clr);
				pSrc += PIXELSIZE_SRC;
				pDst += PIXELSIZE_DST;
			}
		}
		else if (pf == DSPF_RGB16)
		{
#if 0
			BYTE r,g,b;
			for (int j = 0; j < dwWidth; j++)
			{
				unsigned short clr = *(unsigned short*)pSrc;
				r = GetRValue_565(clr);
				g = GetGValue_565(clr);
				b = GetBValue_565(clr);
				*((WORD*)pDst)=RGB_565(r,g,b);
				pSrc += PIXELSIZE_SRC;
				pDst += PIXELSIZE_DST;
			}
#else
			memcpy(pDst,pSrc,bmp.width*PIXELSIZE_SRC);
#endif
		}
	}
	
	int pos_x,pos_y;
	hdc->GetPosition(hdc,&pos_x,&pos_y);
	DFBRectangle rect;
	rect.x = x + pos_x; 
	rect.y = y + pos_y;
	rect.w = bmp.width;
	rect.h = bmp.hight;
	DFBResult r = hdc->Write(hdc,&rect,&dib[0],bmp.width*PIXELSIZE_DST);
	DFBSurfaceFlipFlags flipflag;
	*(int*)&flipflag = 0;
	hdc->Flip (hdc, NULL, flipflag);
#endif
	return Ok;
}