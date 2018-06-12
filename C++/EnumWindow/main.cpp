#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    MainWindow::Instance()->show();
    a.exec();
    MainWindow::Destroy();
    return 0;
}
