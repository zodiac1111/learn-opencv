/*
	企图用v4l直接调用摄像头获得数据
	实现功能:
		采集数据 格式YUYV
	未实现:
		1 YUYV->RGB888 /24
		2 显示图片等等.
*/
#include "main.h"
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
#define c_width 640
#define c_hight 480
//#include <opencv/cvwimage.h>
//#include <opencv/cxcore.h>
//#include <opencv/highgui.h>
typedef struct
{
	void *start;
	int length;
}BUFTYPE;
BUFTYPE *user_buf;
int n_buffer = 0;
//打开摄像头设备
int open_camer_device()
{
	int fd;
	if((fd = open("/dev/video0",O_RDWR | O_NONBLOCK)) < 0){
		perror("Fail to open");
		exit(EXIT_FAILURE);
	}
	return fd;
}

int init_mmap(int fd)
{
	int i = 0;
	struct v4l2_requestbuffers reqbuf;
	bzero(&reqbuf,sizeof(reqbuf));
	reqbuf.count = 4;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	//申请视频缓冲区(这个缓冲区位于内核空间，需要通过mmap映射)
	//这一步操作可能会修改reqbuf.count的值，修改为实际成功申请缓冲区个数
	if(-1 == ioctl(fd,VIDIOC_REQBUFS,&reqbuf)){
		perror("Fail to ioctl 'VIDIOC_REQBUFS'");
		exit(EXIT_FAILURE);
	}
	n_buffer = reqbuf.count;
	printf("n_buffer = %d\n",n_buffer);
	user_buf = calloc(reqbuf.count,sizeof(*user_buf));
	if(user_buf == NULL){
		fprintf(stderr,"Out of memory\n");
		exit(EXIT_FAILURE);
	}
	//将内核缓冲区映射到用户进程空间
	for(i = 0; i < reqbuf.count; i ++)
	{
		struct v4l2_buffer buf;
		bzero(&buf,sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		//查询申请到内核缓冲区的信息
		if(-1 == ioctl(fd,VIDIOC_QUERYBUF,&buf)){
			perror("Fail to ioctl : VIDIOC_QUERYBUF");
			exit(EXIT_FAILURE);
		}
		user_buf[i].length = buf.length;
		user_buf[i].start =
				mmap(
					NULL,/*start anywhere*/
					buf.length,
					PROT_READ | PROT_WRITE,
					MAP_SHARED,
					fd,buf.m.offset
					);
		if(MAP_FAILED == user_buf[i].start){
			perror("Fail to mmap");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}

//初始化视频设备
int init_camer_device(int fd)
{
	struct v4l2_fmtdesc fmt;
	struct v4l2_capability cap;
	struct v4l2_format stream_fmt;
	int ret;
	//当前视频设备支持的视频格式
	memset(&fmt,0,sizeof(fmt));
	fmt.index = 0;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	while((ret = ioctl(fd,VIDIOC_ENUM_FMT,&fmt)) == 0){
		fmt.index ++ ;
		printf("{pixelformat = %c%c%c%c},description = '%s'\n",
		       fmt.pixelformat & 0xff,(fmt.pixelformat >> 8)&0xff,
		       (fmt.pixelformat >> 16) & 0xff,(fmt.pixelformat >> 24)&0xff,
		       fmt.description);
	}
	//查询视频设备驱动的功能
	ret = ioctl(fd,VIDIOC_QUERYCAP,&cap);
	if(ret < 0){
		perror("FAIL to ioctl VIDIOC_QUERYCAP");
		exit(EXIT_FAILURE);
	}
	//判断是否是一个视频捕捉设备
	if(!(cap.capabilities & V4L2_BUF_TYPE_VIDEO_CAPTURE)){
		printf("The Current device is not a video capture device\n");
		exit(EXIT_FAILURE);
	}
	//判断是否支持视频流形式
	if(!(cap.capabilities & V4L2_CAP_STREAMING)){
		printf("The Current device does not support streaming i/o\n");
		exit(EXIT_FAILURE);
	}
	//设置摄像头采集数据格式，如设置采集数据的
	//长,宽，图像格式(JPEG,YUYV,MJPEG等格式)
	stream_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	stream_fmt.fmt.pix.width = 680;
	stream_fmt.fmt.pix.height = 480;
	stream_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	stream_fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
	if(-1 == ioctl(fd,VIDIOC_S_FMT,&stream_fmt)){
		perror("Fail to ioctl:VIDIOC_S_FMT ");
		exit(EXIT_FAILURE);
	}
	//初始化视频采集方式(mmap)
	init_mmap(fd);
	return 0;
}

int start_capturing(int fd)
{
	unsigned int i;
	enum v4l2_buf_type type;
	//将申请的内核缓冲区放入一个队列中
	for(i = 0;i < n_buffer;i ++){
		struct v4l2_buffer buf;
		bzero(&buf,sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		if(-1 == ioctl(fd,VIDIOC_QBUF,&buf)){
			perror("Fail to ioctl 'VIDIOC_QBUF'");
			exit(EXIT_FAILURE);
		}
	}
	//开始采集数据
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(-1 == ioctl(fd,VIDIOC_STREAMON,&type)){
		printf("i = %d.\n",i);
		perror("Fail to ioctl 'VIDIOC_STREAMON'");
		exit(EXIT_FAILURE);
	}
	return 0;
}

//
void yuyv2rgb( int Y,int U,int V
	       ,u8 *B,u8 *G ,u8 *R)
{
	//u8 r ,g,b;
		*R = Y + 1.14*(V-128);
		*G = Y - 0.39*(U-128) - 0.58*(V-128);
		*B = Y + 2.03*(U-128);
// other way yuv2rgb888 <backup>
//	int r, g, b;
//	r = Y + (1.370705 * (V-128));
//	g = Y - (0.698001 * (V-128)) - (0.337633 * (U-128));
//	b = Y + (1.732446 * (U-128));

//	if(r > 255) r = 255;
//	if(g > 255) g = 255;
//	if(b > 255) b = 255;
//	if(r < 0) r = 0;
//	if(g < 0) g = 0;
//	if(b < 0) b = 0;
//	*R=r;
//	*G=g;
//	*B=b;
	//printf("rgb %x %x %x",*R,*G,*B);
}


//将采集好的数据放到文件中
int process_image(void *addr,int length)
{
	FILE *fp;
	static int num = 0;
	char picture_name[20];
	sprintf(picture_name,"picture%d.bmp",num ++);
	if((fp = fopen(picture_name,"w")) == NULL){
		perror("Fail to fopen");
		exit(EXIT_FAILURE);
	}
#define CHANGE_PIC_FORMAT  //转换图像格式YUYV RGB888
#ifdef CHANGE_PIC_FORMAT
	//每次读取4字节(2像素)的YUYV格式:Y0 U0 Y1 V0
	//写入6字节(2像素) BGR BGR
	u8 s[640*480*3]; int i=0;int j=0;int k=0;
	u8 y1,u,y2,v; //依次读取4字节/2像素
	//printf("size of head=%d",sizeof(head));//
	write_file_head(&fp); //文件头

	for(i=c_hight-1;i>=0;i--) //行 从最 底行->顶行 bottom -> top
		for(j=0;j<c_width*2;){ //列 2byte/pix lift -> right
			y1=*(int*)(addr+i*c_width*2+j+0);
			u=*(int*)(addr+i*c_width*2+j+1);
			y2=*(int*)(addr+i*c_width*2+j+2);
			v=*(int*)(addr+i*c_width*2+j+3);
			j+=4;//source :move to next 2 pixs (4byte)
			yuyv2rgb(y1,u,v	,&s[k+0],&s[k+1],&s[k+2]);
			yuyv2rgb(y1,u,v	,&s[k+3],&s[k+4],&s[k+5]);
			k+=6;//detct :move to next 2 pixs (6byte)
		}
	if(fwrite(s,sizeof(s),1,fp)<=0){
		perror("write data ");
	}
#else
	fwrite(addr,length,1,fp);
#endif
	usleep(500);
	fclose(fp);
	return 0;
}

int read_frame(int fd)
{

	struct v4l2_buffer buf;
	unsigned int i;
	bzero(&buf,sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	//从队列中取缓冲区
	if(-1 == ioctl(fd,VIDIOC_DQBUF,&buf)){
		perror("Fail to ioctl 'VIDIOC_DQBUF'");
		exit(EXIT_FAILURE);
	}
	assert(buf.index < n_buffer);
	//读取进程空间的数据到一个文件中
	process_image(user_buf[buf.index].start,user_buf[buf.index].length);
	if(-1 == ioctl(fd,VIDIOC_QBUF,&buf)){
		perror("Fail to ioctl 'VIDIOC_QBUF'");
		exit(EXIT_FAILURE);
	}
	return 1;
}
//主循环
int mainloop(int fd)
{
	int count = 30;
	while(count -- > 0){
		for(;;){
			fd_set fds;
			struct timeval tv;
			int r;
			FD_ZERO(&fds);
			FD_SET(fd,&fds);
			/*Timeout*/
			tv.tv_sec = 2;
			tv.tv_usec = 0;
			r = select(fd + 1,&fds,NULL,NULL,&tv);
			if(-1 == r){
				if(EINTR == errno)
					continue;
				perror("Fail to select");
				exit(EXIT_FAILURE);
			}
			if(0 == r){
				fprintf(stderr,"select Timeout\n");
				exit(EXIT_FAILURE);
			}
			if(read_frame(fd))
				break;
		}
	}
	return 0;
}

void stop_capturing(int fd)
{
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(-1 == ioctl(fd,VIDIOC_STREAMOFF,&type)){
		perror("Fail to ioctl 'VIDIOC_STREAMOFF'");
		exit(EXIT_FAILURE);
	}
	return;
}

void uninit_camer_device()
{
	unsigned int i;
	for(i = 0;i < n_buffer;i ++){
		if(-1 == munmap(user_buf[i].start,user_buf[i].length)){
			exit(EXIT_FAILURE);
		}
	}
	free(user_buf);
	return;
}

void close_camer_device(int fd)
{
	if(-1 == close(fd)){
		perror("Fail to close fd");
		exit(EXIT_FAILURE);
	}
	return;
}

int main()
{
	int fd;
	fd = open_camer_device();
	init_camer_device(fd);
	start_capturing(fd);
	mainloop(fd);
	stop_capturing(fd);
	uninit_camer_device(fd);
	close_camer_device(fd);
	return 0;
}

/*
	bmp位图文件 文件头 参见:维基百科
	http://en.wikipedia.org/wiki/BMP_file_format
*/
void write_file_head(FILE**fp)
{
	const int  width=c_width;//位图宽4字节的整数倍/不够补 00
	const int  hight=c_hight;
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

	//写入头文件
	fwrite(MagicNumber_hex,2,1,*fp);  //magic number
	fwrite(&head,sizeof(head),1,*fp); //file head
	fwrite(&infohead,sizeof(infohead),1,*fp); //bmp info head
}
