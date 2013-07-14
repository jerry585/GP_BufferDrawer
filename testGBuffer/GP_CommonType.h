#ifndef _GP_COMMONTYPE_H_
#define _GP_COMMONTYPE_H_
//位图格式描述
typedef enum _GP_PixelFormat{
	PIXEL_16BPP_RGB565=0
	,PIXEL_16BPP_RGB555
	,PIXEL_24BPP_RGB888
	,PIXEL_32BPP_ARGB8888
} GP_PixelFormat;

//位图
typedef struct _GP_HBITMAP 
{
	int width;
	int hight;
	GP_PixelFormat pixelformat;
	unsigned char* pBits;
	int depth;
	_GP_HBITMAP(){
		width = 0;
		hight = 0;
		pixelformat = PIXEL_16BPP_RGB565;
		pBits = 0;
	}
}GP_HBITMAP;

//点
typedef struct _GP_Point 
{
	int x;
	int y;
	_GP_Point(){
		x = 0;
		y = 0;
	}
	_GP_Point(int tx,int ty){
		x = tx;
		y = ty;
	}
}GP_Point;

typedef struct _GP_Line 
{
	int x1;
	int y1;
	int x2;
	int y2;
	_GP_Line(){
		x1 = 0;
		y1 = 0;
		x2 = 0;
		y2 = 0;
	}
	_GP_Line(int tx1,int ty1,int tx2,int ty2){
		x1 = tx1;
		y1 = ty1;
		x2 = tx2;
		y2 = ty2;
	}
}GP_Line;


typedef struct _GP_INTLIST 
{
	int x;
	bool px;
	_GP_INTLIST* next;
}GP_INTLIST;

//绝对值
inline int ABS(int x){ return x > 0 ? x : -x;} ;

//矩形区域
typedef struct  _GP_RECT
{
    int left;
    int top;
    int right;
    int bottom;
	_GP_RECT(){
		left = top = right = bottom = 0;
	}
	_GP_RECT(int _left,int _top,int _right,int _bottom){
		if (_left>_right || _top>_bottom)
		{
			left = top = right = bottom = 0;
		}
		else
		{
			left = _left;
			top = _top;
			right = _right;
			bottom = _bottom;
		}
	}
	int Width(){
		return ABS(left - right);
	}
	int Hight(){
		return ABS(top - bottom);
	}
}GP_RECT;

//!返回执行操作状态。
enum GP_Status
{
	Ok = 0,
	Failed = 1
};

//! RGBA颜色通道位移枚举。
enum
{
	//!Alpha通道位移。
	AlphaShift = 24 ,
	//!Red通道位移。
	RedShift   = 16 ,
	//!Green通道位移。
	GreenShift =  8 ,
	//!Blue通道位移。
	BlueShift  =  0 
};

//位图颜色格式内存
typedef unsigned long ARGB;
typedef unsigned short RGB565;
typedef unsigned short RGB555;
typedef unsigned char BYTE;

class GP_Color 
{
protected:
	ARGB m_argb ;
	
public:
	// 构造函数
	GP_Color(ARGB argb = 0xFF000000): m_argb(argb){}
	GP_Color(BYTE a, BYTE r, BYTE g, BYTE b) {m_argb = MakeARGB(a,r,g,b);}
	GP_Color(BYTE r, BYTE g, BYTE b) {m_argb = MakeARGB(255,r,g,b);}
	GP_Color(BYTE a, const GP_Color& color) {m_argb = MakeARGB(a,color.GetRed(), color.GetGreen(), color.GetBlue());}
	
	~GP_Color(){};
	
	// 获取值
	BYTE GetAlpha() const   { return (BYTE) (m_argb >> AlphaShift); }
	BYTE GetRed()   const   { return (BYTE) (m_argb >> RedShift); }
	BYTE GetGreen() const   { return (BYTE) (m_argb >> GreenShift); }
	BYTE GetBlue()  const   { return (BYTE) (m_argb >> BlueShift); }
	
	ARGB GetValue() const { return m_argb; }
	
	RGB565 Get565Value() const { 
		BYTE r = GetRed();
		BYTE g = GetGreen();
		BYTE b = GetBlue(); 
		long R = r*31/255; 
		long G = g*63/255; 
		long B = b*31/255; 
		RGB565 mrgb565 = (R<<11) | (G<<5) | B; 
		return mrgb565; 
	}

	RGB555 Get555Value() const { 
		BYTE r = GetRed();
		BYTE g = GetGreen();
		BYTE b = GetBlue(); 
		long R = r*31/255; 
		long G = g*31/255; 
		long B = b*31/255; 
		RGB555 mrgb555 = (R<<10) | (G<<5) | B; 
		return mrgb555; 
	}
	
	// 设置值
	void SetValue(BYTE a, BYTE r, BYTE g, BYTE b) { m_argb = MakeARGB(a,r,g,b); }
	
	
	// 组合ARGB值
	static ARGB MakeARGB(BYTE a, BYTE r, BYTE g, BYTE b) 
	{
		return ( ((ARGB)a<<AlphaShift) |
			((ARGB)r<<RedShift)   |
			((ARGB)g<<GreenShift) |
			((ARGB)b<<BlueShift) ) ;
	}
	
	const GP_Color& operator=(const GP_Color& src){
		m_argb = src.m_argb ;
		return (*this) ;
	}
};

#define GetRValue_565(w)    ((BYTE)(((w) & 0xF800) >> 8))
#define GetGValue_565(w)    ((BYTE)(((w) & 0x07E0) >> 3))
#define GetBValue_565(w)    ((BYTE)(((w) & 0x001F) << 3))

#define GetRValue_555(w)    ((BYTE)(((w) & 0x7C00) >> 7))
#define GetGValue_555(w)    ((BYTE)(((w) & 0x03E0) >> 2))
#define GetBValue_555(w)    ((BYTE)(((w) & 0x001F) << 3))

#define RGB_565(r, g, b)    ((WORD)(( ( (r) >> 3 ) << 11 ) | ( ( (g) >> 2 ) << 5 ) | (( (b) >> 3 ) << 0)))

#ifndef NULL
#define NULL 0
#endif

typedef struct _tagBITMAPFILEHEADER {
	//WORD bfType;
	unsigned long bfSize; //文件大小
	unsigned short bfReserved1; //保留字，不考虑
	unsigned short bfReserved2; //保留字，同上
	unsigned long bfOffBits; //实际位图数据的偏移字节数，即前三个部分长度之和
} GP_BITMAPFILEHEADER;


//信息头BITMAPINFOHEADER，也是一个结构，其定义如下：
typedef struct _tagBITMAPINFOHEADER{
	//public:
	unsigned long biSize; //指定此结构体的长度，为40
	long biWidth; //位图宽
	long biHeight; //位图高
	unsigned short biPlanes; //平面数，为1
	unsigned short biBitCount; //采用颜色位数，可以是1，2，4，8，16，24，新的可以是32
	unsigned long biCompression; //压缩方式，可以是0，1，2，其中0表示不压缩
	unsigned long biSizeImage; //实际位图数据占用的字节数
	long biXPelsPerMeter; //X方向分辨率
	long biYPelsPerMeter; //Y方向分辨率
	unsigned long biClrUsed; //使用的颜色数，如果为0，则表示默认值(2^颜色位数)
	unsigned long biClrImportant; //重要颜色数，如果为0，则表示所有颜色都是重要的
} GP_BITMAPINFOHEADER;

//调色板Palette，当然，这里是对那些需要调色板的位图文件而言的。24位和32位是不需要调色板的。
//（似乎是调色板结构体个数等于使用的颜色数。）
typedef struct _tagRGBQUAD {
	//public:
	unsigned char rgbBlue; //该颜色的蓝色分量
	unsigned char rgbGreen; //该颜色的绿色分量
	unsigned char rgbRed; //该颜色的红色分量
	unsigned char rgbReserved; //保留值
} GP_RGBQUAD;


const double GP_PAI = 3.1415926535897932384626433832795;//3.1415926535897932;


//
// Int64 类型定义
//
#if defined(_MSC_VER)
typedef __int64 GP_INT64;
#else
typedef long long GP_INT64;
#endif

#endif