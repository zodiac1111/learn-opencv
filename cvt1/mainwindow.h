#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <QImage>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	
private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
