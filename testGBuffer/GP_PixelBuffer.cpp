#include "GP_PixelBuffer.h"
#include <string.h>

// GP_PixelBuffer::GP_PixelBuffer(int w,int h,PixelFormat pf){
// 	if ( w > 65535 || h > 65535 || w <=0 || h <=0)
// 	{
// 		m_hbitmap.width = 0;
// 		m_hbitmap.hight = 0;
// 		m_hbitmap.pixelformat = PIXEL_16BPP_RGB555;
// 		m_hbitmap.pBits = NULL;
// 		return;
// 	}
// 
// 
// 	m_Width = w;
// 	m_Hight = h;
// 	m_PixelFormat = pf;
// 
// 	m_hbitmap.width = w;
// 	m_hbitmap.hight = h;
// 	m_hbitmap.pixelformat = pf;
// 
// 	m_bitcount = 0;
// 	if (m_hbitmap.pixelformat == PIXEL_16BPP_RGB565 || m_hbitmap.pixelformat == PIXEL_16BPP_RGB555)
// 	{
// 		m_bitcount = 16;
// 	} 
// 	else if(m_hbitmap.pixelformat == PIXEL_24BPP_RGB888)
// 	{
// 		m_bitcount = 24;
// 	}
// 	else if (m_hbitmap.pixelformat == PIXEL_32BPP_ARGB8888)
// 	{
// 		m_bitcount = 32;
// 	}
// 
// 	m_hbitmap.pBits = new unsigned char[w*h*m_bitcount/8];
// 
// 	Clear();
// }

GP_PixelBuffer::GP_PixelBuffer(int w,int h,GP_PixelFormat pf,unsigned char* buffer){
	if ( w > 65535 || h > 65535 || w <=0 || h <=0)
	{
		m_hbitmap.width = 0;
		m_hbitmap.hight = 0;
		m_hbitmap.pixelformat = PIXEL_16BPP_RGB555;
		m_hbitmap.pBits = NULL;
		return;
	}
	
	
	m_Width = w;
	m_Hight = h;
	m_PixelFormat = pf;
	
	m_hbitmap.width = w;
	m_hbitmap.hight = h;
	m_hbitmap.pixelformat = pf;
	
	
	m_bitcount = 0;
	if (m_hbitmap.pixelformat == PIXEL_16BPP_RGB565 || m_hbitmap.pixelformat == PIXEL_16BPP_RGB555)
	{
		m_bitcount = 16;
		m_hbitmap.depth = 16;
	} 
	else if(m_hbitmap.pixelformat == PIXEL_24BPP_RGB888)
	{
		m_bitcount = 24;
		m_hbitmap.depth = 24;
	}
	else if (m_hbitmap.pixelformat == PIXEL_32BPP_ARGB8888)
	{
		m_bitcount = 32;
		m_hbitmap.depth = 32;
	}

	if (buffer == NULL)
	{
		m_hbitmap.pBits = new unsigned char[w*h*m_bitcount/8];
		
		Clear();
	}
	else
	{
		m_hbitmap.pBits = buffer;
	}

}

bool GP_PixelBuffer::Init(GP_PixelBuffer* pPixBuffer){
	if (m_hbitmap.pBits == NULL)
	{
		//本身并未创建成功
		return false;
	}
	if (pPixBuffer == NULL)
	{
		//传入无效buffer
		return false;
	}
	if (m_Width != pPixBuffer->m_Width ||
		m_Hight != pPixBuffer->m_Hight ||
		m_PixelFormat != pPixBuffer->m_PixelFormat ||
		pPixBuffer->GetBitMapHandle().pBits == NULL)
	{
		return false;
	}
	
	memcpy(m_hbitmap.pBits, pPixBuffer->GetBitMapHandle().pBits, m_Width*m_Hight*m_bitcount/8);
	return true;
}

GP_PixelBuffer::~GP_PixelBuffer(){
	delete[] m_hbitmap.pBits;
}

void GP_PixelBuffer::Release(){
	delete this;
}

unsigned char* GP_PixelBuffer::GetBitPtr(){
	return m_hbitmap.pBits;
}

void GP_PixelBuffer::Clear(){
	int bitcount = 0;
	if (m_hbitmap.pixelformat == PIXEL_16BPP_RGB565 || m_hbitmap.pixelformat == PIXEL_16BPP_RGB555)
	{
		bitcount = 16;
	} 
	else if(m_hbitmap.pixelformat == PIXEL_24BPP_RGB888)
	{
		bitcount = 24;
	}
	else if (m_hbitmap.pixelformat == PIXEL_32BPP_ARGB8888)
	{
		bitcount = 32;
	}
	memset(m_hbitmap.pBits,0xFF,m_hbitmap.width*m_hbitmap.hight*bitcount/8);
}

GP_HBITMAP GP_PixelBuffer::GetBitMapHandle(){
	return m_hbitmap;
}