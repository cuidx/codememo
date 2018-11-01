#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebView>
#include <QLineEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void webCallBackSlot(int type,const QString &text);

protected slots:
    void onASay();
    void onBSay();
    void webDocLoadFinished(bool);
    void onSwitch();

private:
    Ui::MainWindow *ui;
    QWebView * m_webView;
    QLineEdit * m_editBox;
};

#endif // MAINWINDOW_H
