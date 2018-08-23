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
#ifdef USE_QMEDIA
    m_player = new QMediaPlayer();
#else
    m_player = new SoundPlayerLib();
    m_player->Init();
#endif
}

SoundPlayer::~SoundPlayer()
{
#ifndef USE_QMEDIA
    m_player->UnInit();
#endif
    delete m_player;
}

void SoundPlayer::PlayFile(const QString& url)
{
    m_file = url;
#ifdef USE_QMEDIA
    if (m_player->state() == QMediaPlayer::PlayingState)
    {
        m_player->stop();
    }
    m_player->setMedia(QMediaContent(QUrl::fromLocalFile(url)));
    m_player->play();
#else
    m_player->PlaySoundFile(m_file.toStdString().c_str());
#endif
    qDebug()<<"start play sound file" << url;
}
