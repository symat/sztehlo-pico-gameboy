
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#ifndef _MAIN_H
#define _MAIN_H

// BMP file header (size 70 bytes)
#pragma pack(push,1)
/*
typedef struct {
	// BMP file header (size 14 bytes)
	u16	bfType;			// 0x00: magic, 'B' 'M' = 0x4D42
	u32	bfSize;			// 0x02: file size (32bit DWORD alignment of pixel data included) = 70 + 280*240*2 = 134470 = 46 0D 02 00 little endian
	u16	bfReserved1;		// 0x06: = 0
	u16	bfReserved2;		// 0x08: = 0
	u32	bfOffBits;		// 0x0A: offset of data bits after file header = 70 (0x46): 46 00 00 00 little endian
	// BMP info header (size 40 bytes) - BITMAPV3INFOHEADER
	u32	biSize;			// 0x0E: size of this info header = 56 (0x38): 38 00 00 00 little endian
	s32	biWidth;		// 0x12: width = 280 (0x118): 18 01 00 00
	s32	biHeight;		// 0x16: height, negate if flip row order = -240 (0xFFFFFF10)
	u16	biPlanes;		// 0x1A: planes = 1 : 01 00 little endian
	u16	biBitCount;		// 0x1C: number of bits per pixel = 16 : 10 00 little endian
	u32	biCompression;		// 0x1E: compression = 3 (BI_BITFIELDS) : 03 00 00 00 little endian
	u32	biSizeImage;		// 0x22: size of data of image (each row aligned to 32bit DWORD) = 280*240*2 = 134 400 (0x20D00) = 00 0D 02 00 little endian
	s32	biXPelsPerMeter;	// 0x26: X pixels per meter = 2834, 72ppi (= 0xB12): 12 0B 00 00 little endian 
	s32	biYPelsPerMeter;	// 0x2A: Y pixels per meter = 2834, 72ppi (= 0xB12): 12 0B 00 00 little endian
	u32	biClrUsed;		// 0x2E: number of user colors (0 = all)
	u32	biClrImportant;		// 0x32: number of important colors (0 = all)
	// BMP color bit mask (size 16 bytes) - color info is also stored in little endian!
	u32	biRedMask;		// 0x36: red mask = 0x0000F800
	u32	biGreenMask;		// 0x3A: green mask = 0x000007E0
	u32	biBlueMask;		// 0x3E: blue mask = 0x0000001F
	u32	biAlphaMask;		// 0x42: alpha mask = 0x00000000
					// 0x46
	// data array: each row size aligned to 4byte DWORD; pixel data starts bottom-left corner
	//            (if height is negative, then we start with top-left corner)
} sBmp;*/
#pragma pack(pop)

// format: 4-bit paletted pixel graphics
// image width: 10 pixels
// image height: 10 lines
// image pitch: 5 bytes
extern const u16 ColSelImg_Pal[3] __attribute__ ((aligned(4)));
extern const u8 ColSelImg[50] __attribute__ ((aligned(4)));
#define COLSEL_IMG_W	10
#define COLSEL_IMG_H	10
#define COLSEL_IMG_KEY	COL_MAGENTA

// format: 4-bit paletted pixel graphics
// image width: 8 pixels
// image height: 10 lines
// image pitch: 4 bytes
extern const u16 ColSel2Img_Pal[3] __attribute__ ((aligned(4)));
extern const u8 ColSel2Img[40] __attribute__ ((aligned(4)));
#define COLSEL2_IMG_W	8
#define COLSEL2_IMG_H	10
#define COLSEL2_IMG_KEY	COL_MAGENTA

// format: 4-bit paletted pixel graphics
// image width: 7 pixels
// image height: 10 lines
// image pitch: 4 bytes
extern const u16 ColSel3Img_Pal[3] __attribute__ ((aligned(4)));
extern const u8 ColSel3Img[40] __attribute__ ((aligned(4)));
#define COLSEL3_IMG_W	7
#define COLSEL3_IMG_H	10
#define COLSEL3_IMG_KEY	COL_MAGENTA

// Cursors
// format: 16-bit pixel graphics
// image width: 32 pixels
// image height: 32 lines
// image pitch: 64 bytes
extern const u16 CurArrowImg[1024] __attribute__ ((aligned(4)));
extern const u16 CurBoxImg[1024] __attribute__ ((aligned(4)));
extern const u16 CurBrushImg[1024] __attribute__ ((aligned(4)));
extern const u16 CurBucketImg[1024] __attribute__ ((aligned(4)));
extern const u16 CurCircleImg[1024] __attribute__ ((aligned(4)));
extern const u16 CurDropImg[1024] __attribute__ ((aligned(4)));
extern const u16 CurLineImg[1024] __attribute__ ((aligned(4)));
extern const u16 CurLine2Img[1024] __attribute__ ((aligned(4)));
extern const u16 CurPencilImg[1024] __attribute__ ((aligned(4)));
extern const u16 CurRectImg[1024] __attribute__ ((aligned(4)));
extern const u16 CurRect2Img[1024] __attribute__ ((aligned(4)));
extern const u16 CurRingImg[1024] __attribute__ ((aligned(4)));
extern const u16 CurRoundImg[1024] __attribute__ ((aligned(4)));
#define CURSOR_W	32		// cursor width
#define CURSOR_H	32		// cursor height
#define CURSOR_KEY	COL_MAGENTA	// cursor key color
#define CURSOR_SUBS	COL_GREEN	// cursor substitute color

// format: 8-bit paletted pixel graphics
// image width: 320 pixels
// image height: 54 lines
// image pitch: 320 bytes
extern const u16 PaletteImg_Pal[216] __attribute__ ((aligned(4)));
extern const u8 PaletteImg[17280] __attribute__ ((aligned(4)));
#define PALETTE_IMG_W	320
#define PALETTE_IMG_H	54

// format: 8-bit paletted pixel graphics
// image width: 60 pixels
// image height: 180 lines
// image pitch: 60 bytes
extern const u16 ToolsImg_Pal[146] __attribute__ ((aligned(4)));
extern const u8 ToolsImg[10800] __attribute__ ((aligned(4)));
#define TOOLS_IMG_W	60
#define TOOLS_IMG_H	180

#endif // _MAIN_H
