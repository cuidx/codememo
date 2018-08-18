#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <QString>
#include <QMediaPlayer>

class SoundPlayer
{
public:
    SoundPlayer();

    static SoundPlayer* Instance();

    void PlayFile(const QString& url);

protected:
    QString m_file;
    QMediaPlayer* m_player;

    static SoundPlayer* m_instance;
};

#endif // SOUNDPLAYER_H
