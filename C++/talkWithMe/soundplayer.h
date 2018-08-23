#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <QString>
#include <QMediaPlayer>
#include "inc/soundplayerlib.h"

class SoundPlayer
{
public:
    SoundPlayer();
    ~SoundPlayer();

    static SoundPlayer* Instance();

    void PlayFile(const QString& url);

protected:
    QString m_file;
#ifdef USE_QMEDIA
    QMediaPlayer* m_player;
#else
    SoundPlayerLib* m_player;
#endif

    static SoundPlayer* m_instance;
};

#endif // SOUNDPLAYER_H
