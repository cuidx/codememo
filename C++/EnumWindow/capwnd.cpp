#include "capwnd.h"
#include <QPainter>

CapWnd::CapWnd(QWidget *parent) : QWidget(parent)
{

    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    this->setAttribute(Qt::WA_AlwaysStackOnTop,true);
    setAttribute(Qt::WA_TranslucentBackground);

    m_pDrawWnd = new DrawWnd(NULL);
    m_pDrawWnd->setGeometry(geometry().left(),geometry().top(),this->geometry().width(),geometry().height());
    m_pDrawWnd->show();
}

void CapWnd::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect rcWin(0,0,this->geometry().width(),geometry().height());
    QPen pen;
    pen.setWidth(8);
    pen.setColor(QColor(Qt::red));
    painter.setPen(pen);
    painter.drawRect(rcWin);
}

void CapWnd::resizeEvent(QResizeEvent *event)
{
   // m_pDrawWnd->setGeometry(0,0,this->geometry().width(),geometry().height());
    QWidget::resizeEvent(event);

    //m_pDrawWnd->setGeometry(geometry().left(),geometry().top(),this->geometry().width(),geometry().height());
    m_pDrawWnd->move(geometry().left(),geometry().top());
    m_pDrawWnd->resize(this->geometry().width(),geometry().height());
}
