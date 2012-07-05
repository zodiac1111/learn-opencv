SOURCES += main.cpp
#opencv库
LIBS += -lopencv_core \
	 -lopencv_highgui \
	-lopencv_imgproc #图像处理 包括cvResize() 函数


#头文件 用库文件包含2方式就不需要这个了 搜素
INCLUDEPATH +=  /usr/local/include/

#设置库搜索目录
LIBS += -L/usr/local/lib

