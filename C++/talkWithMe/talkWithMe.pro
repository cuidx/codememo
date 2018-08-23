#-------------------------------------------------
#
# Project created by QtCreator 2018-08-18T18:00:55
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = talkWithMe
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    objectview.cpp \
    soundplayer.cpp

HEADERS  += mainwindow.h \
    objectview.h \
    soundplayer.h

FORMS    += mainwindow.ui

RESOURCES += \
    talkview.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/release/ -lSoundPlayerLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/debug/ -lSoundPlayerLib
else:unix: LIBS += -L$$PWD/lib/ -lSoundPlayerLib -Wl,-rpath=.

INCLUDEPATH += $$PWD/lib
DEPENDPATH += $$PWD/lib
