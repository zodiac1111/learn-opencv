#include <QtGui/QApplication>
#include "mainwindow.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <QImage>
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

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	//QCvImage qcv;
	//qcv.getQImage();

	return a.exec();
}
