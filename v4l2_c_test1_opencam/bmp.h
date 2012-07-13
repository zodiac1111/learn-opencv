#ifndef BMP_H
#define BMP_H

#include "type.h"

u8 MagicNumber_ascii[2]={'B','M'};
u8 MagicNumber_hex[2]={0x42,0x4d};

struct bmp_file_head
 { //一共12byte
  //     u16	bfType; //0~1
       u32	bfSize;//2~5
       u16	bfReserved1;//6~7
       u16	bfReserved2;//8~9
       u32	bfOffBits;//10~13
 };
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
  };
//0~1 字节 幻数

void write_file_head(FILE**fp,int width,int hight);
#endif // BMP_H
