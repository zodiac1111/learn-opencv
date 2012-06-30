/*
	由一系列图片创建视频
	*/
#include <opencv2/highgui/highgui_c.h>//包含opencv库头文件
#include <opencv2/imgproc/imgproc_c.h>//图像处理 包括cvResize() 函数 同时链接库lopencv_imgproc
#include <opencv2/core/core_c.h>
#include <stdio.h> //sprint

int main()
{
	CvSize size=cvSize(128,128); //指定视频文件大小
	double fps=3;//每秒3帧.方便观看
	//图片绝对路径 注意%d
	const char *pics=
			"/home/zodiac1111/arm/learn-opencv/makeAviFileFromPic/pic/led_%d.png";
	const char *avi_out="/home/zodiac1111/arm/learn-opencv/makeAviFileFromPic/out.avi";
	CvVideoWriter *writer=cvCreateVideoWriter(avi_out
						  ,CV_FOURCC('X', 'V', 'I', 'D') //解码器独立于opencv
						  ,fps
						  ,size
						  ,1);
	int picindex=0;
	char filename[256];
	sprintf(filename,pics,picindex);
	IplImage *src=cvLoadImage(filename);
	if(src==NULL){
		return -1;
	}
	IplImage *src_resize=cvCreateImage(size,8,3); //因为图片不一定与视频文件一样大,所以缩放图片
	cvNamedWindow("makeavi",CV_WINDOW_AUTOSIZE);
	while(src!=NULL){
		cvShowImage("makeavi",src_resize);//显示可选,仅为了能看到过程
		cvWaitKey(100); //等待久点方便观看
		cvResize(src,src_resize,CV_INTER_NN);//缩放图片
		cvWriteFrame(writer,src_resize);//将缩放后的图片写入avi
		//写完一帧 释放一帧
		cvReleaseImage(&src); //释放源图片
		picindex++;//下一张图片
		sprintf(filename,pics,picindex);
		src=cvLoadImage(filename);//加载下一张源图片

	}
	cvReleaseVideoWriter(&writer);// 释放
	cvReleaseImage(&src_resize);//释放缩放用的框架
	return 0;
}

