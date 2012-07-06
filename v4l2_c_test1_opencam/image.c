/*  The image handling.
 *
 *      This file is part of the camshot utility.
 *
 *  Copyright (c) 2010 Gabriel Zabusek <gabriel.zabusek@gmail.com>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      3 of the License, or (at your option) any later version.
 */

#include <stdio.h>

#include "image.h"
//#include "arguments.h"

static int convert_yuv_to_rgb_pixel(int y, int u, int v);

/* Copyright 2007 (c) Logitech. All Rights Reserved. (yuv -> rgb conversion) */
int convert_yuv_to_rgb_pixel(int y, int u, int v)
{
	unsigned int pixel32 = 0;
	unsigned char *pixel = (unsigned char *)&pixel32;
	int r, g, b;

	r = y + (1.370705 * (v-128));
	g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
	b = y + (1.732446 * (u-128));

	if(r > 255) r = 255;
	if(g > 255) g = 255;
	if(b > 255) b = 255;
	if(r < 0) r = 0;
	if(g < 0) g = 0;
	if(b < 0) b = 0;

	pixel[0] = r * 220 / 256;
	pixel[1] = g * 220 / 256;
	pixel[2] = b * 220 / 256;

	return pixel32;
}


int convert_yuv_to_rgb_buffer(unsigned char *yuv, 
                              unsigned char *rgb, 
                              unsigned int width, 
                              unsigned int height)
{
	unsigned int in, out = 0;
	unsigned int pixel_16;
	unsigned char pixel_24[3];
	unsigned int pixel32;
	int y0, u, y1, v;

	for(in = 0; in < width * height * 2; in += 4) {
		pixel_16 =
			yuv[in + 3] << 24 |
			yuv[in + 2] << 16 |
			yuv[in + 1] <<  8 |
			yuv[in + 0];

		y0 = (pixel_16 & 0x000000ff);
		u  = (pixel_16 & 0x0000ff00) >>  8;
		y1 = (pixel_16 & 0x00ff0000) >> 16;
		v  = (pixel_16 & 0xff000000) >> 24;

		pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
		pixel_24[0] = (pixel32 & 0x000000ff);
		pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
		pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

		rgb[out++] = pixel_24[0];
		rgb[out++] = pixel_24[1];
		rgb[out++] = pixel_24[2];

		pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
		pixel_24[0] = (pixel32 & 0x000000ff);
		pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
		pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

		rgb[out++] = pixel_24[0];
		rgb[out++] = pixel_24[1];
		rgb[out++] = pixel_24[2];
	}

	return 0;
}




