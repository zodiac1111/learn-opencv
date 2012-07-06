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


#ifndef __IMAGE_H
#define __IMAGE_H

#include <stdint.h>

#define BMP_MAGIC_0 0x42
#define BMP_MAGIC_1 0x4D
#define BMP_CREATOR1 0xADDE
#define BMP_CREATOR2 0xEFBE
#define BMP_DEFAULT_HRES 2835
#define BMP_DEFAULT_VRES 2835

typedef struct bmpfile_magic_s {
  unsigned char magic[2];
} bmpfile_magic_t;
 
typedef struct bmpfile_header_s {
  uint32_t filesz;
  uint16_t creator1;
  uint16_t creator2;
  uint32_t bmp_offset;
} bmpfile_header_t;

typedef struct bmp_dib_v3_header_s {
  uint32_t header_sz;
  uint32_t width;
  uint32_t height;
  uint16_t nplanes;
  uint16_t bitspp;
  uint32_t compress_type;
  uint32_t bmp_bytesz;
  uint32_t hres;
  uint32_t vres;
  uint32_t ncolors;
  uint32_t nimpcolors;
} bmp_dib_v3_header_t;


/* Copyright 2007 (c) Logitech. All Rights Reserved. (yuv -> rgb conversion) */
extern int convert_yuv_to_rgb_buffer(uint8_t *yuv, 
                                     uint8_t *rgb, 
                                     uint32_t width, 
                                     uint32_t height);

/* creates a bmp image from the rgb_buffer */
extern int make_bmp(uint8_t *rgb_buffer, 
                    const char *file_name,
                    uint32_t width,
                    uint32_t height);

/* creates a rgb image from the rgb buffer */
extern int make_rgb(uint8_t *rgb_buffer, 
                    const char *file_name,
                    uint32_t width,
                    uint32_t height);

#endif
