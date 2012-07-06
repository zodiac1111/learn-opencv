#ifndef JPG_H
#define JPG_H
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <jpeglib.h>
#include <errno.h>
GLOBAL(void)
write_JPEG_file (JSAMPLE * image_buffer
		 ,int image_width,int image_height
		 ,char * filename, int quality);
#endif // JPG_H
