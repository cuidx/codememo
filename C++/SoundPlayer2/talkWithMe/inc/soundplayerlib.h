#ifndef SOUNDPLAYERLIB_H
#define SOUNDPLAYERLIB_H

#include "soundplayerlib_global.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>

class SOUNDPLAYERLIBSHARED_EXPORT SoundPlayerLib
{

public:
    SoundPlayerLib();

    virtual bool Init();

    virtual bool UnInit();

    virtual bool PlaySoundFile(const char* filename);

    static void PlayFunc(SoundPlayerLib* player);

protected:
    std::thread m_playerThread;
    std::mutex  m_playerMutex;
    std::condition_variable m_playerCondition;
    std::string m_playFile;
    bool        m_isRunning;
};

#endif // SOUNDPLAYERLIB_H
