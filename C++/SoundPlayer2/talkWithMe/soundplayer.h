#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <QString>
#include <QMediaPlayer>
#include "inc/soundplayerlib.h"

enum SOUND_PLAYER_FLAGS
{
    PLAYER_TYPE_WAVEFILE    = 1,    //sound file(.wav only current)
    PLAYER_TYPE_TTS         = 2,    //tts text

    PLAYER_TYPE_ALL         = PLAYER_TYPE_WAVEFILE | PLAYER_TYPE_TTS,
};

class SoundPlayer
{
public:
    SoundPlayer(int flag = 1);
    ~SoundPlayer();

    static SoundPlayer* Instance();

    void PlayFile(const QString& url);

    void PlayText(const char* text);

protected:
    QString m_file;
#ifdef USE_QMEDIA
    QMediaPlayer* m_player;
#else
    SoundPlayerLib* m_player;
#endif

    bool m_ttsEnginInit;
    static SoundPlayer* m_instance;
};

#endif // SOUNDPLAYER_H
