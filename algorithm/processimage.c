#include "processimage.h"
#include <assert.h>

#define LIMIT(x) ((x)>0xffffff?0xff: ((x)<=0xffff?0:((x)>>16)))

static inline void
v4l_copy_422_block (int yTL, int yTR, int u, int v, 
	int rowPixels, unsigned char * rgb, int bits)
{
	const int rvScale = 91881;
	const int guScale = -22553;
	const int gvScale = -46801;
	const int buScale = 116129;
	const int yScale  = 65536;
	int r, g, b;

	g = guScale * u + gvScale * v;
	r = rvScale * v;
	b = buScale * u;

	yTL *= yScale; yTR *= yScale;

	if (bits == 24) {
		/* Write out top two pixels */
		rgb[0] = LIMIT(b+yTL); rgb[1] = LIMIT(g+yTL); rgb[2] = LIMIT(r+yTL);
		rgb[3] = LIMIT(b+yTR); rgb[4] = LIMIT(g+yTR); rgb[5] = LIMIT(r+yTR);

	} else if (bits == 16) {
		/* Write out top two pixels */
		rgb[0] = ((LIMIT(b+yTL) >> 3) & 0x1F) | ((LIMIT(g+yTL) << 3) & 0xE0);
		rgb[1] = ((LIMIT(g+yTL) >> 5) & 0x07) | (LIMIT(r+yTL) & 0xF8);

		rgb[2] = ((LIMIT(b+yTR) >> 3) & 0x1F) | ((LIMIT(g+yTR) << 3) & 0xE0);
		rgb[3] = ((LIMIT(g+yTR) >> 5) & 0x07) | (LIMIT(r+yTR) & 0xF8);
	}
}

int
v4l_yuv422p2rgb (unsigned char *rgb_out, unsigned char *yuv_in,
		int width, int height, int bits)
{
	const int numpix = width * height;
	const unsigned int bytes = bits >> 3;
	int h, w, y00, y01, u, v;
	unsigned char *pY = yuv_in;
	unsigned char *pOut = rgb_out;

	assert(rgb_out && yuv_in);

	for (h = 0; h < height; h += 1) {
		for (w = 0; w < width*2; w += 4) {
			y00 = *(pY++);
			u = *(pY++) - 128;
			y01 = *(pY++);
			v  = *(pY++) - 128;

			v4l_copy_422_block (y00, y01, u, v, width, pOut, bits);
			pOut += bytes << 1;
		}
	}
	return 0;
}
