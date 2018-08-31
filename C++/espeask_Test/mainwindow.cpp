#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "./libespeak/include/speak_lib.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btn_clear,SIGNAL(clicked(bool)),ui->plainTextEdit,SLOT(clear()));
    connect(ui->btn_play,SIGNAL(clicked(bool)),this,SLOT(playsound()));

    static const char* err_load = "Failed to read ";
    int synth_flags = espeakCHARS_AUTO | espeakPHONEMES | espeakENDPAUSE;
    char *p_text = "欢迎使用";
    espeak_VOICE voice_select;
    char voicename[40] = "zh";
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
            qDebug() << err_load << "voice " << voicename;
            //fprintf(stderr,"%svoice '%s'\n",err_load,voicename);
            //exit(2);
        }
    }

    if(p_text != NULL)
    {
        int size;
        size = strlen(p_text);
        espeak_Synth(p_text,size+1,0,POS_CHARACTER,0,synth_flags,NULL,NULL);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::playsound()
{
    QString sText = ui->plainTextEdit->toPlainText();
    if (!sText.isEmpty())
    {
        int synth_flags = espeakCHARS_AUTO | espeakPHONEMES | espeakENDPAUSE;
        int size;
        size = sText.toStdString().length();
        espeak_Synth(sText.toStdString().c_str(),size+1,0,POS_CHARACTER,0,synth_flags,NULL,NULL);
    }
}
