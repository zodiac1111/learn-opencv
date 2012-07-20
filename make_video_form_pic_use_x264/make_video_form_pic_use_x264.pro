TEMPLATE = app
CONFIG += console125
CONFIG -= qt

SOURCES += \
    main.c \
    convert_csp.c

HEADERS += \
    main.h \
    convert_csp.h
LIBS += -L/usr/local/lib

INCLUDEPATH += /usr/local/include/

#x264 need these library
LIBS += -lm \
	-lpthread \
	-lx264
