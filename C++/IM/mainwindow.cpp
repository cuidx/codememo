#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWebKit/QWebView>
#include <QPushButton>
#include <QWebFrame>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWebView *view = new QWebView(this);
    connect(view,SIGNAL(loadFinished(bool)),this,SLOT(webDocLoadFinished(bool)));
    view->load(QUrl::fromLocalFile("C:/Users/cuidx/Documents/IM/chat.html"));
    view->setGeometry(0,0,800,600);
    view->show();
    m_webView = view;
    QPushButton *btnA = new QPushButton(this);
    btnA->setGeometry(30,610,100,30);
    btnA->setText("A Say");

    QPushButton *btnB = new QPushButton(this);
    btnB->setGeometry(150,610,100,30);
    btnB->setText("B Say");

    QPushButton *switchBtn = new QPushButton(this);
    switchBtn->setGeometry(270,610,100,30);
    switchBtn->setText("Switch");

    m_editBox = new QLineEdit(this);
    m_editBox->setGeometry(30,650,450,30);

    connect(btnA,SIGNAL(clicked(bool)),this,SLOT(onASay()));
    connect(btnB,SIGNAL(clicked(bool)),this,SLOT(onBSay()));
    connect(switchBtn,SIGNAL(clicked(bool)),this,SLOT(onSwitch()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onASay()
{
    static int type = 1;
    QString str = m_editBox->text();
    QString speaker = "Tom(69600001)";
    QString cmd = QString("AddTalkContentEx(0,'%1',%2,'%3')").arg(str).arg(type%2).arg(speaker);
    m_webView->page()->mainFrame()->evaluateJavaScript(cmd);
    type++;
    qDebug()<<cmd;
    //QVariant f1result = m_webView->page()->mainFrame()->evaluateJavaScript(QString("AddTalkContent(0,'%1')").arg(str));
    //qDebug()<<f1result.toString();
}

void MainWindow::onBSay()
{
    static int type = 1;
    QString str = m_editBox->text();
    QString speaker = "Tom(69600001)";
    QString cmd = QString("AddTalkContentEx(1,'%1',%2,'%3')").arg(str).arg(type%2).arg(speaker);
    m_webView->page()->mainFrame()->evaluateJavaScript(cmd);
    type++;
    //m_webView->page()->mainFrame()->evaluateJavaScript(QString("AddTalkContent(1,'%1')").arg(str));
}

void MainWindow::webDocLoadFinished(bool)
{
    m_webView->page()->mainFrame()->addToJavaScriptWindowObject("chathost",this);
}

void MainWindow::webCallBackSlot(int type,const QString &text)
{
    qDebug()<<"recv web callback "<< type <<" desc:"<< text;
}

void MainWindow::onSwitch()
{
    static int type = 1;
    m_webView->page()->mainFrame()->evaluateJavaScript(QString("ShowTalkContent(%1)").arg(type%2));
    type++;
}
