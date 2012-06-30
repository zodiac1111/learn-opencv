#-------------------------------------------------
#
# Project created by QtCreator 2012-06-13T19:51:48
#
#-------------------------------------------------

QT       += core gui
VERSION = 0.0.1
TARGET = cvt1
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

#界面
FORMS    += mainwindow.ui

#头文件 用库文件包含2方式就不需要这个了
#INCLUDEPATH +=  /usr/include/opencv

#库文件 绝对路径 慎用
#LIBS += /usr/lib/libopencv_core.so \
#	/usr/lib/libopencv_highgui.so

#库文件2 这种方式表示更健壮
LIBS += -lopencv_core \
	 -lopencv_highgui \


RESOURCES += \
    res.qrc
