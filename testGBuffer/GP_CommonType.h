#ifndef _GP_COMMONTYPE_H_
#define _GP_COMMONTYPE_H_
//λͼ��ʽ����
typedef enum _GP_PixelFormat{
	PIXEL_16BPP_RGB565=0
	,PIXEL_16BPP_RGB555
	,PIXEL_24BPP_RGB888
	,PIXEL_32BPP_ARGB8888
} GP_PixelFormat;

//λͼ
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

//��
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

//����ֵ
inline int ABS(int x){ return x > 0 ? x : -x;} ;

//��������
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

//!����ִ�в���״̬��
enum GP_Status
{
	Ok = 0,
	Failed = 1
};

//! RGBA��ɫͨ��λ��ö�١�
enum
{
	//!Alphaͨ��λ�ơ�
	AlphaShift = 24 ,
	//!Redͨ��λ�ơ�
	RedShift   = 16 ,
	//!Greenͨ��λ�ơ�
	GreenShift =  8 ,
	//!Blueͨ��λ�ơ�
	BlueShift  =  0 
};

//λͼ��ɫ��ʽ�ڴ�
typedef unsigned long ARGB;
typedef unsigned short RGB565;
typedef unsigned short RGB555;
typedef unsigned char BYTE;

class GP_Color 
{
protected:
	ARGB m_argb ;
	
public:
	// ���캯��
	GP_Color(ARGB argb = 0xFF000000): m_argb(argb){}
	GP_Color(BYTE a, BYTE r, BYTE g, BYTE b) {m_argb = MakeARGB(a,r,g,b);}
	GP_Color(BYTE r, BYTE g, BYTE b) {m_argb = MakeARGB(255,r,g,b);}
	GP_Color(BYTE a, const GP_Color& color) {m_argb = MakeARGB(a,color.GetRed(), color.GetGreen(), color.GetBlue());}
	
	~GP_Color(){};
	
	// ��ȡֵ
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
	
	// ����ֵ
	void SetValue(BYTE a, BYTE r, BYTE g, BYTE b) { m_argb = MakeARGB(a,r,g,b); }
	
	
	// ���ARGBֵ
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
	unsigned long bfSize; //�ļ���С
	unsigned short bfReserved1; //�����֣�������
	unsigned short bfReserved2; //�����֣�ͬ��
	unsigned long bfOffBits; //ʵ��λͼ���ݵ�ƫ���ֽ�������ǰ�������ֳ���֮��
} GP_BITMAPFILEHEADER;


//��ϢͷBITMAPINFOHEADER��Ҳ��һ���ṹ���䶨�����£�
typedef struct _tagBITMAPINFOHEADER{
	//public:
	unsigned long biSize; //ָ���˽ṹ��ĳ��ȣ�Ϊ40
	long biWidth; //λͼ��
	long biHeight; //λͼ��
	unsigned short biPlanes; //ƽ������Ϊ1
	unsigned short biBitCount; //������ɫλ����������1��2��4��8��16��24���µĿ�����32
	unsigned long biCompression; //ѹ����ʽ��������0��1��2������0��ʾ��ѹ��
	unsigned long biSizeImage; //ʵ��λͼ����ռ�õ��ֽ���
	long biXPelsPerMeter; //X����ֱ���
	long biYPelsPerMeter; //Y����ֱ���
	unsigned long biClrUsed; //ʹ�õ���ɫ�������Ϊ0�����ʾĬ��ֵ(2^��ɫλ��)
	unsigned long biClrImportant; //��Ҫ��ɫ�������Ϊ0�����ʾ������ɫ������Ҫ��
} GP_BITMAPINFOHEADER;

//��ɫ��Palette����Ȼ�������Ƕ���Щ��Ҫ��ɫ���λͼ�ļ����Եġ�24λ��32λ�ǲ���Ҫ��ɫ��ġ�
//���ƺ��ǵ�ɫ��ṹ���������ʹ�õ���ɫ������
typedef struct _tagRGBQUAD {
	//public:
	unsigned char rgbBlue; //����ɫ����ɫ����
	unsigned char rgbGreen; //����ɫ����ɫ����
	unsigned char rgbRed; //����ɫ�ĺ�ɫ����
	unsigned char rgbReserved; //����ֵ
} GP_RGBQUAD;


const double GP_PAI = 3.1415926535897932384626433832795;//3.1415926535897932;


//
// Int64 ���Ͷ���
//
#if defined(_MSC_VER)
typedef __int64 GP_INT64;
#else
typedef long long GP_INT64;
#endif

#endif