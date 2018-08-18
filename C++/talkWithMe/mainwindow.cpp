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
    view->SetResource(QString(":/res/Hello.bmp"),QString("C:/cuidx/SDL2-2.0.8/SDL2-2.0.8/test/sample.wav"));
}

MainWindow::~MainWindow()
{
    delete ui;
}
