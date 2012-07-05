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
		perror("Fail to ioctl");
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

//将采集好的数据放到文件中
int process_image(void *addr,int length)
{
	FILE *fp;
	static int num = 0;
	char picture_name[20];
	sprintf(picture_name,"picture%d.jpg",num ++);
	if((fp = fopen(picture_name,"w")) == NULL){
		perror("Fail to fopen");
		exit(EXIT_FAILURE);
	}
	fwrite(addr,length,1,fp);
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
	int count = 10;
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
