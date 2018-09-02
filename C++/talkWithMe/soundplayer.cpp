#include "soundplayer.h"
#include "./inc/speak_lib.h"


SoundPlayer* SoundPlayer::m_instance = NULL;

SoundPlayer* SoundPlayer::Instance()
{
    if (NULL == m_instance)
    {
        m_instance = new SoundPlayer(PLAYER_TYPE_ALL);
    }
    return m_instance;
}

SoundPlayer::SoundPlayer(int flag)
{
    if ((flag & PLAYER_TYPE_WAVEFILE) != 0)
    {
#ifdef USE_QMEDIA
        m_player = new QMediaPlayer();
#else
        m_player = new SoundPlayerLib();
        m_player->Init();
#endif
    }
    else
    {
        m_player = NULL;
    }

    if ((flag & PLAYER_TYPE_TTS) != 0)
    {
        espeak_VOICE voice_select;
        char voicename[40] = "zh+f2";
        char *data_path = NULL;   // use default path for espeak-data
        int samplerate = espeak_Initialize(AUDIO_OUTPUT_PLAYBACK,0,data_path,0);

        if(voicename[0] == 0)
            strcpy(voicename,"default");

        if(espeak_SetVoiceByName(voicename) != EE_OK)
        {
            memset(&voice_select,0,sizeof(voice_select));
            voice_select.languages = voicename;
            if(espeak_SetVoiceByProperties(&voice_select) != EE_OK)
            {
                qDebug() << "fail to load" << "voice " << voicename;
                //fprintf(stderr,"%svoice '%s'\n",err_load,voicename);
                //exit(2);
            }
        }
        m_ttsEnginInit = true;
    }
    else
    {
        m_ttsEnginInit = false;
    }
}

SoundPlayer::~SoundPlayer()
{
    if (m_player != NULL)
    {
#ifndef USE_QMEDIA
        m_player->UnInit();
#endif
        delete m_player;
        m_player = NULL;
    }

    if (m_ttsEnginInit)
    {
        espeak_Cancel();
    }
}

void SoundPlayer::PlayFile(const QString& url)
{
    if (m_player != NULL)
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
    }
    qDebug()<<"start play sound file" << url;
}

void SoundPlayer::PlayText(const char* text)
{
    if (!m_ttsEnginInit)
    {
        return;
    }
    espeak_Cancel();

    if (NULL != text)
    {
        int synth_flags = espeakCHARS_AUTO | espeakPHONEMES | espeakENDPAUSE | espeakSSML;
        int size;
        size = strlen(text);
        espeak_Synth(text,size+1,0,POS_CHARACTER,0,synth_flags,NULL,NULL);
    }
}
