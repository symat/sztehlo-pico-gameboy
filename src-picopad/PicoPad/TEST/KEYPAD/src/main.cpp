
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "include.h"

#define BTNW	50	// button width
#define BTNW_SMALL  30 // width of small buttons (A, B, X, Y)
#define BTNH	34	// button height
#define FRAME	2	// frame width
#define X_OFFSET   20


void DispKey(int x, int y, const char* txt, Bool hold)
{
	int len = StrLen(txt);
	int width=BTNW;
	if(len==1) width=BTNW_SMALL;
	x -= width/2;
	y -= BTNH/2;

	// prepare color
	u16 fgcol = COL_BLACK;
	u16 bgcol = COL_WHITE;
	if (hold)
	{
		fgcol = COL_YELLOW;
		bgcol = COL_BLUE;
	}

	// draw background
	DrawRect(x, y, width, BTNH, bgcol);

	// draw frame
	DrawFrame(x+FRAME, y+FRAME, width-2*FRAME, BTNH-2*FRAME, fgcol);

	// draw text
	DrawText(txt, x + (width-len*FONTW)/2, y + (BTNH-FONTH)/2, fgcol);
}

int main()
{
	// clear screen
	DrawClearCol(COL_DKGREEN);

	// X hold counter
	u32 t = Time();
	DrawText("Hold X for 2 seconds to quit", (WIDTH-28*FONTW)/2, HEIGHT-FONTH-10, COL_YELLOW);

	while (True)
	{
		// wait for VSync
		VgaWaitVSync();
	
		// left
		DispKey(X_OFFSET + (WIDTH-2*X_OFFSET)/16*4, HEIGHT/4*2, "Left", KeyPressed(KEY_LEFT));

		// up
		DispKey(X_OFFSET + (WIDTH-2*X_OFFSET)/16*7, HEIGHT/4*1, "Up", KeyPressed(KEY_UP));

		// right
		DispKey(X_OFFSET + (WIDTH-2*X_OFFSET)/16*10, HEIGHT/4*2, "Right", KeyPressed(KEY_RIGHT));

		// down
		DispKey(X_OFFSET + (WIDTH-2*X_OFFSET)/16*7, HEIGHT/4*3, "Down", KeyPressed(KEY_DOWN));

		// A
		DispKey(X_OFFSET + (WIDTH-2*X_OFFSET)/16*15, HEIGHT/8*2, "A", KeyPressed(KEY_A));

		// B
		DispKey(X_OFFSET + (WIDTH-2*X_OFFSET)/16*14, HEIGHT/4*2, "B", KeyPressed(KEY_B));

		// X
		DispKey(X_OFFSET + (WIDTH-2*X_OFFSET)/16*7, HEIGHT/4*2, "X", KeyPressed(KEY_X));

		// Y
		DispKey(X_OFFSET + (WIDTH-2*X_OFFSET)/8*1, HEIGHT/8*2, "Y", KeyPressed(KEY_Y));

		// update display
		DispUpdate();

		// updateX  hold counter
		if (!KeyPressed(KEY_X))
			t = Time();
		else
		{
			if ((s32)(Time() - t) >= 2000000)
			{
				DrawClear();
				DispUpdate();
				ResetToBootLoader();
			}
		}

#if USE_SCREENSHOT		// use screen shots
		if (KeyGet() == KEY_Y) {
			ScreenShot();
			SmallScreenShot();
		}
#endif
	}
}
