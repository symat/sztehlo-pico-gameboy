
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "../include.h"

#define WIDTH2 220
#define HEIGHT2 220
#define FRAMESIZE2 (WIDTH2*HEIGHT2)

ALIGNED u16 BackBuf[FRAMESIZE2];

// transformation matrix
sMat2D Mat;

// setup rotation transformation matrix
void SetMat(float a)
{
	// prepare transformation matrix
	// Image of real dimensions WIDTH * HEIGHT will be enlarged to square 400 * 400
//  ws ... source image width
//  hs ... source image height
//  x0 ... reference point X on source image
//  y0 ... reference point Y on source image
//  wd ... destination image width (negative = flip image in X direction)
//  hd ... destination image height (negative = flip image in Y direction)
//  shearx ... shear image in X direction
//  sheary ... shear image in Y direction
//  rot ... rotate image (angle in radians)
//  tx ... shift in X direction (ws = whole image width)
//  ty ... shift in Y direction (hs = whole image height)
	Mat2D_PrepDrawImg(&Mat, WIDTH2, HEIGHT2, WIDTH2/2, HEIGHT2/2, 400, 400, 0, 0, a, 0, 0);
}

// main function
int main()
{
	int x, y, k;
	float r, a, y2;
	u16* d;

	// setup transformation matrix
	SetMat(0);

	// generate hypno image
	d = BackBuf;
	for (y = -HEIGHT2/2; y < HEIGHT2/2; y++)
	{
		for (x = -WIDTH2/2; x < WIDTH2/2; x++)
		{
			// radius (y is converted to ensure square image)
			y2 = (float)y*WIDTH2/HEIGHT2;
			r = sqrtf(x*x + y2*y2);

			// angle
			a = atan2f(y2, x);

			// intensity (+ small raster dithering)
			k = (int)(127*sinf(r*0.17f + a) + 127);

			// store pixel, on red color
			*d++ = (k & 0xf8)<<8;
		}
	}

	// main loop
	a = 0;
	while (True)
	{
		SetMat(a);
#if USE_PICOPADHSTX		// use PicoPadHSTX device configuration
		a -= 0.1f;
#else
		a -= 0.3f;
#endif
		if (a < 0) a += PI*2;

		// draw image with transformation
		DrawImgMat(BackBuf, WIDTH2, HEIGHT2, 0, 0, WIDTH, HEIGHT, &Mat, DRAWIMG_CLAMP, 0); 

		// update display
		DispUpdateAll();

		switch (KeyGet())
		{
		// reset to boot loader
		case KEY_Y: ResetToBootLoader();

#if USE_SCREENSHOT		// use screen shots
		case KEY_X:
			ScreenShot();
			break;
#endif
		}
	}
}
