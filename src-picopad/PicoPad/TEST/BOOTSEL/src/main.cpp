
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "../include.h"

int main()
{
	// draw text
	DrawText("Press B=BOOTSEL, Y=quit...", (WIDTH-26*8)/2, (HEIGHT-8)/2, COL_WHITE);
	DispUpdate();

	// reset to boot loader
	while (True)
	{
		switch (KeyGet())
		{
		case KEY_B:
			ResetUsb(0, 0);

		case KEY_Y:
			ResetToBootLoader();

#if USE_SCREENSHOT		// use screen shots
		case KEY_X:
			ScreenShot();
			break;
#endif
		}
	}
}
