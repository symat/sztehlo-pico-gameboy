
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "../include.h"

int main()
{
	// intro screen
	DrawImgRle(IntroImg, IntroImg_Pal, 0, 0, WIDTH, HEIGHT);
	DispUpdate();
#if USE_PICOPADVGA
	WaitMs(3000);
#else
	WaitMs(1000);
#endif

	// select scene set (returns True = OK, False = Esc)
	while (SetSelect())
	{
		// select scene (returns True = OK, False = Esc)
		while (LevSelect())
		{
			NewGame(True);

			// game loop
			while (GameLoop())
			{
				// continue to next scene
				SceneInx++;
				if (SceneInx >= SceneSetNum) break;
				NewGame(False);
			}
		}
	}

	ResetToBootLoader();
}
