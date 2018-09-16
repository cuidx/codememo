#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <SDL2/SDL.h>
#include <list>
#include <thread>
#include <functional>

namespace Ui {
class MainWindow;
}

class QPushButton;

class AudioFrame
{
public:
    AudioFrame(Uint8 *data,int len)
    {
        if (data && len > 0)
        {
            m_data = new Uint8[len];
            memcpy(m_data,data,len);
            m_dataLen = len;
        }
        else
        {
            m_data = NULL;
            m_dataLen = 0;
        }
    }
    ~AudioFrame()
    {
        if (m_data)
        {
            delete []m_data;
            m_data = NULL;
            m_dataLen = 0;
        }
    }

    Uint8* m_data;
    int m_dataLen;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void onRecord();
    void onPlay();
    void onPlayEnd();

signals:
    void playEnd();

protected:
    static void SDL_AudioCaptureCallBack(void *userdata,Uint8 * stream, int len);
    static void SDL_AudioPlayCallBack(void *userdata,Uint8 * stream, int len);

    void SaveAudioData(Uint8* data,int len);
    void GetAudioData(Uint8* data,int len);

    void DoEncode();

private:
    Ui::MainWindow *ui;
    QPushButton *m_play;

    SDL_AudioDeviceID m_capDevId;
    SDL_AudioDeviceID m_playDevId;
    SDL_AudioSpec m_audioSpec;
    SDL_AudioSpec m_audioOutSpec;

    std::list<AudioFrame*> m_audioList;
    bool m_bIsRecording;

    std::thread m_encodeThread;
};

#endif // MAINWINDOW_H
