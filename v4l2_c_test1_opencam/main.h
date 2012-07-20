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
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <assert.h>
#include <linux/videodev2.h>
//#include <jpeglib.h>
#include "type.h"
void close_camer_device(int fd);
#endif // MAIN_H
