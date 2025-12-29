
// ****************************************************************************
//
//                            PicoPad Screen Shot
//
// ****************************************************************************
// PicoLibSDK - Alternative SDK library for Raspberry Pico and RP2040
// Copyright (c) 2023 Miroslav Nemecek, Panda38@seznam.cz, hardyplotter2@gmail.com
// 	https://github.com/Panda381/PicoLibSDK
//	https://www.breatharian.eu/hw/picolibsdk/index_en.html
//	https://github.com/pajenicko/picopad
//	https://picopad.eu/en/
// License:
//	This source code is freely available for any purpose, including commercial.
//	It is possible to take and modify the code or parts of it, without restriction.

#if USE_PICOPAD

#ifndef _PICOPAD_SS_H
#define _PICOPAD_SS_H

#include "../../_lib/inc/lib_fat.h"
#include "../../_lib/inc/lib_sd.h"

#ifdef __cplusplus
extern "C" {
#endif

#if USE_SCREENSHOT || USE_EMUSCREENSHOT		// use screen shots

#if USE_EMUSCREENSHOT		// use emulator screen shots
extern volatile Bool DoEmuScreenShot;	// request to do emulator screenshot
#endif

/*
// BMP file header (size 70 bytes)
#pragma pack(push,1)
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
} sBmp;
#pragma pack(pop)
*/

extern Bool ScreenShotDiskWasMount; // disk was mounted
extern volatile Bool ScreenShotIsOpen; // screen shot was open
extern sFile ScreenShotFile; // screen shot file

// BMP file header, 16 bits per pixel
extern const sBmp BmpHeader;

// open screenshot (returns False if cannot open)
Bool OpenScreenShot();

// write data to screenshot file
void WriteScreenShot(const void* data, int num);

// close screenshot
void CloseScreenShot();

// Do one screen shot (generates file SCRxxxxx.BMP in root of SD card)
void ScreenShot();

// Do one small screen shot (generates file SCRxxxxx.BMP in root of SD card)
void SmallScreenShot();

#endif // USE_SCREENSHOT || USE_EMUSCREENSHOT	// use screen shots

#ifdef __cplusplus
}
#endif

#endif // _PICOPAD_SS_H

#endif // USE_PICOPAD
