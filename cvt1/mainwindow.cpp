#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cv.h"
#include "highgui.h"
#include "QImage"
class QCvImage{

private:
	IplImage* cvBGRImg;
	IplImage* cvRGBImg;

public:
	QCvImage()
	{

	}

	QCvImage(char* imgPath)
	{
		cvBGRImg = cvLoadImage(imgPath, true);
		cvRGBImg = NULL;
	}

	QImage* getQImage(void)
	{
		if(cvBGRImg == NULL)
			return NULL;

		cvRGBImg = cvCloneImage(cvBGRImg);
		cvCvtColor(cvBGRImg, cvRGBImg, CV_BGR2RGB);

		uchar* imgData = (uchar*)(cvRGBImg->imageData);
		QImage* qimg = new QImage(imgData, cvRGBImg->width, cvRGBImg->height, QImage::Format_RGB888);

		return qimg;
	}

	~QCvImage()
	{
		cvReleaseImage(&cvBGRImg);
		cvReleaseImage(&cvRGBImg);
	}

};
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	IplImage* img = 0;
	IplImage* img2 = 0;
	IplImage* img3 = 0;
	int height,width,step,channels;
	uchar *data;
	uchar *data2;
	uchar *data3;
	int i,j,k;
	//	  if(argc<2){
	//	    printf("Usage: main <image-file-name>\n\7");
	//	    exit(0);
	//	  }

	// load an image
	//img=cvLoadImage(argv[1]);
	img=cvLoadImage("/home/zodiac1111/Pictures/1.png"); //红
	img2=cvLoadImage("/home/zodiac1111/Pictures/2.png"); //青
	img3=cvLoadImage("/home/zodiac1111/Pictures/1.png"); //合并图片
	if(!img){
		printf("Could not load image file: XXX\n");
		exit(0);
	}

	// get the image data
	height    = img->height;
	width     = img->width;
	step      = img->widthStep;
	channels  = img->nChannels;
	data      = (uchar *)img->imageData;
	data2      = (uchar *)img2->imageData;
	data3      = (uchar *)img3->imageData;
	printf("中文Processing a %dx%d image with %d channels\n",height,width,channels);
	//printf("%s",data); //图片数据

	// create a window
	//cvNamedWindow("mainWin", CV_WINDOW_AUTOSIZE);
	//cvMoveWindow("mainWin", 100, 100);

	// invert the image
	// 相当于 cvNot(img);
	// IplImage *pDstImg = cvCreateImage(cvGetSize(img),img->depth,img->nChannels);
	// cvNot(img, pDstImg);
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
			for(k=0;k<channels;k++){
				//k=0:红 1:
				//data[i*step+j*channels+k]=
				//255-data[i*step+j*channels+k];
				if(k==0){//Red
					data[i*step+j*channels+k]=
							0xFF & data[i*step+j*channels+k];
					data2[i*step+j*channels+k]=
							0x00 & data2[i*step+j*channels+k];

				}
				if(k==1){//Green
					data[i*step+j*channels+k]=
							0x00 & data[i*step+j*channels+k];
					data2[i*step+j*channels+k]=
							0xFF & data2[i*step+j*channels+k];


				}
				if(k==2){//Blue
					data[i*step+j*channels+k]=
							0x00 & data[i*step+j*channels+k];
					data2[i*step+j*channels+k]=
							0xFF & data2[i*step+j*channels+k];

				}
			}
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
			for(k=0;k<channels;k++){
				if(k==0){//Red
					//合并的图片,红=图1
					data3[i*step+j*channels+k]=
							data[i*step+j*channels+k];
				}
				if(k==1){//Green
					//合并的突破片 青=图2
					data3[i*step+j*channels+k]=
							data2[i*step+j*channels+k];
				}
				if(k==2){//Blue
					//合并的突破片 青=图2
					data3[i*step+j*channels+k]=
							data2[i*step+j*channels+k];
				}
			}

	// show the image
	//cvShowImage("mainWin", img );
	//	capture = cvCreateCameraCapture(0);//打开摄像头，从摄像头中获取视频
	//  IplImage  *frame;
	//frame = cvQueryFrame(cam);// 从摄像头中抓取并返回每一帧

	QImage image((const uchar*)img->imageData, img->width, img->height, QImage::Format_RGB888);

	ui->label->setMinimumWidth(img->width);
	ui->label->setMinimumHeight(img->height);
	ui->label->setPixmap(QPixmap::fromImage(image));

	QImage image2((const uchar*)img2->imageData, img2->width, img2->height, QImage::Format_RGB888);
	ui->label_2->setMinimumWidth(img2->width);
	ui->label_2->setMinimumHeight(img2->height);
	ui->label_2->setPixmap(QPixmap::fromImage(image2));

	QImage image3((const uchar*)img3->imageData, img3->width, img3->height, QImage::Format_RGB888);
	ui->label_3->setMinimumWidth(img3->width);
	ui->label_3->setMinimumHeight(img3->height);
	ui->label_3->setPixmap(QPixmap::fromImage(image3));

	//移动位置
//	ui->label_3->geometry().setX(ui->label->lineWidth());
//	ui->label_2->geometry().setX(2*ui->label_3->lineWidth());

	//qimg->getQImage();
	// wait for a key
	cvWaitKey(0);

	// release the image
	cvReleaseImage(&img );
	//return 0;

}

MainWindow::~MainWindow()
{
	delete ui;
}

