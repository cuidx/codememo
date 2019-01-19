#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <playlist.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    PlayList* m_playList;
};

#endif // MAINWINDOW_H
