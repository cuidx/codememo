#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "SDL2/SDL.h"
#include <QComboBox>
#include <QPushButton>
#include <QDebug>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_bIsRecording = false;
    m_capDevId = 0;
    m_playDevId = 0;
    SDL_zero(m_audioSpec);

    QComboBox *pAudioDevsIn = new QComboBox(this);
    QComboBox *pAudioDevsOut = new QComboBox(this);
    QComboBox *pAudioDriver = new QComboBox(this);
    QPushButton *pRecord = new QPushButton(this);
    m_play = new QPushButton(this);

    pRecord->setText(QString::fromLocal8Bit("Start Record"));
    pRecord->setGeometry(50,150,120,28);

    m_play->setText(QString::fromLocal8Bit("Start Play"));
    m_play->setGeometry(pRecord->geometry().right() + 20,pRecord->geometry().top(),pRecord->geometry().width(),pRecord->geometry().height());

    connect(pRecord,SIGNAL(clicked()),this,SLOT(onRecord()));
    connect(m_play,SIGNAL(clicked(bool)),this,SLOT(onPlay()));
    connect(this,SIGNAL(playEnd()),this,SLOT(onPlayEnd()));

    pAudioDevsIn->setGeometry(20,30,300,28);
    pAudioDevsOut->setGeometry(20,70,300,28);
    pAudioDriver->setGeometry(20,110,300,28);

    SDL_Init(SDL_INIT_AUDIO);

    int nAudioDriver = SDL_GetNumAudioDrivers();
    for (int nPos = 0; nPos < nAudioDriver; nPos++)
    {
        const char *name = SDL_GetAudioDriver(nPos);
        if (name)
        {
            pAudioDriver->addItem(QString::fromLocal8Bit(name));
        }
    }
    if (pAudioDriver->count() > 0)
    {
        pAudioDriver->setCurrentIndex(0);
    }

    int nCapDevNum = SDL_GetNumAudioDevices(1);
    for (int nPos = 0; nPos < nCapDevNum; nPos++)
    {
        const char* name = SDL_GetAudioDeviceName(nPos,1);
        if (name != NULL)
        {
            pAudioDevsIn->addItem(QString::fromLocal8Bit(name));
        }
    }
    if (pAudioDevsIn->count() > 0)
    {
        pAudioDevsIn->setCurrentIndex(0);
        SDL_zero(m_audioOutSpec);
        m_audioOutSpec.freq = 44100;
        m_audioOutSpec.channels = 2;
        m_audioOutSpec.format = AUDIO_U16LSB;
        m_audioOutSpec.samples = 4096;
        m_audioOutSpec.callback = &MainWindow::SDL_AudioPlayCallBack;
        m_audioOutSpec.userdata = (void*)this;
        m_playDevId = SDL_OpenAudioDevice(NULL,SDL_FALSE,&m_audioOutSpec,&m_audioOutSpec,SDL_AUDIO_ALLOW_ANY_CHANGE);

        qDebug()<<"Audio out spec: frea="<<m_audioOutSpec.freq<<" channel="<<m_audioOutSpec.channels<<" format="<<m_audioOutSpec.format
               <<" samples="<<m_audioOutSpec.samples<<" slience="<<m_audioOutSpec.silence<<" size" << m_audioOutSpec.size;

        m_audioSpec = m_audioOutSpec;
        m_audioSpec.callback = &MainWindow::SDL_AudioCaptureCallBack;
        m_audioSpec.userdata = (void*)this;
        m_capDevId = SDL_OpenAudioDevice(NULL,SDL_TRUE,&m_audioSpec,&m_audioSpec,SDL_AUDIO_ALLOW_ANY_CHANGE);
        SDL_PauseAudioDevice(m_capDevId,0);

        qDebug()<<"Audio In spec: frea="<<m_audioSpec.freq<<" channel="<<m_audioSpec.channels<<" format="<<m_audioSpec.format
               <<" samples="<<m_audioSpec.samples<<" slience="<<m_audioSpec.silence<<" size" << m_audioSpec.size;
    }

    int nPlayDevNum = SDL_GetNumAudioDevices(0);
    for (int nPos = 0; nPos < nPlayDevNum; nPos++)
    {
        const char* name = SDL_GetAudioDeviceName(nPos,0);
        if (name != NULL)
        {
            pAudioDevsOut->addItem(QString::fromLocal8Bit(name));
        }
    }
    if (pAudioDevsOut->count() > 0)
    {
        pAudioDevsOut->setCurrentIndex(0);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onRecord()
{
    QPushButton* pBtn = (QPushButton*)sender();
    if (pBtn)
    {
        QString text = pBtn->text();
        if (text.compare(QString::fromLocal8Bit("Start Record")) == 0)
        {
            pBtn->setText(QString::fromLocal8Bit("Stop Record"));
            if (m_capDevId > 0)
            {
                m_bIsRecording = true;
                //SDL_ClearQueuedAudio(m_capDevId);
                //SDL_PauseAudioDevice(m_capDevId,0);
            }
        }
        else
        {
            pBtn->setText(QString::fromLocal8Bit("Start Record"));
            if (m_capDevId > 0)
            {
                m_bIsRecording = false;
                //SDL_PauseAudioDevice(m_capDevId,1);
            }
        }
    }
}

void MainWindow::SDL_AudioCaptureCallBack(void *userdata,Uint8 * stream, int len)
{
    qDebug()<<"recv audio cb "<<len<<" " <<QDateTime::currentDateTime().toString(Qt::DefaultLocaleLongDate);
    MainWindow *pThis = (MainWindow*)userdata;
    if (pThis)
    {
        pThis->SaveAudioData(stream,len);
    }
}

void MainWindow::SDL_AudioPlayCallBack(void *userdata,Uint8 * stream, int len)
{
    qDebug()<<"play audio cb "<<len<<" " <<QDateTime::currentDateTime().toString(Qt::DefaultLocaleLongDate);
    MainWindow *pThis = (MainWindow*)userdata;
    if (pThis)
    {
        pThis->GetAudioData(stream,len);
    }
}

void MainWindow::onPlay()
{
    QPushButton* pBtn = (QPushButton*)sender();
    if (pBtn)
    {
        QString text = pBtn->text();
        if (text.compare(QString::fromLocal8Bit("Start Play")) == 0)
        {
            pBtn->setText(QString::fromLocal8Bit("Stop Play"));
            if (m_playDevId > 0)
            {
                SDL_PauseAudioDevice(m_playDevId,0);
            }
        }
        else
        {
            pBtn->setText(QString::fromLocal8Bit("Start Play"));
            if (m_playDevId > 0)
            {
                SDL_PauseAudioDevice(m_playDevId,1);
            }
        }
    }
}

void MainWindow::SaveAudioData(Uint8* data,int len)
{
    if (m_bIsRecording)
    {
        AudioFrame* frame = new AudioFrame(data,len);
        m_audioList.push_back(frame);
    }
}

void MainWindow::GetAudioData(Uint8* data,int len)
{
    if (data && len > 0)
    {
        while (len > 0 && m_audioList.size() > 0)
        {
            AudioFrame* frame = m_audioList.front();
            if (frame->m_dataLen <= len)
            {
                memcpy(data,frame->m_data,frame->m_dataLen);
                len -= frame->m_dataLen;
                data += frame->m_dataLen;
                m_audioList.pop_front();
                delete frame;
                frame = NULL;
            }
            else
            {
                memcpy(data,frame->m_data,len);
                memcpy(frame->m_data,frame->m_data + len,frame->m_dataLen - len);
                frame->m_dataLen -= len;
                len = 0;
            }
        }
        if (len > 0)
        {
            emit playEnd();
        }
    }
}

void MainWindow::onPlayEnd()
{
    if (m_playDevId > 0)
    {
        SDL_PauseAudioDevice(m_playDevId,1);
        qDebug()<<"Play End";
    }
    if (m_play)
    {
        m_play->setText("Start Play");
    }
}
