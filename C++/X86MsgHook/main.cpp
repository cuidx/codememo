#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (argc < 2)
    {

        return -1;
    }
    unsigned long long hwnd = atoll(argv[1]);
    MainWindow w;
    //w.show();
    w.hide();
    w.InstallMsgHook((HWND)hwnd);

    return a.exec();
}
