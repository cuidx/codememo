#-------------------------------------------------
#
# Project created by QtCreator 2018-09-15T06:08:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET = takephoto
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    camrainput.cpp

HEADERS  += mainwindow.h \
    camrainput.h

FORMS    += mainwindow.ui



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/release/ -lavformat
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/debug/ -lavformat
else:unix: LIBS += -L$$PWD/../ffmpeg/lib/ -lavformat

INCLUDEPATH += $$PWD/../ffmpeg/include
DEPENDPATH += $$PWD/../ffmpeg/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/release/ -lavcodec
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/debug/ -lavcodec
else:unix: LIBS += -L$$PWD/../ffmpeg/lib/ -lavcodec

INCLUDEPATH += $$PWD/../ffmpeg/include
DEPENDPATH += $$PWD/../ffmpeg/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/release/ -lavdevice
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/debug/ -lavdevice
else:unix: LIBS += -L$$PWD/../ffmpeg/lib/ -lavdevice

INCLUDEPATH += $$PWD/../ffmpeg/include
DEPENDPATH += $$PWD/../ffmpeg/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/release/ -lavutil
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/debug/ -lavutil
else:unix: LIBS += -L$$PWD/../ffmpeg/lib/ -lavutil

INCLUDEPATH += $$PWD/../ffmpeg/include
DEPENDPATH += $$PWD/../ffmpeg/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/release/ -lswscale
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/debug/ -lswscale
else:unix: LIBS += -L$$PWD/../ffmpeg/lib/ -lswscale

INCLUDEPATH += $$PWD/../ffmpeg/include
DEPENDPATH += $$PWD/../ffmpeg/include
