#include <fcntl.h> //file ctrl
#include <sys/types.h>
#include <stdio.h>
#include "bmp.h"
#define c_width 640
#define c_hight 480
/*
	写bmp位图文件 文件头 参见:维基百科
	http://en.wikipedia.org/wiki/BMP_file_format
*/
void write_file_head(FILE**fp,int width,int hight)
{
//	const int  width=width;//位图宽4字节的整数倍/不够补 00
//	const int  hight=hight;
	struct bmp_file_head head;//文件头 14byte
	struct bmp_info_head infohead; //位图头 40 byte
	//Magic number 0~1
	head.bfSize=width*hight*3+14+40 ;//文件大小 //2~5
	head.bfReserved1=0;  //6~7
	head.bfReserved2=0; //8~9
	head.bfOffBits=2+12+40;//偏置 10~13
	//bmp file info head
	infohead.biSize=40; //14~17
	infohead.biWidth=width;//18-21
	infohead.biHeight=hight;//22-25
	infohead.biPlanes=1;//26-27
	infohead.biBitCount=24;//28-29
	infohead.biCompression=0;//30-33
	infohead.biSizeImage=width*hight*3;//34-37
	infohead.biXPelsPerMerer=width;//38-41
	infohead.biYPelsPerMerer=hight;//42-45
	infohead.biClrUsed=0;//46-49
	infohead.biClrImportant=0;//50-53
	//write to file
	fwrite(MagicNumber_hex,2,1,*fp);  //magic number
	fwrite(&head,sizeof(head),1,*fp); //file head
	fwrite(&infohead,sizeof(infohead),1,*fp); //bmp info head
}
