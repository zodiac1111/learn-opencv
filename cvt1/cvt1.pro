#-------------------------------------------------
#
# Project created by QtCreator 2012-06-13T19:51:48
#
#-------------------------------------------------

QT       += core gui

TARGET = cvt1
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

#头文件
INCLUDEPATH +=  /usr/include/opencv

#库文件
LIBS += /usr/lib64/libopencv_core.so \
	/usr/lib64/libopencv_highgui.so
