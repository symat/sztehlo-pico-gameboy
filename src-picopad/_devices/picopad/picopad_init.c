
// ****************************************************************************
//
//                              PicoPad Init
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

#if USE_PICOPAD

#include "../../_lib/inc/lib_pwmsnd.h"

#include "../../_display/st7789/st7789.h"
#include "../../_lib/inc/lib_fat.h"
#include "../../_lib/inc/lib_sd.h"
#include "picopad_bat.h"
#include "picopad_key.h"
#include "picopad_led.h"
#include "picopad_init.h"	// initialize
#include "picopad_ss.h"		// screen shot

#include "../../_sdk/inc/sdk_gpio.h"

#if USE_DISPHSTX && USE_DISPHSTX_VMODE	// 1=use HSTX Display driver

#if USE_DISPHSTX_DISPBUF	// 1 = use DispBuf + FrameBuf
FRAMETYPE ALIGNED DispBuf[FRAMESIZE];	// display buffer
#endif

#if USE_FRAMEBUF && USE_DISPHSTX_VMODE	// use videomode 0=custom, 1=320x240/16 BackBuf, ...
FRAMETYPE ALIGNED FrameBuf[FRAMESIZE];	// frame buffer
#endif // USE_FRAMEBUF

// last system time of auto update
u32 DispAutoUpdateLast;

#endif // USE_DISPHSTX && USE_DISPHSTX_VMODE

// DEBUG indicator - copy it into program checkpoint
// { GPIO_Init(LED1_PIN); GPIO_DirOut(LED1_PIN); while (1) { GPIO_Flip(LED1_PIN); io32 i; for (i = 1000000; i > 0; i--); } }

// Device init
void DeviceInit()
{
#if USE_PICOPADHSTX
	// initialize LEDs
	LedInit();
#else // USE_PICOPADHSTX
#if USE_MINIVGA					// use mini-VGA display with simple frame buffer
	// start VGA on CPU 1 (must be paired with VgaStop)
	VgaStart();
#else // USE_MINIVGA
	// initialize LEDs
	LedInit();

	// initialize display
	DispInit(DISP_ROT);
#endif // USE_MINIVGA
#endif // !USE_PICOPADHSTX

	// initilize keys
	KeyInit();

	// init battery measurement
	BatInit();

#if USE_PWMSND		// use PWM sound output; set 1.. = number of channels (lib_pwmsnd.c, lib_pwmsnd.h)
	// initialize PWM sound output (must be re-initialized after changing CLK_SYS system clock)
	PWMSndInit();
#endif

#if USE_SD		// use SD card (lib_sd.c, lib_sd.h)
	// initialize SD card interface (must be re-initialized after changing system clock)
	SDInit();
#endif

	// TODO: touch screen init (only disable the CS line for now)
	// TOUCH_CS is GPIO 14
	//GPIO_Init(14);
	//GPIO_DirOut(14);
	//GPIO_Out1(14);  
}

// Device terminate
void DeviceTerm()
{

	// terminate LEDs
	LedTerm();

	// terminate display
	DispTerm();

	// terminate keys
	KeyTerm();

	// terminate battery measurement
	BatTerm();

#if USE_PWMSND	// use PWM sound output; set 1.. = number of channels (lib_pwmsnd.c, lib_pwmsnd.h)
	// terminate PWM sound output
	PWMSndTerm();
#endif

#if USE_SD		// use SD card (lib_sd.c, lib_sd.h)
	// terminate SD card interface
	SDTerm();
#endif
}

#endif // USE_PICOPAD
