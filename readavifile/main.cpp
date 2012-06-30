/*
  从视频文件读取视频 显示出来;不包含任何控制
  */
//using namespace std;
#include <opencv2/highgui/highgui_c.h>//包含opencv库头文件
#include <opencv2/core/core_c.h>

int main( void) {
	CvCapture *capture   = NULL;// 视频获取结构， 用来作为视频获取函数的一个参数
	IplImage* frame;//申请IplImage类型指针，就是申请内存空间来存放每一帧图像
	const char *videofile=\
			"/home/zodiac1111/arm/learn-opencv/"
			"readavifile/2012-06-30-165111.webm";
	char key=0;//按键
	//创建窗口，（名字，默认大小）
	cvNamedWindow( "读取视频文件例子", CV_WINDOW_AUTOSIZE );
	capture=cvCreateFileCapture(videofile);

	for(;;) {
		frame = cvQueryFrame( capture );// 从摄像头中抓取并返回每一帧
		if( !frame ) //错误.或者视频读完返回NULL,解码器错误也会返回NULL
			break;
		//在窗口上显示每一帧
		cvShowImage( "显示窗口(Esc退出)", frame );
		char key = cvWaitKey(1000/44);//延时，每秒钟约33帧；符合人眼观看速度；
		if( key == 27 ) {
			break;//由于是死循环，而且没有控制台，当按下键盘ESC键，退出循环；
		}
	}

	cvReleaseCapture( &capture );//释放内存,一帧图像的大小
	cvDestroyWindow( "读取视频文件例子" );//销毁窗口
}
