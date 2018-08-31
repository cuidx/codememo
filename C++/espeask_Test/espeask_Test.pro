#-------------------------------------------------
#
# Project created by QtCreator 2018-08-30T06:43:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = espeask_Test
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libespeak/lib/release/ -lespeak
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libespeak/lib/debug/ -lespeak
else:unix: LIBS += -L$$PWD/libespeak/lib/ -lespeak

INCLUDEPATH += $$PWD/libespeak/include
DEPENDPATH += $$PWD/libespeak/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libespeak/lib/release/libespeak.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libespeak/lib/debug/libespeak.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libespeak/lib/release/espeak.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libespeak/lib/debug/espeak.lib
else:unix: PRE_TARGETDEPS += $$PWD/libespeak/lib/libespeak.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libespeak/lib/release/ -lportaudio
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libespeak/lib/debug/ -lportaudio
else:unix: LIBS += -L$$PWD/libespeak/lib/ -lportaudio

INCLUDEPATH += $$PWD/libespeak/include
DEPENDPATH += $$PWD/libespeak/include
