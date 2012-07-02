//#include <iostream>
#include <opencv2/highgui/highgui_c.h>//包含opencv库头文件
#include <opencv2/core/core_c.h>
#include <stdio.h>
CvCapture *capture   = NULL;// 视频获取结构， 用来作为视频获取函数的一个参数
int bar_pos=0;//滚动条位置
int curFrame=0; //指示当前播放的帧
//回调函数
void onTrackBarChange(int pos)
{
	//设置视屏当前播放的帧
	cvSetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES,pos);
	curFrame=pos;
}

int main( void) {

	IplImage* frame;//申请IplImage类型指针，就是申请内存空间来存放每一帧图像
//	const char *videofile=\
//			"/home/zodiac1111/arm/learn-opencv/"
//			"readavifile/2012-06-30-165111.webm";
	const char *videofile=	"/home/zodiac1111/Downloads/S7N4Q65CQ.flv";
	char key=0;//按键
	int frames =0;

	//创建窗口，（名字，默认大小）
	cvNamedWindow( "控制视频文件例子", CV_WINDOW_AUTOSIZE );
	capture=cvCreateFileCapture(videofile);
	//获取CV_CAP_PROP_FRAME_COUNT 视屏文件中帧的总数
	//frame = cvQueryFrame( capture );//需要读取一次才能正确读取帧数
	frame = cvQueryFrame( capture );
	frames =  cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_COUNT);
	//TODO 帧数读取错误
	frames++;
	if(true){ //创建一个滚动条
		cvCreateTrackbar("pos" //滚动条名称
				 ,"控制视频文件例子" //父窗口名称
				 ,&bar_pos //滚动条当前值指针
				 ,100 //滚动条最大值
				 ,onTrackBarChange); //回调函数
	}

	for(;;) {
		frame = cvQueryFrame( capture );// 从摄像头中抓取并返回每一帧
		if( !frame ) //错误.或者视频读完返回NULL,解码器错误也会返回NULL
			break;
		//在窗口上显示每一帧
		//1显示窗口 , 2图像
		cvShowImage( "控制视频文件例子",frame );


		char key = cvWaitKey(1000/44);//延时，每秒钟约33帧；符合人眼观看速度；
		if( key == 27 ) {
			break;//由于是死循环，而且没有控制台，当按下键盘ESC键，退出循环；
		}
		cvSetTrackbarPos("pos","控制视频文件例子",curFrame);
		 curFrame++;
	}

	cvReleaseCapture( &capture );//释放内存,一帧图像的大小
	cvDestroyWindow( "控制视频文件例子" );//销毁窗口
}
