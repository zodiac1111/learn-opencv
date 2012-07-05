TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.c



HEADERS += \
    main.h


#设置库搜索目录
LIBS += -L/usr/local/lib

LIBS += -lopencv_core \
	 -lopencv_highgui

