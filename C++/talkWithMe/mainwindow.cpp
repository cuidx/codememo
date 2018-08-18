#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "objectview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ObjectView *view = new ObjectView(this);
    view->setGeometry(rect());
    view->SetResource(QString(":/res/Hello.bmp"),QString(":/res/sample.wav"));
}

MainWindow::~MainWindow()
{
    delete ui;
}
