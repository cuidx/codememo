#-------------------------------------------------
#
# Project created by QtCreator 2018-08-20T07:24:27
#
#-------------------------------------------------

QT       -= core gui

TARGET = SoundPlayerLib
TEMPLATE = lib

DEFINES += SOUNDPLAYERLIB_LIBRARY

SOURCES += soundplayerlib.cpp \
    mp3filedecoder.cpp \
    ffplayer.cpp

HEADERS += soundplayerlib.h\
        soundplayerlib_global.h \
    mp3filedecoder.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libsdl/bin/release/ -lSDL2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libsdl/bin/debug/ -lSDL2
else:unix: LIBS += -L$$PWD/libsdl/bin/ -lSDL2 -Wl,--no-undefined -lm -ldl -lpthread -lrt -g

INCLUDEPATH += $$PWD/libsdl/bin
DEPENDPATH += $$PWD/libsdl/bin

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libsdl/bin/release/libSDL2.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libsdl/bin/debug/libSDL2.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libsdl/bin/release/SDL2.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libsdl/bin/debug/SDL2.lib
else:unix: PRE_TARGETDEPS += $$PWD/libsdl/bin/libSDL2.a


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/release/ -lavcodec
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/debug/ -lavcodec
else:unix: LIBS += -L$$PWD/../ffmpeg/lib/ -lavcodec

INCLUDEPATH += $$PWD/../ffmpeg/include
DEPENDPATH += $$PWD/../ffmpeg/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/release/libavcodec.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/debug/libavcodec.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/release/avcodec.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/debug/avcodec.lib
else:unix: PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/libavcodec.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/release/ -lavformat
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/debug/ -lavformat
else:unix: LIBS += -L$$PWD/../ffmpeg/lib/ -lavformat

INCLUDEPATH += $$PWD/../ffmpeg/include
DEPENDPATH += $$PWD/../ffmpeg/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/release/libavformat.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/debug/libavformat.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/release/avformat.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/debug/avformat.lib
else:unix: PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/libavformat.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/release/ -lswscale
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/debug/ -lswscale
else:unix: LIBS += -L$$PWD/../ffmpeg/lib/ -lswscale

INCLUDEPATH += $$PWD/../ffmpeg/include
DEPENDPATH += $$PWD/../ffmpeg/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/release/libswscale.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/debug/libswscale.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/release/swscale.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/debug/swscale.lib
else:unix: PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/libswscale.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/release/ -lavutil
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/debug/ -lavutil
else:unix: LIBS += -L$$PWD/../ffmpeg/lib/ -lavutil

INCLUDEPATH += $$PWD/../ffmpeg/include
DEPENDPATH += $$PWD/../ffmpeg/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/release/libavutil.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/debug/libavutil.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/release/avutil.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/debug/avutil.lib
else:unix: PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/libavutil.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/release/ -lswresample
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ffmpeg/lib/debug/ -lswresample
else:unix: LIBS += -L$$PWD/../ffmpeg/lib/ -lswresample

INCLUDEPATH += $$PWD/../ffmpeg/include
DEPENDPATH += $$PWD/../ffmpeg/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/release/libswresample.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/debug/libswresample.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/release/swresample.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/debug/swresample.lib
else:unix: PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/libswresample.a
