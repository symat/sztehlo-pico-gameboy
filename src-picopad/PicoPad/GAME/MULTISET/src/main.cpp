
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "../include.h"

#include "../img/battery.cpp"
#include "../img/menu.cpp"

// Icon vertical 60 pixels:
//  2 frame
//  1 space
//  45 icon internal
//  1 space
//  8 text
//  1 space
//  2 frame

// Icon horizontal 80 pixels:
//  2 frame
//  1 space
//  74 icon internal
//  1 space
//  2 frame

#define MENUW	4	// menu width in number of icons
#define MENUH	4	// menu height in number of icons
#define MENUNUM (MENUW*MENUH) // menu total entries
#define ICONW	80	// icon width (icon internal width = 74)
#define ICONH	60	// icon height (icon internal height = 45)
#define TEXTY	49	// text Y coordinate relative to icon

// format: compressed 8-bit paletted pixel graphics
// image width: 320 pixels
// image height: 240 lines
// image pitch: 320 bytes
//extern const u16 MenuImg_Pal[256] __attribute__ ((aligned(4)));
//extern const u8 MenuImg[28557] __attribute__ ((aligned(4)));


// format: compressed 8-bit paletted pixel graphics
// image width: 128 pixels
// image height: 240 lines
// image pitch: 128 bytes
//extern const u16 BatteryImg_Pal[16] __attribute__ ((aligned(4)));
//extern const u8 BatteryImg[4197] __attribute__ ((aligned(4)));

// color
const u16 BatCol[8] = {
	COL_BLACK,		// 0
	COL_RED,		// 1
	COLOR(255, 120, 0),	// 2
	COLOR(255, 120, 0),	// 3
	COLOR(255, 240, 0),	// 4
	COLOR(255, 240, 0),	// 5
	COLOR(0, 240, 0),	// 6
	COLOR(0, 240, 0),	// 7
};

void Battery()
{
	int i, j;

	DrawClear();

	pText txt;
	TextInit(&txt);	

	// set font
	pDrawFont = FontBold8x16; // font 8x8
	DrawFontHeight = 16; // font height

	// draw background
	DrawImgRle(BatteryImg, BatteryImg_Pal, 0, 0, 128, 240);

	int k = 0;

	while (True)
	{
		if (KeyGet() != NOKEY) return;

		// wait for VSync
		VgaWaitVSync();

		// draw battery
		float bat = GetBat();

#if USE_CONFIG			// use device configuration (lib_config.c, lib_config.h)
		i = (int)((bat - ConfigGetBatEmpty())/(ConfigGetBatFull()-ConfigGetBatEmpty())*8);
#else
		i = (int)((bat - BATTERY_EMPTY)/(BATTERY_FULL-BATTERY_EMPTY)*8);
#endif

		if (i < 0) i = 0;
		if (i > 7) i = 7;

		if (i < 7)
		{
			j = 6;
			for (; j < i-1; j--) DrawRect(25, 198 - j*26, 77, 19, COL_BLACK);
		}
		u16 col = BatCol[i];
		if (i > 0)
		{
			for (i--; i >= 0; i--) DrawRect(25, 198 - i*26, 77, 19, col);
		}

#define DISPX	128
#define DISPW	(WIDTH-128)
#define DY	20
		int y = 30;

		// title
		DrawText2("PicoPad", DISPX + (DISPW-7*16)/2, y, COL_YELLOW);
		y += 50;

		// SDK version
		TextPrint(&txt, "SDK version %.2f", SDK_VER*0.01f);
		DrawTextBg(TextPtr(&txt), DISPX + (DISPW - TextLen(&txt)*8)/2, y, COL_WHITE, COL_BLACK);
		y += DY;

		// CPU
#if RP2040
		TextPrint(&txt, "CPU: RP2040-B%d", RomGetVersion() - 1);
#elif RISCV
		TextPrint(&txt, "CPU: RP2350-RISCV-A%d", RomGetVersion());
#else
		TextPrint(&txt, "CPU: RP2350-ARM-A%d", RomGetVersion());
#endif
		DrawTextBg(TextPtr(&txt), DISPX + (DISPW - TextLen(&txt)*8)/2, y, COL_WHITE, COL_BLACK);
		y += DY;

		// flash size
		TextPrint(&txt, "Flash size %d MB", FlashSize/(1024*1024));
		DrawTextBg(TextPtr(&txt), DISPX + (DISPW - TextLen(&txt)*8)/2, y, COL_WHITE, COL_BLACK);
		y += DY;

		// flash ID
		TextPrint(&txt, "ID %s", FlashIDText);
		DrawTextBg(TextPtr(&txt), DISPX + (DISPW - TextLen(&txt)*8)/2, y, COL_WHITE, COL_BLACK);
		y += DY;

		// temperature
		TextPrint(&txt, " temperature %.1f`C ", ADC_Temp());
		DrawTextBg(TextPtr(&txt), DISPX + (DISPW - TextLen(&txt)*8)/2, y, COL_WHITE, COL_BLACK);
		y += DY;

		// battery
		TextPrint(&txt, " battery %.2fV ", bat);
		DrawTextBg(TextPtr(&txt), DISPX + (DISPW - TextLen(&txt)*8)/2, y, COL_WHITE, COL_BLACK);
		y += DY;

		// frequency
		TextPrint(&txt, " frequency %dMHz ", (ClockGetHz(CLK_SYS)+500000)/1000000);
		DrawTextBg(TextPtr(&txt), DISPX + (DISPW - TextLen(&txt)*8)/2, y, COL_WHITE, COL_BLACK);
		y += DY;

		// redraw display
		DispUpdate();

		WaitMs(100);
	}
}

const char* const Titles[MENUNUM] = {
	"Ants",
	"Atoms",
	"Eggs",
	"Fifteen",
	"Flappy",
	"GhostRace",
	"Chess",
	"Life",
	"Maze",
	"PacMan",
	"Raptor",
	"Sokoban",
	"Tetris",
	"Train",
	"TV Tennis",
	"VegaSlot",
//	"Battery",
};

typedef int (*pMenuFnc)();

const pMenuFnc MenuFnc[MENUNUM] = {
	namespace_ants::main,
	namespace_atoms::main,
	namespace_eggs::main,
	namespace_fifteen::main,
	namespace_flappy::main,
	namespace_ghostracing::main,
	namespace_chess::main,
	namespace_life::main,
	namespace_maze::main,
	namespace_pacman::main,
	namespace_raptor::main,
	namespace_sokoban::main,
	namespace_tetris::main,
	namespace_train::main,
	namespace_tvtennis::main,
	namespace_vegaslot::main,
//	Battery,
};

// selected game
int GameSel = 0;

int main()
{
	int i, x, y, strip;

	// main loop
	while (True)
	{
		// set font
		SelFont8x8();

		// wait for VSync
		//VgaWaitVSync();

		for (strip = DISP_STRIP_NUM; strip > 0; strip--)
		{
			// next strip
			DispSetStripNext();

			// draw menu image
			DrawImgRle(MenuImg, MenuImg_Pal, 0, 0, WIDTH, HEIGHT);

			// draw text	
			for (i = 0; i < MENUNUM; i++)
			{
				y = (i/MENUW)*ICONH + TEXTY;
				x = (i % MENUW)*ICONW;
				x += (ICONW - StrLen(Titles[i])*8)/2;
				DrawText(Titles[i], x, y, COL_WHITE);
			}

			// draw selection frame
			y = (GameSel/MENUW)*ICONH;
			x = (GameSel % MENUW)*ICONW;
			DrawFrame(x, y, ICONW, ICONH, COL_RED);
			DrawFrame(x+1, y+1, ICONW-2, ICONH-2, COL_RED);
			DispUpdate();
		}

		// set off back buffers
		DispSetStripOff();

		// keyboard service
		switch (KeyGet())
		{
		case KEY_RIGHT:
			GameSel++;
			if (GameSel >= MENUNUM) GameSel = 0;
			break;

		case KEY_LEFT:
			GameSel--;
			if (GameSel < 0) GameSel = MENUNUM-1;
			break;

		case KEY_UP:
			GameSel -= MENUW;
			if (GameSel < 0) GameSel += MENUNUM;
			break;

		case KEY_DOWN:
			GameSel += MENUW;
			if (GameSel >= MENUNUM) GameSel -= MENUNUM;
			break;

		// run game
		case KEY_A:
			DrawClear();
			MenuFnc[GameSel]();
			DrawClear();
			KeyFlush();
			StopSound();
			DispSetStripOff();
			break;

		// battery
		case KEY_X:
#if USE_SCREENSHOT		// use screen shots
			ScreenShot();
#endif
			DrawClear();
			Battery();
			DrawClear();
			KeyFlush();
			break;

		case KEY_Y: ResetToBootLoader();

		// sound
		case KEY_B:
			if (GlobalSoundOff)
			{
				SelFont8x16();
				DrawTextBg2(" SOUND ON ", (WIDTH-10*16)/2, (HEIGHT-16)/2, COL_WHITE, COL_BLACK);
				GlobalSoundSetOn();
				DispUpdate();
				WaitMs(600);
				DrawClear();
				KeyFlush();
			}
			else
			{
				SelFont8x16();
				DrawTextBg2(" SOUND OFF ", (WIDTH-11*16)/2, (HEIGHT-16)/2, COL_WHITE, COL_BLACK);
				GlobalSoundSetOff();
				DispUpdate();
				WaitMs(600);
				DrawClear();
				KeyFlush();
			}
			break;
		}
	}
}
