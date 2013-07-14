#ifndef _GP_BITMAP_H_
#define _GP_BITMAP_H_

#include "GP_CommonType.h"
#include "pico/picostorage.hpp"
#include "pico/os.hpp"
#include "pico/offsetfile.hpp"

class GP_BitMap{
public:
	GP_BitMap();
	GP_BitMap(int w,int h,GP_PixelFormat pf = PIXEL_16BPP_RGB565);
	~GP_BitMap();
	bool LoadFromFile(const char* filepath);
	bool LoadFromGPFile(File fp);
	GP_HBITMAP GetHBITMAP();
private:
	void ChangeTo555();
private:
	GP_HBITMAP m_hBitMap;
};

#endif