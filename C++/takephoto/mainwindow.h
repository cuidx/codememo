#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "camrainput.h"
#include <QLabel>
#include <QImage>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static void FrameCallBack(unsigned char* data, unsigned int len,int width,int height,int format,void* userdata);
    void OnGetFrame(unsigned char* data, unsigned int len,int width,int height,int format);

protected slots:
    void onCamraDevChange(QString text);
    void onCapture();
    void onPreview();
    void onCapImag(QImage pic);

protected:
    void paintEvent(QPaintEvent* event);

signals:
    void onCapImgSignal(QImage pic);

private:
    Ui::MainWindow *ui;
    CamraInput* m_camIput;
    QLabel* m_img;
    QImage m_pic;
    bool m_bCapImg;
};

#endif // MAINWINDOW_H
