#include <stdio.h>
#include<libv4l1.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#define VIDEO_MAXFRAME 1024
#define DEFAULT_DEVICE “/dev/video0”
struct _v4l_struct
{
	int fd;//保存打开视频文件的设备描述符
	struct video_capability capability;//该结构及下面的结构为v4l所定义可在上述头文件中找到
	struct video_picture picture;
	struct video_mmap mmap;
	struct video_mbuf mbuf;
	unsigned char *map;//用于指向图像数据的指针
	int frame_current;
	int frame_using[VIDEO_MAXFRAME];//这两个变量用于双缓冲在后面介绍。
};
typedef struct _v4l_struct v4l_device;

int v4l_get_capability(v4l_device *vd)
{
	if (ioctl(vd->fd, VIDIOCGCAP, &(vd->capability)) < 0) {
		perror("v4l_get_capability:");
		return -1;
	}
	return 0;
}
int v4l_get_picture(v4l_device *vd)
{
	if (ioctl(vd->fd, VIDIOCGPICT, &(vd->picture)) < 0) {
		perror("v4l_get_picture:");
		return -1;
	}
	return 0;
}

int v4l_open(char *dev , v4l_device *vd)
{
	if(!dev){
		//dev = “/dev/video0”;
		return -2;
	}
	if((vd->fd=open(dev,O_RDWR))<0) {
		//(“v4l_open:”);
		return -1;
	}
	if(v4l_get_capability(vd))return -1;
	if(v4l_get_picture(vd))return -1;//这两个函数就是即将要完成的获取设备信息的函数
	return 0;
}
int v4l_close(v4l_device *vd)
{
	close(vd->fd);
	return 0;
}
int v4l_grab_picture(v4l_device *vd, unsigned int size)
{
	if(read(vd->fd,&(vd->map),size)==0)
		return -1;
	else
		return 0;

}
int main(void)
{
	v4l_device *vd;
	char *dev="/dev/video0";
	v4l_open(dev,vd);
	v4l_grab_picture(vd,1024);
	printf("Hello World!\n");
	return 0;
}

