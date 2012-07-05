/*
  从摄像头打开视频
*/

#include <opencv2/highgui/highgui_c.h>//包含opencv库头文件
#include <opencv2/core/core_c.h>
#define OPEN_FROM_CAM 1
int main( int argc, char** argv ) {//主函数
	CvCapture *capture   = NULL;// 视频获取结构， 用来作为视频获取函数的一个参数
	IplImage* frame=NULL;//申请IplImage类型指针，就是申请内存空间来存放每一帧图像
	//创建窗口，（名字，默认大小）
	cvNamedWindow( "opencv测试主控制窗口", CV_WINDOW_AUTOSIZE );
	//打开摄像头，从摄像头中获取视频
#ifdef OPEN_FROM_CAM
	capture = cvCreateCameraCapture(0);
#else
	//rtsp
	capture = cvCreateCameraCapture("rtsp://127.0.0.1:/1");
#endif
	for(;;) {
		frame = cvQueryFrame( capture );// 从摄像头中抓取并返回每一帧
		if( !frame ) break;
		cvShowImage( "opencv测试主控制窗口", frame );//在窗口上显示每一帧
		char c = cvWaitKey(1000/44);//延时，每秒钟约33帧；符合人眼观看速度；
		if( c == 27 ) {
			break;//由于是死循环，而且没有控制台，当按下键盘ESC键，退出循环；
		}
	}
	cvReleaseCapture( &capture );//释放内存；
	cvDestroyWindow( "opencv测试主控制窗口" );//销毁窗口
}
