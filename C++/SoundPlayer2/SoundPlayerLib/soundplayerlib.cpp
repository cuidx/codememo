#include "soundplayerlib.h"
#include "SDL2/SDL_config.h"
#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"

extern int PlayFile(const char* file);

using namespace std;

static struct wavedata
{
    SDL_AudioSpec spec;
    Uint8 *sound;               /* Pointer to wave data */
    Uint32 soundlen;            /* Length of wave data */
    int soundpos;               /* Current play position */
} wave;

static SDL_AudioDeviceID device;
static int done = 0;

/* Call this instead of exit(), so we can clean up SDL: atexit() is evil. */
static void
quit(int rc)
{
    SDL_Quit();
    exit(rc);
}

static void
close_audio()
{
    if (device != 0) {
        SDL_CloseAudioDevice(device);
        device = 0;
    }
}

static void
open_audio()
{
    /* Initialize fillerup() variables */
    device = SDL_OpenAudioDevice(NULL, SDL_FALSE, &wave.spec, NULL, 0);
    if (!device) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't open audio: %s\n", SDL_GetError());
        SDL_FreeWAV(wave.sound);
        quit(2);
    }


    /* Let the audio run */
    SDL_PauseAudioDevice(device, SDL_FALSE);
}

static void reopen_audio()
{
    close_audio();
    open_audio();
}


void SDLCALL
fillerup(void *unused, Uint8 * stream, int len)
{
    Uint8 *waveptr;
    int waveleft;

    /* Set up the pointers */
    waveptr = wave.sound + wave.soundpos;
    waveleft = wave.soundlen - wave.soundpos;

    /* Go! */
    while (waveleft <= len) {
        SDL_memcpy(stream, waveptr, waveleft);
        stream += waveleft;
        len -= waveleft;
        waveptr = wave.sound;
        waveleft = wave.soundlen;
        wave.soundpos = 0;
        done = true;
        return ;
    }
    SDL_memcpy(stream, waveptr, len);
    wave.soundpos += len;
}



#ifdef __EMSCRIPTEN__
void
loop()
{
    if(done || (SDL_GetAudioDeviceStatus(device) != SDL_AUDIO_PLAYING))
        emscripten_cancel_main_loop();
}
#endif


void SoundPlayerLib::PlayFunc(SoundPlayerLib* player)
{
    if (NULL == player)
    {
        return;
    }

    while (player->m_isRunning)
    {
        std::unique_lock<std::mutex> lk(player->m_playerMutex);
        player->m_playerCondition.wait(lk);

        if (!player->m_isRunning)
        {
            return;
        }

        done = false;

        int i;
        char filename[4096];

        SDL_strlcpy(filename, player->m_playFile.c_str(), sizeof(filename));
        /* Load the wave file into memory */
        wave.soundpos = 0;
        if (SDL_LoadWAV(filename, &wave.spec, &wave.sound, &wave.soundlen) == NULL) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load %s: %s\n", filename, SDL_GetError());
            quit(1);
        }

        wave.spec.callback = fillerup;

        /* Show the list of available drivers */
        SDL_Log("Available audio drivers:");
        for (i = 0; i < SDL_GetNumAudioDrivers(); ++i) {
            SDL_Log("%i: %s", i, SDL_GetAudioDriver(i));
        }

        SDL_Log("Using audio driver: %s\n", SDL_GetCurrentAudioDriver());

        open_audio();

        SDL_FlushEvents(SDL_AUDIODEVICEADDED, SDL_AUDIODEVICEREMOVED);

    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(loop, 0, 1);
    #else
        while (!done) {
            SDL_Event event;

            while (SDL_PollEvent(&event) > 0) {
                if (event.type == SDL_QUIT) {
                    done = 1;
                }
                if ((event.type == SDL_AUDIODEVICEADDED && !event.adevice.iscapture) ||
                    (event.type == SDL_AUDIODEVICEREMOVED && !event.adevice.iscapture && event.adevice.which == device)) {
                    reopen_audio();
                }
            }
            SDL_Delay(100);
        }
    #endif

        /* Clean up on signal */
        close_audio();
        SDL_FreeWAV(wave.sound);

    }
}


SoundPlayerLib::SoundPlayerLib()
{
    m_isRunning = false;
}

bool SoundPlayerLib::PlaySoundFile(const char* filename)
{
    if (NULL == filename)
    {
        return false;
    }

    const char* sufix = strrchr(filename,'.');
    if (NULL == sufix)
    {
        return false;
    }
    if (strcasecmp(sufix,".mp3") == 0)
    {
        PlayFile(filename);
        return true;
    }

    done = true;

    {
        std::unique_lock<std::mutex> lk(m_playerMutex);
        m_playFile = filename;
    }

    m_playerCondition.notify_one();

    return true;
}

bool SoundPlayerLib::Init()
{
    /* Enable standard application logging */
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    /* Load the SDL library */
    if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_EVENTS) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    m_isRunning = true;

    m_playerThread = std::thread(PlayFunc,this);

    return true;
}

bool SoundPlayerLib::UnInit()
{
    if (m_isRunning)
    {
        m_isRunning = false;
        m_playerCondition.notify_one();
        m_playerThread.join();
        SDL_Quit();
    }
    return true;
}
