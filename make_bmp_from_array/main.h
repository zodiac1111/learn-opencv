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
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
//0~1 字节 幻数
u8 MagicNumber_ascii[2]={'B','M'}; //0~1
u8 MagicNumber_hex[2]={0x42,0x4d};  //0~1

struct bmp_file_head
{ //一共12byte
	//     u16	bfType; //0~1
	u32	bfSize;//2~5
	u16	bfReserved1;//6~7
	u16	bfReserved2;//8~9
	u32	bfOffBits;//10~13
	//__attribute__ ((packed))取消字节对齐优化,文件头需要严格安顺序排列.我自己来
}__attribute__ ((packed));
struct bmp_info_head
{
	u32	biSize;//14~17
	u32	biWidth;//18~21
	u32	biHeight;//22~25
	u16	biPlanes;//26 27
	u16	biBitCount; //28 29
	u32	biCompression; //30 ~33
	u32	biSizeImage;//34~37
	u32	biXPelsPerMerer;//38~41
	u32	biYPelsPerMerer; //42 ~45
	u32	biClrUsed; //46 ~49
	u32	biClrImportant; //50~53
}__attribute__ ((packed));
void write_file_head(FILE**fp);
#endif // MAIN_H
