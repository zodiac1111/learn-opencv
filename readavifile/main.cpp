/*

  从视频文件读取视频 显示出来;不包含任何控制

 120704新增:网路读取文件视频文件.
 显示在窗口 并保存到本地.
  */
#include <opencv2/highgui/highgui_c.h>//包含opencv库头文件
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc_c.h>//图像处理 包括cvResize() 函数 同时链接库lopencv_imgproc
#include <stdio.h> //sprint
#include <liveMedia/WAVAudioFileSource.hh>
int main( void) {
	int ret=0;
	CvCapture *capture   = NULL;// 视频获取结构， 用来作为视频获取函数的一个参数
	IplImage* frame;//申请IplImage类型指针，就是申请内存空间来存放每一帧图像
	//读取本地视频文件
	const char *videofile=\
			"/home/zodiac1111/arm/learn-opencv/"
			"readavifile/2012-07-04-145533.webm";
	char key;//按键
	CvSize size=cvSize(640,480); //指定视频文件大小
	double fps=25;//保存视频时 帧频
	//保存输出文件
	const char *avi_out="/home/zodiac1111/arm/learn-opencv/readavifile/out.avi";
	//保存视频编码
	CvVideoWriter *writer=cvCreateVideoWriter(avi_out
						  //解码器独立于opencv
						  ,CV_FOURCC('X', 'V', 'I', 'D') //
						  // ,CV_FOURCC('D', 'I', 'V', 'X')
						   //,CV_FOURCC('X', '2', '6', '4') //h264的一种实现
						   //,CV_FOURCC('Y', 'U', 'V', '9')
						  ,fps
						  ,size
						  ,1);
	//创建窗口，（名字，默认大小）
	cvNamedWindow( "read_video_from_net_locfile_cam_and_save", CV_WINDOW_AUTOSIZE );
	//capture=cvCreateFileCapture(videofile); //
	const char* rtsp_addr="rtsp://10.10.10.174:8554/1.webm";//网络上的文件地址
	//从网络文件读取.
	capture=cvCreateFileCapture(videofile);
	if(capture==NULL)
	{
		return -2;
	}
	IplImage *src_resize=cvCreateImage(size,8,3); //因为图片不一定与视频文件一样大,所以缩放图片
	for(;;) {
		frame = cvQueryFrame( capture );// 从摄像头中抓取并返回每一帧
		if( !frame ){//错误.或者视频读完返回NULL,解码器错误也会返回NULL
			break;
		}
		//在窗口上显示每一帧
		cvShowImage( "read_video_from_net_locfile_cam_and_save", frame );
		cvResize(frame,src_resize,CV_INTER_NN);//缩放图片
		ret=cvWriteFrame(writer,src_resize);//将缩放//cvNamedWindow("makeavi",CV_WINDOW_AUTOSIZE);
		//后的图片写入avi
		char key = cvWaitKey(1000/44);//延时，每秒钟约33帧；符合人眼观看速度；
		if( key == 27 ) {
			break;//由于是死循环，而且没有控制台，当按下键盘ESC键，退出循环；
		}
	}
	//保存视频文件
	cvReleaseVideoWriter(&writer);// 写入完成释放
	cvReleaseImage(&src_resize);//释放缩放用的框架
	//
	cvReleaseCapture( &capture );//释放内存,一帧图像的大小
	cvDestroyWindow( "read_video_from_net_locfile_cam_and_save" );//销毁窗口
	return 0;
}
