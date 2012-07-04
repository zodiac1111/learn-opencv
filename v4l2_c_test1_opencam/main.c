#include <stdio.h>
#include<linux/videodev2.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
int main(void)
{
	//打开摄像头
	int fd=open("/dev/video0",O_RDWR);
	int ret;
	struct v4l2_capability s;
	int ioctl(int fd, int request, struct v4l2_capability *argp);
	close(fd);
	return 0;
}

