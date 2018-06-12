#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include "capwnd.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static MainWindow* Instance();
    static void Destroy();

    virtual bool event(QEvent *event);

    virtual bool winEvent(MSG *message, long *result);

    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);

    void moveCapWnd(HWND hCapWnd);

protected slots:
    void onStart(bool);
    void onStartCap(bool);

private:
    Ui::MainWindow *ui;
    static MainWindow* m_pInstance;
    unsigned int  m_notifyMsgId;
    QProcess m_x86msghook;
    CapWnd* m_pCapWnd;
    std::list<HWND> m_winHwnds;
    HWND m_hCapWnd;
};

#endif // MAINWINDOW_H
