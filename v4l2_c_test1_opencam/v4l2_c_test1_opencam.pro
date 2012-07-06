TEMPLATE = app
CONFIG += console
CONFIG -= qt
QT       += core gui
SOURCES += \
	main.c \
	image.c




HEADERS += \
    main.h \
	image.h


#设置库搜索目录
LIBS += -L/usr/local/lib
LIBS += -L/usr/lib
LIBS += -lopencv_core \
	 -lopencv_highgui


