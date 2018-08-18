#include "soundplayer.h"


SoundPlayer* SoundPlayer::m_instance = NULL;

SoundPlayer* SoundPlayer::Instance()
{
    if (NULL == m_instance)
    {
        m_instance = new SoundPlayer();
    }
    return m_instance;
}

SoundPlayer::SoundPlayer()
{
    m_player = new QMediaPlayer();
}

void SoundPlayer::PlayFile(const QString& url)
{
    m_file = url;
    if (m_player->state() == QMediaPlayer::PlayingState)
    {
        m_player->stop();
    }
    m_player->setMedia(QMediaContent(QUrl::fromLocalFile(url)));
    m_player->play();
    qDebug()<<"start play sound file" << url;
}
