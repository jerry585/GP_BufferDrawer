#ifndef _GP_PIXELBUFFER_H_
#define _GP_PIXELBUFFER_H_

#include "GP_CommonType.h"

class GP_PixelBuffer{
public:
	//GP_PixelBuffer(int w,int h,PixelFormat pf = PIXEL_16BPP_RGB565);
	GP_PixelBuffer(int w,int h,GP_PixelFormat pf = PIXEL_16BPP_RGB555,unsigned char* buffer = NULL);
	~GP_PixelBuffer();

	unsigned char* GetBitPtr();

	void Clear();

	void Release();

	bool Init(GP_PixelBuffer* pPixBuffer);

	GP_HBITMAP GetBitMapHandle();

private:
	GP_HBITMAP m_hbitmap;
	int m_Width;
	int m_Hight;
	int m_bitcount;
	GP_PixelFormat m_PixelFormat;
};

#endif