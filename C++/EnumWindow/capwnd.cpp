#include "capwnd.h"
#include <QPainter>

CapWnd::CapWnd(QWidget *parent) : QWidget(parent)
{

    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    this->setAttribute(Qt::WA_AlwaysStackOnTop,true);
    setAttribute(Qt::WA_TranslucentBackground);
}

void CapWnd::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect rcWin(0,0,this->geometry().width(),geometry().height());
    //rcWin = rcWin - rcWin.topLeft();
    //rcWin.setWidth(rcWin.width() - 4);
    QPen pen;
    pen.setWidth(8);
    pen.setColor(QColor(Qt::red));
    painter.setPen(pen);
    painter.drawRect(rcWin);
}
