TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp

#opencv库
LIBS += -lopencv_core \		#一般函数
	 -lopencv_highgui \   #
	-lopencv_imgproc #图像处理 包括cvResize() 函数
