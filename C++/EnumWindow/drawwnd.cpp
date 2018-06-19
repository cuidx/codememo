#include "drawwnd.h"
#include <QPainter>

DrawWnd::DrawWnd(QWidget *parent) : QWidget(parent),m_bLbtnDown(false)
{
    //this->setWindowFlags(Qt::SubWindow);
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    //this->setAttribute(Qt::WA_TransparentForMouseEvents,false);
    //this->setMouseTracking(true);
    //this->setAttribute(Qt::WA_AlwaysStackOnTop,true);
    setAttribute(Qt::WA_TranslucentBackground,true);
    //this->setStyleSheet(QString("background:transparent;"));
    setAutoFillBackground(false);
//    m_bg = new QLabel(this);
//    m_bg->setStyleSheet(QString("background-color:rgba(0,0,0,1);"));
//    //m_bg->setAttribute(Qt::WA_TranslucentBackground,true);
//    m_bg->setGeometry(0,0,width(),height());
//    m_bg->show();

    m_pixmap = new QPixmap(1920,1080);
    m_pixmap->fill(QColor(0,0,0,1));

    m_tmppix = new QPixmap(1920,1080);
    m_tmppix->fill(QColor(0,0,0,1));

    m_painter = new QPainter(m_pixmap);
    m_tmppainter = new QPainter(m_tmppix);

    m_pen = new QPen();
    m_pen->setWidth(4);
    m_pen->setColor(QColor(Qt::red));

    m_painter->setPen(*m_pen);
    m_tmppainter->setPen(*m_pen);

    m_drawType = DT_NONE;
    m_drawObj = NULL;

    //SetDrawType(DT_LINE);
    //SetDrawType(DT_CURVE);
    //SetDrawType(DT_POLYGON);
    //SetDrawType(DT_RECT);
    SetDrawType(DT_ELLIPSE);
}

void DrawWnd::mousePressEvent(QMouseEvent *event)
{
    m_bLbtnDown = true;
#if 0
    m_ptLbtnDown = event->pos();

#else
    if (m_drawObj)
    {
        m_drawObj->OnMouseLButtonDown(event->pos(),*m_tmppainter);
    }
#endif
}

void DrawWnd::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_bLbtnDown)
    {
#if 0
        QPainter painter(m_pixmap);
        QPen pen;
        pen.setWidth(4);
        pen.setColor(QColor(Qt::red));
        painter.setPen(pen);
        painter.drawLine(m_ptLbtnDown,event->pos());
        m_ptLbtnDown = event->pos();
#else
        if (m_drawObj)
        {
            m_drawObj->OnMouseLButtonUp(event->pos(),*m_painter);
        }
#endif

        m_bLbtnDown = false;

        update();
    }
}

void DrawWnd::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (m_drawObj)
    {
        m_drawObj->OnMouseLButtonDoubleClick(event->pos(),*m_painter);

        update();
    }
}

void DrawWnd::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bLbtnDown)
    {
#if 0
        m_tmppix->fill(QColor(0,0,0,1));
        QPainter painter(m_tmppix);
        QPen pen;
        pen.setWidth(4);
        pen.setColor(QColor(Qt::red));
        painter.setPen(pen);
        painter.drawLine(m_ptLbtnDown,event->pos());
        //m_ptLbtnDown = event->pos();
#else
        if (m_drawObj)
        {
            if (m_drawType != DT_CURVE)
            {
                m_tmppix->fill(QColor(0,0,0,1));
                m_drawObj->OnMouseMove(event->pos(),*m_tmppainter);
            }
            else
            {
                m_drawObj->OnMouseMove(event->pos(),*m_painter);
            }
        }
#endif
        update();
    }
}

void DrawWnd::resizeEvent(QResizeEvent *event)
{
    //m_bg->setGeometry(0,0,width(),height());
}

void DrawWnd::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
//    painter.fillRect(0, 0, width(), height(), QColor(0, 0, 0, 1));
//    QPen pen;
//    pen.setWidth(8);
//    pen.setColor(QColor(Qt::red));
//    painter.setPen(pen);
//    painter.drawLine(QPoint(0,0),QPoint(10,10));

    painter.drawPixmap(0,0,width(),height(),*m_pixmap,0,0,width(),height());

    if (m_bLbtnDown)
    {
        painter.drawPixmap(0,0,width(),height(),*m_tmppix,0,0,width(),height());
    }
}

int DrawWnd::SetDrawType(DRAW_TYPE type)
{
    if (type != m_drawType)
    {
        m_drawType = type;
        if (m_drawObj)
        {
            delete m_drawObj;
            m_drawObj = NULL;
        }
        m_drawObj = CreateDrawObject(type);
    }
    return 0;
}

DrawObject* DrawWnd::CreateDrawObject(int type)
{
    DrawObject* pDrawObj = NULL;
    switch (type)
    {
    case DT_LINE:
    {
        pDrawObj = new DrawLine();
        break;
    }
    case DT_CURVE:
    {
        pDrawObj = new DrawCurve();
        break;
    }
    case DT_POLYGON:
    {
        pDrawObj = new DrawPolygon();
        break;
    }
    case DT_RECT:
    {
        pDrawObj = new DrawRect();
        break;
    }
    case DT_ELLIPSE:
    {
        pDrawObj = new DrawEllipse();
        break;
    }
    }
    return pDrawObj;
}
