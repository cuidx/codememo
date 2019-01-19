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
    soundplayer.cpp \
    playlist.cpp

HEADERS  += mainwindow.h \
    objectview.h \
    soundplayer.h \
    playlist.h

FORMS    += mainwindow.ui

RESOURCES += \
    talkview.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/release/ -lSoundPlayerLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/debug/ -lSoundPlayerLib
else:unix: LIBS += -L$$PWD/lib/ -lSoundPlayerLib -Wl,-rpath=.

INCLUDEPATH += $$PWD/lib
DEPENDPATH += $$PWD/lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/release/ -lportaudio
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/debug/ -lportaudio
else:unix: LIBS += -L$$PWD/lib/ -lportaudio

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/release/ -lespeak
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/debug/ -lespeak
else:unix: LIBS += -L$$PWD/lib/ -lespeak

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/release/libespeak.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/debug/libespeak.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/release/espeak.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/debug/espeak.lib
else:unix: PRE_TARGETDEPS += $$PWD/lib/libespeak.a
