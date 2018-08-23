#-------------------------------------------------
#
# Project created by QtCreator 2018-08-20T07:24:27
#
#-------------------------------------------------

QT       -= core gui

TARGET = SoundPlayerLib
TEMPLATE = lib

DEFINES += SOUNDPLAYERLIB_LIBRARY

SOURCES += soundplayerlib.cpp

HEADERS += soundplayerlib.h\
        soundplayerlib_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libsdl/bin/release/ -lSDL2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libsdl/bin/debug/ -lSDL2
else:unix: LIBS += -L$$PWD/libsdl/bin/ -lSDL2 -Wl,--no-undefined -lm -ldl -lpthread -lrt

INCLUDEPATH += $$PWD/libsdl/bin
DEPENDPATH += $$PWD/libsdl/bin

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libsdl/bin/release/libSDL2.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libsdl/bin/debug/libSDL2.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libsdl/bin/release/SDL2.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libsdl/bin/debug/SDL2.lib
else:unix: PRE_TARGETDEPS += $$PWD/libsdl/bin/libSDL2.a
