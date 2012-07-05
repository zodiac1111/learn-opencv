#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include<linux/videodev2.h> //v4l2
#include <sys/types.h>
#include <fcntl.h> //file ctrl
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h> //used by memset
#include <sys/mman.h> //used by mmap
#include <stdlib.h>
#include <assert.h> //used by assert

//void yuyv2rgb24(unsigned char  *p ,size_t n);

#endif // MAIN_H
