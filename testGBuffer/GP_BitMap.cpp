#include <stdio.h>
#include <string.h>
#include "GP_BitMap.h"

#define GP_BI_RGB        0L
#define GP_BI_RLE8       1L
#define GP_BI_RLE4       2L
#define GP_BI_BITFIELDS  3L

GP_BitMap::GP_BitMap(){
	m_hBitMap.width = 0;
	m_hBitMap.hight = 0;
	m_hBitMap.pixelformat = PIXEL_16BPP_RGB555;
}

GP_BitMap::GP_BitMap(int w,int h,GP_PixelFormat pf){
	m_hBitMap.width = w;
	m_hBitMap.hight = h;
	int bitcount = 0;
	if (m_hBitMap.pixelformat == PIXEL_16BPP_RGB565 || m_hBitMap.pixelformat == PIXEL_16BPP_RGB555)
	{
		bitcount = 16;
	} 
	else if(m_hBitMap.pixelformat == PIXEL_24BPP_RGB888)
	{
		bitcount = 24;
	}
	else if (m_hBitMap.pixelformat == PIXEL_32BPP_ARGB8888)
	{
		bitcount = 32;
	}
	m_hBitMap.pBits = new unsigned char[w*h*bitcount/8];
}

GP_BitMap::~GP_BitMap(){
	if (m_hBitMap.pBits != NULL)
	{
		delete[] m_hBitMap.pBits;
	}
}

bool GP_BitMap::LoadFromGPFile(File fp){
	unsigned char buffer[1024] = {0};
	int ncount = 0;
	int offset = 0;
// 	while((ncount = fp.pread(buffer,1024,offset))>0){
// 		os_pwrite(fid,buffer,ncount,offset);
// 		offset+=ncount; 
// 	}

	GP_BITMAPFILEHEADER bitHead;
	GP_BITMAPINFOHEADER bitInfoHead;
	unsigned short fileType;
	ncount = fp.pread(&fileType,sizeof(unsigned short),offset);offset+=ncount;
	if(fileType != 0x4d42)
	{
		printf("BitMapFile is not .bmp file!\n");
		return false;
	}

	ncount = fp.pread(&bitHead,sizeof(GP_BITMAPFILEHEADER),offset);offset+=ncount;
	ncount = fp.pread(&bitInfoHead,sizeof(GP_BITMAPINFOHEADER),offset);offset+=ncount;

	if(bitInfoHead.biBitCount < 24)
	{
		printf("该位图为非真彩色位图，暂不支持\n\n");
	}
	else
	{
		printf("该位图为24位真彩色\n\n");
	}

	offset = bitHead.bfOffBits;

	m_hBitMap.width = bitInfoHead.biWidth;
	m_hBitMap.hight = bitInfoHead.biHeight;

	if (bitInfoHead.biBitCount == 24)
	{
		m_hBitMap.pixelformat = PIXEL_24BPP_RGB888;
	}
	else if (bitInfoHead.biBitCount == 32)
	{
		m_hBitMap.pixelformat = PIXEL_32BPP_ARGB8888;
	}
	else if (bitInfoHead.biBitCount == 16 && bitInfoHead.biCompression == GP_BI_RGB)
	{
		//先默认为555可能有问题
		m_hBitMap.pixelformat = PIXEL_16BPP_RGB555;
	}
	else
	{
		return false;
	}

	if (m_hBitMap.pBits != NULL)
	{
		delete[] m_hBitMap.pBits;
	}
	m_hBitMap.pBits = new unsigned char[bitInfoHead.biWidth*bitInfoHead.biHeight*bitInfoHead.biBitCount/8];
	
	ncount = fp.pread(m_hBitMap.pBits,bitInfoHead.biWidth*bitInfoHead.biHeight*bitInfoHead.biBitCount/8,offset);offset+=ncount;

	if (m_hBitMap.pixelformat != PIXEL_16BPP_RGB555)
	{
		ChangeTo555();
	}
	
	return true;

}

bool GP_BitMap::LoadFromFile(const char* filepath){
	FILE* fp;
	GP_BITMAPFILEHEADER bitHead;
	GP_BITMAPINFOHEADER bitInfoHead;
//	GP_RGBQUAD *pRgb ;

	fp=fopen(filepath,"rb");
	if (fp == NULL)
	{
		printf("BitMapFile \"%s\" opene failed\n",filepath);
		return false;
	}

	unsigned short fileType;
	fread(&fileType,1,sizeof(unsigned short),fp);
	if(fileType != 0x4d42)
	{
		printf("BitMapFile \"%s\" is not .bmp file!\n",filepath);
		fclose(fp);
		return false;
	}

	fread(&bitHead,1,sizeof(GP_BITMAPFILEHEADER),fp);
	fread(&bitInfoHead,1,sizeof(GP_BITMAPINFOHEADER),fp);

	if(bitInfoHead.biBitCount < 24)
	{
		printf("该位图为非真彩色位图，暂不支持\n\n");
//		return false;
	}
	else
	{
		printf("该位图为24位真彩色\n\n");
	}
	fseek(fp,bitHead.bfOffBits,0);
	m_hBitMap.width = bitInfoHead.biWidth;
	m_hBitMap.hight = bitInfoHead.biHeight;
	//bitInfoHead.
	if (bitInfoHead.biBitCount == 24)
	{
		m_hBitMap.pixelformat = PIXEL_24BPP_RGB888;
	}
	else if (bitInfoHead.biBitCount == 32)
	{
		m_hBitMap.pixelformat = PIXEL_32BPP_ARGB8888;
	}
	else if (bitInfoHead.biBitCount == 16 && bitInfoHead.biCompression == GP_BI_RGB)
	{
		//先默认为555可能有问题
		m_hBitMap.pixelformat = PIXEL_16BPP_RGB555;
	}
	else
	{
		fclose(fp);
		return false;
	}

	if (m_hBitMap.pBits != NULL)
	{
		delete[] m_hBitMap.pBits;
	}
	m_hBitMap.pBits = new unsigned char[bitInfoHead.biWidth*bitInfoHead.biHeight*bitInfoHead.biBitCount/8];

	fread(m_hBitMap.pBits,1,bitInfoHead.biWidth*bitInfoHead.biHeight*bitInfoHead.biBitCount/8,fp);

	fclose(fp);

	if (m_hBitMap.pixelformat != PIXEL_16BPP_RGB555)
	{
		ChangeTo555();
	}

	return true;
}

void GP_BitMap::ChangeTo555(){
	unsigned char* temp = new unsigned char[m_hBitMap.width*m_hBitMap.hight<<1];

	if (m_hBitMap.pixelformat == PIXEL_24BPP_RGB888)
	{
		int add = 0;
		//按行扫描
		for (int i = 0; i < m_hBitMap.hight; i++)
		{
			//计算该行偏移量
			add = ((4-(m_hBitMap.width*3)%4)%4)*i;
			for (int j = 0 ;j < m_hBitMap.width; j++)
			{
				int index = (i*m_hBitMap.width*3) + (j*3) +add;
				GP_Color cl(m_hBitMap.pBits[index+2],m_hBitMap.pBits[index+1],m_hBitMap.pBits[index]);
				int index2 = ((i*m_hBitMap.width)<<1) + (j<<1);
				RGB555 color = cl.Get555Value();
				memcpy(&temp[index2],&color,2);
			}
		}
		m_hBitMap.pixelformat = PIXEL_16BPP_RGB555;
		delete[] m_hBitMap.pBits;
		m_hBitMap.pBits = temp;
	} 
	else if(m_hBitMap.pixelformat == PIXEL_32BPP_ARGB8888)
	{
		for (int i = 0;i < m_hBitMap.hight; i++)
		{
			for (int j = 0 ;j < m_hBitMap.width; j++)
			{
				int index = (i*m_hBitMap.width<<2) + (j<<2);
				GP_Color cl(m_hBitMap.pBits[index+2],m_hBitMap.pBits[index+1],m_hBitMap.pBits[index]);
				int index2 = ((i*m_hBitMap.width)<<1) + (j<<1);
				RGB555 color = cl.Get555Value();
				memcpy(&temp[index2],&color,2);
			} 
		}
		m_hBitMap.pixelformat = PIXEL_16BPP_RGB555;
		delete[] m_hBitMap.pBits;
		m_hBitMap.pBits = temp;
	}

}

GP_HBITMAP GP_BitMap::GetHBITMAP(){
	return m_hBitMap;
}