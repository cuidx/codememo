#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../msgHook/msghook.h"

#pragma comment(lib, "msgHook.lib")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    UnInstallCbtHook();
    delete ui;
}

bool MainWindow::InstallMsgHook(HWND hwnd)
{
    unsigned int m_notifyMsgId = InstallCbtHook(hwnd);
    return true;
}
