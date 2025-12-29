
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

#include "../../global.h"	// globals


#include "../../_display/st7789/st7789.h"
#include "../../_sdk/inc/sdk_timer.h"
#include "picopad_init.h"	// initialize
#include "picopad_ss.h"

#if USE_SCREENSHOT || USE_EMUSCREENSHOT		// use screen shots

#if USE_EMUSCREENSHOT		// use emulator screen shots
volatile Bool DoEmuScreenShot = False;	// request to do emulator screenshot
#endif

Bool ScreenShotDiskWasMount = False; // disk was mounted
volatile Bool ScreenShotIsOpen = False; // screen shot was open
sFile ScreenShotFile; // screen shot file

// BMP file header, 16 bits per pixel
const sBmp BmpHeader = {
	// BMP file header (size 14 bytes)
	0x4D42,			// u16	bfType;		// 0x00: magic, 'B' 'M' = 0x4D42
	70 + SCREEN_WIDTH*SCREEN_HEIGHT*2, // u32	bfSize;		// 0x02: file size, aligned to DWORD: 70 + 280*240*2 = 134470 = 46 0D 02 00 little endian
	0,			// u16	bfReserved1;	// 0x06: = 0
	0,			// u16	bfReserved2;	// 0x08: = 0
	70,			// 0x0A: offset of data bits after file header = 70 (0x46)
	// BMP info header (size 40 bytes)
	56,			// u32	biSize;		// 0x0E: size of this info header = 56 (0x38)
	SCREEN_WIDTH,			// s32	biWidth;	// 0x12: width: 280 (0x118): 18 01 00 00
	-SCREEN_HEIGHT,		// s32	biHeight;	// 0x16: height, negate if flip row order = -240 (0xFFFFFF10)
	1,			// u16	biPlanes;	// 0x1A: planes = 1
	16,			// u16	biBitCount;	// 0x1C: number of bits per pixel = 16
	3,			// u32	biCompression;	// 0x1E: compression = 3 (BI_BITFIELDS)
	SCREEN_WIDTH*SCREEN_HEIGHT*2,	// u32	biSizeImage;	// 0x22: size of data of image (each row aligned to 32bit DWORD) = 280*240*2 = 134 400 (0x20D00) = 00 0D 02 00 little endian
	2834,			// s32	biXPelsPerMeter; // 0x26: X pixels per meter = 2834, 72ppi (= 0xB12): 12 0B 00 00 little endian
	2834,			// s32	biYPelsPerMeter; // 0x2A: Y pixels per meter = 2834, 72ppi (= 0xB12): 12 0B 00 00 little endian
	0,			// u32	biClrUsed;	// 0x2E: number of user colors (0 = all)
	0,			// u32	biClrImportant;	// 0x32: number of important colors (0 = all)
	// BMP color bit mask (size 16 bytes)
	0xF800,			// u32	biRedMask;	// 0x36: red mask = 0x0000F800
	0x07E0,			// u32	biGreenMask;	// 0x3A: green mask = 0x000007E0
	0x001F,			// u32	biBlueMask;	// 0x3E: blue mask = 0x0000001F
	0,			// u32	biAlphaMask;	// 0x42: alpha mask = 0x00000000
							// 0x46
};

// small BMP file header, 16 bits per pixel
const sBmp SmallBmpHeader = {
	// BMP file header (size 14 bytes)
	0x4D42,			// u16	bfType;		// 0x00: magic, 'B' 'M' = 0x4D42
	70 + (SCREEN_WIDTH/2)*(SCREEN_HEIGHT/2)*2, // u32	bfSize;		// 0x02: file size, aligned to DWORD = 70 + 140*120*2 = 33670 = 0x8386 = 86 83 00 00 little endian
	0,			// u16	bfReserved1;	// 0x06: = 0
	0,			// u16	bfReserved2;	// 0x08: = 0
	70,			// 0x0A: offset of data bits after file header = 70 (0x46)
	// BMP info header (size 40 bytes)
	56,			// u32	biSize;		// 0x0E: size of this info header = 56 (0x38)
	SCREEN_WIDTH/2,		// s32	biWidth;	// 0x12: width = 140 (0x8C)
	-SCREEN_HEIGHT/2,		// s32	biHeight;	// 0x16: height, negate if flip row order = -120 (0xFFFFFF88)
	1,			// u16	biPlanes;	// 0x1A: planes = 1
	16,			// u16	biBitCount;	// 0x1C: number of bits per pixel = 16
	3,			// u32	biCompression;	// 0x1E: compression = 3 (BI_BITFIELDS)
	(SCREEN_WIDTH/2)*(SCREEN_HEIGHT/2)*2,	// u32	biSizeImage;	// 0x22: size of data of image + aligned file = 180*120*2 + 2 = 33600 (0x8340)
	2834,			// s32	biXPelsPerMeter; // 0x26: X pixels per meter = 2834, 72ppi (= 0xB12): 12 0B 00 00 little endian
	2834,			// s32	biYPelsPerMeter; // 0x2A: Y pixels per meter = 2834, 72ppi (= 0xB12): 12 0B 00 00 little endian
	0,			// u32	biClrUsed;	// 0x2E: number of user colors (0 = all)
	0,			// u32	biClrImportant;	// 0x32: number of important colors (0 = all)
	// BMP color bit mask (size 16 bytes)
	0xF800,			// u32	biRedMask;	// 0x36: red mask = 0x0000F800
	0x07E0,			// u32	biGreenMask;	// 0x3A: green mask = 0x000007E0
	0x001F,			// u32	biBlueMask;	// 0x3E: blue mask = 0x0000001F
	0,			// u32	biAlphaMask;	// 0x42: alpha mask = 0x00000000
							// 0x46
};

// open screenshot (returns False if cannot open)
Bool OpenScreenShot()
{
	int i;
	char fn[] = "/SCR00001.BMP";

	if (ScreenShotIsOpen) return False; // already open
	ScreenShotIsOpen = True;
	dmb();

	// mounted flag
	ScreenShotDiskWasMount = DiskMounted();

	Bool try = True;
	Bool res;

ScreenShotReset:

	// auto-mount disk
	res = DiskAutoMount();
	if (!res) DiskUnmount();
	if (DiskAutoMount())
	{
		// prepare file name
		while (FileExist(fn))
		{
			i = 8;
			for (;;)
			{
				fn[i]++;
				if (fn[i] <= '9') break;
				fn[i] = '0';
				i--;
			}
		}

		res = FileCreate(&ScreenShotFile, fn);
		if (!res && try)
		{
			DiskUnmount();
			try = False;
			goto ScreenShotReset;
		}

		if (res)
		{
			// write BMP file header
			FileWrite(&ScreenShotFile, &BmpHeader, sizeof(sBmp));

			// was open
			return True;
		}
	}

	// error - was not mounted
	ScreenShotIsOpen = False;
	if (!ScreenShotDiskWasMount) DiskUnmount();
	return False;
}

// write data to screenshot file
void WriteScreenShot(const void* data, int num)
{
	if (ScreenShotIsOpen)
	{
		// write image data
		FileWrite(&ScreenShotFile, data, num);
	}
}

// close screenshot
void CloseScreenShot()
{
	if (ScreenShotIsOpen)
	{
		// close file
		FileClose(&ScreenShotFile);

		// flush disk writes
		DiskFlush();

		// unmount
		if (!ScreenShotDiskWasMount) DiskUnmount();
		dmb();
		ScreenShotIsOpen = False;
	}
}

// Do one screen shot (generates file SCRxxxxx.BMP in root of SD card)
void ScreenShot()
{
	u16* s;
	// open screen shot
	if (OpenScreenShot())
	{
		// write image data (focusing on the middle of the framebuffer, to a screen_width x screen_height area)
		s = FrameBuf;
		int height_offset = (int)((HEIGHT-SCREEN_HEIGHT)/2);
		int width_offset = (int)((WIDTH-SCREEN_WIDTH)/2);
		s += height_offset * WIDTH;
		for (int i = 0; i < SCREEN_HEIGHT; i++)
		{
			s += width_offset;
			WriteScreenShot(s, SCREEN_WIDTH*2);
			s += width_offset + SCREEN_WIDTH;
		}

		// close screenshot
		CloseScreenShot();
	}
}

// Do one small screen shot (generates file SCRxxxxx.BMP in root of SD card)
void SmallScreenShot()
{
	int i, j;
	char fn[] = "/SCR00001.BMP";
	sFile file;
	u16 buf[SCREEN_WIDTH/2];
	u16* s;

	// mounted flag
	Bool ismount = DiskMounted();

	// auto-mount disk
	if (DiskAutoMount())
	{
		// prepare file name
		while (FileExist(fn))
		{
			i = 8;
			for (;;)
			{
				fn[i]++;
				if (fn[i] <= '9') break;
				fn[i] = '0';
				i--;
			}
		}

		if (FileCreate(&file, fn))
		{
			// write BMP file header
			FileWrite(&file, &SmallBmpHeader, sizeof(sBmp));
			// write image data (focusing on the middle of the framebuffer, to a screen_widthxscreen_height area)
			s = FrameBuf;
			int height_offset = (int)((HEIGHT-SCREEN_HEIGHT)/2);
			int width_offset = (int)((WIDTH-SCREEN_WIDTH)/2);
			s+=height_offset*WIDTH;
			for (i = SCREEN_HEIGHT/2; i > 0; i--)
			{
				s += width_offset;
				for (j = 0; j <SCREEN_WIDTH/2; j++)
				{
					buf[j] = RGBBLEND4(s[0], s[1], s[WIDTH], s[WIDTH+1]);
					s += 2;
				}
				s += width_offset;
				s += WIDTH;
				FileWrite(&file, buf, SCREEN_WIDTH/2*2);
			}

			// write align
			//FileWrite(&file, buf, 2);

			// close file
			FileClose(&file);

			// flush disk writes
			DiskFlush();
		}
	}

	// unmount disk
	if (!ismount) DiskUnmount();
}

#endif // USE_SCREENSHOT || USE_EMUSCREENSHOT	// use screen shots


