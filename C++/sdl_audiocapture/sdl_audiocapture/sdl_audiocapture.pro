#-------------------------------------------------
#
# Project created by QtCreator 2018-09-09T09:59:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sdl_audiocapture
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../usr/local/lib/release/ -lSDL2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../usr/local/lib/debug/ -lSDL2
else:unix: LIBS += -L$$PWD/../../../usr/local/lib/ -lSDL2

INCLUDEPATH += $$PWD/../../../usr/local/include
DEPENDPATH += $$PWD/../../../usr/local/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lame/bin/release/ -lmp3lame
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lame/bin/debug/ -lmp3lame
else:unix: LIBS += -L$$PWD/lame/bin/ -lmp3lame

INCLUDEPATH += $$PWD/lame/bin
DEPENDPATH += $$PWD/lame/bin
