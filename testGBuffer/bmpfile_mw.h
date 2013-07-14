 

#ifndef __bmpfile_h__
#define __bmpfile_h__

#ifdef __cplusplus
	#define BMP_BEGIN_DECLS extern "C" {
	#define BMP_END_DECLS }
#else
	#define BMP_BEGIN_DECLS
	#define BMP_END_DECLS
#endif

BMP_BEGIN_DECLS

#ifndef FALSE
#	define FALSE (0)
#	define TRUE !FALSE
#endif

#ifndef bmp_uint8_t
typedef unsigned char bmp_uint8_t;
#endif

#ifndef bmp_uint16_t
typedef unsigned short bmp_uint16_t;
#endif

#ifndef bmp_uint32_t
typedef unsigned int bmp_uint32_t;
#endif


enum bmp_compression_method_t
{
  GISE_BI_RGB = 0,
  GISE_BI_RLE8,
  GISE_BI_RLE4,
  GISE_BI_BITFIELDS,
  GISE_BI_JPEG,
  GISE_BI_PNG
} ;

typedef struct {
  bmp_uint8_t blue;
  bmp_uint8_t green;
  bmp_uint8_t red;
  bmp_uint8_t alpha;
} rgb_pixel_t;

typedef struct {
  bmp_uint8_t magic[2];   /* the magic number used to identify the BMP file:
			 0x42 0x4D (Hex code points for B and M).
			 The following entries are possible:
			 BM - Windows 3.1x, 95, NT, ... etc
			 BA - OS/2 Bitmap Array
			 CI - OS/2 Color Icon
			 CP - OS/2 Color Pointer
			 IC - OS/2 Icon
			 PT - OS/2 Pointer. */
  bmp_uint32_t filesz;    /* the size of the BMP file in bytes */
  bmp_uint16_t creator1;  /* reserved. */
  bmp_uint16_t creator2;  /* reserved. */
  bmp_uint32_t offset;    /* the offset, i.e. starting address,
			 of the byte where the bitmap data can be found. */
} bmp_header_t;

typedef struct {
	bmp_uint32_t header_sz;     /* the size of this header (40 bytes) */
	bmp_uint32_t width;         /* the bitmap width in pixels */
	bmp_uint32_t height;        /* the bitmap height in pixels */
  bmp_uint16_t nplanes;       /* the number of color planes being used.
			     Must be set to 1. */
  bmp_uint16_t depth;         /* the number of bits per pixel,
			     which is the color depth of the image.
			     Typical values are 1, 4, 8, 16, 24 and 32. */
  bmp_uint32_t compress_type; /* the compression method being used.
			     See also bmp_compression_method_t. */
  bmp_uint32_t bmp_bytesz;    /* the image size. This is the size of the raw bitmap
			     data (see below), and should not be confused
			     with the file size. */
  bmp_uint32_t hres;          /* the horizontal resolution of the image.
			     (pixel per meter) */
  bmp_uint32_t vres;          /* the vertical resolution of the image.
			     (pixel per meter) */
  bmp_uint32_t ncolors;       /* the number of colors in the color palette,
			     or 0 to default to 2<sup><i>n</i></sup>. */
  bmp_uint32_t nimpcolors;    /* the number of important colors used,
			     or 0 when every color is important;
			     generally ignored. */
} bmp_dib_v3_header_t;

typedef struct _bmpfile bmpfile_t;

bmpfile_t *bmp_create(bmp_uint32_t width, bmp_uint32_t height, bmp_uint32_t depth);
/* TODO */
/* bmpfile_t *bmp_create_from_file(const char *filename); */
void bmp_destroy(bmpfile_t *bmp);

bmp_header_t bmp_get_header(bmpfile_t *bmp);
bmp_dib_v3_header_t bmp_get_dib(bmpfile_t *bmp);

bmp_uint32_t bmp_get_width(bmpfile_t *bmp);
bmp_uint32_t bmp_get_height(bmpfile_t *bmp);
bmp_uint32_t bmp_get_depth(bmpfile_t *bmp);

bmp_uint32_t bmp_get_dpi_x(bmpfile_t *bmp);
bmp_uint32_t bmp_get_dpi_y(bmpfile_t *bmp);
void bmp_set_dpi(bmpfile_t *bmp, bmp_uint32_t x, bmp_uint32_t y);

rgb_pixel_t *bmp_get_pixel(bmpfile_t *bmp, bmp_uint32_t x, bmp_uint32_t y);
int bmp_set_pixel(bmpfile_t *bmp, bmp_uint32_t x, bmp_uint32_t y, rgb_pixel_t pixel);

int bmp_save(bmpfile_t *bmp, const char *filename);

int bmp_save_to_file(const char* filename,int width,int height,int colordepth,int rowstride,unsigned char* pixels);

BMP_END_DECLS

#endif /* __bmpfile_h__ */
