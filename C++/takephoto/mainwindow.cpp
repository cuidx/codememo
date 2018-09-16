#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCamera>
#include <QCameraInfo>
#include <QComboBox>
#include <QDebug>
#include <QPushButton>
#include <QPixmap>
#include <QPainter>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_bCapImg = false;

    QComboBox *camlist = new QComboBox(this);
    camlist->setGeometry(20,30,300,30);
    QPushButton *previewBtn = new QPushButton(this);
    QPushButton *captureBtn = new QPushButton(this);
    m_img = new QLabel(this);
    previewBtn->setGeometry(camlist->geometry().right() + 20,30,100,30);
    captureBtn->setGeometry(previewBtn->geometry().right() + 20,30,100,30);
    m_img->setGeometry(20,80,640,480);

    previewBtn->setText(QString::fromLocal8Bit("Start Preview"));
    captureBtn->setText(QString::fromLocal8Bit("Capture"));

    m_camIput = new CamraInput();
    m_camIput->Init(&MainWindow::FrameCallBack,(void*)this);
    std::list<std::string> devs;
    m_camIput->GetCamraDevList(devs);
    for (std::list<std::string>::iterator iter = devs.begin(); iter != devs.end(); iter++)
    {
        camlist->addItem(QString::fromUtf8(iter->c_str()));
    }

    connect(camlist,SIGNAL(currentTextChanged(QString)),this,SLOT(onCamraDevChange(QString)));
    connect(captureBtn,SIGNAL(clicked(bool)),this,SLOT(onCapture()));
    connect(previewBtn,SIGNAL(clicked(bool)),this,SLOT(onPreview()));
    connect(this,SIGNAL(onCapImgSignal(QImage)),this,SLOT(onCapImag(QImage)));

#ifdef USE_QCAMRA
    QList<QCameraInfo> camInfos = QCameraInfo::availableCameras();
    for (QList<QCameraInfo>::iterator iter = camInfos.begin(); iter != camInfos.end();iter++)
    {
        camlist->addItem(iter->deviceName());
    }
    if (camlist->count() > 0)
    {
        camlist->setCurrentIndex(0);
    }
    QString defaultcam = QCameraInfo::defaultCamera().deviceName();
    QString camdesc = QCameraInfo::defaultCamera().description();
    qDebug()<<"default camera:"<<defaultcam<<", desc:"<<camdesc;
#endif

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCamraDevChange(QString text)
{
    if (m_camIput && !text.isEmpty())
    {
        m_camIput->SetDefaultCamra(text.toStdString());
    }
}

void MainWindow::onCapture()
{
    m_bCapImg = true;
}

void MainWindow::onPreview()
{
    if (m_camIput)
    {
        m_camIput->StartCapture();
    }
}

void MainWindow::FrameCallBack(unsigned char* data, unsigned int len,int width,int height,int format,void* userdata)
{
    MainWindow* pThis = (MainWindow*)userdata;
    if (pThis)
    {
        pThis->OnGetFrame(data,len,width,height,format);
    }
}

void MainWindow::OnGetFrame(unsigned char* data, unsigned int len,int width,int height,int format)
{
    qDebug() << "OnGetFrame get frame,type="<<format<<" ,len="<<len<<"w="<<width<<"h="<<height;
    if (format == 2)    //packed RGB 8:8:8, 24bpp, RGBRGB...
    {
        QImage pic(data,width,height,QImage::Format_RGB888);
       // emit onCapImgSignal(pic);
        m_pic = pic.copy();
        if (m_bCapImg)
        {
            QString ctime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
            QString fname = QString("image_") + ctime + QString(".png");
            m_pic.save(fname,"PNG");
            m_bCapImg = false;
        }
        update();
    }//
}

void MainWindow::onCapImag(QImage pic)
{
    QPixmap pix;
    QPainter painter(m_img);
    painter.drawImage(0,0,pic);
    m_img->setPixmap(pix);
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.drawImage(20,80,m_pic);
    qDebug() <<"paintevent draw image";
}
