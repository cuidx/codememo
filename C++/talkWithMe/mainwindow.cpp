#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "objectview.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ObjectView *view = new ObjectView(this);
    view->setGeometry(rect());
    view->SetResource(QString(":/res/Hello.bmp"),
                      QString(""),//QString("/home/cuidx/talkWithMe/sample.wav"),
                     // QString::fromUtf8("<emphasis level=\'strong\'>爸</emphasis><emphasis level=\'reduce\'>爸</emphasis>"));
                      //QString::fromUtf8("爸<break time=\'300ms\'/>爸"));
                      QString::fromUtf8("<phoneme>ba4</phoneme><phoneme>ba</phoneme>"));

    m_playList = new PlayList();
    m_playList->LoadPlayList(std::string("./playList.xml"));
    if (m_playList->GetCount() == 0)
    {
        for (int iPos = 0; iPos < 10; iPos++)
        {
            ObjectInfo obj;
            obj.m_title = "TestObject";
            obj.m_title.push_back('0' + iPos);
            obj.m_picPath = ":/res/Hello.bmp";
            obj.m_soundPath = "/home/cuidx/talkWithMe/sample.wav";
            m_playList->AddObject(obj);
        }
        m_playList->SavePlayList();
    }
    else
    {
        qDebug() << "playlist count : " <<m_playList->GetCount();
        PlayList::PlayListIterator iter = m_playList->begin();
        for (; iter != m_playList->end(); iter++)
        {
            qDebug()<<QString::fromStdString(iter->m_title)<< " \t"
                   <<QString::fromStdString(iter->m_picPath)<<" \t"
                  <<QString::fromStdString(iter->m_soundPath);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
