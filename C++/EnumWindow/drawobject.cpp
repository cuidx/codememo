#include "drawobject.h"
#include <QDebug>

DrawObject::DrawObject()
{

}

DrawLine::DrawLine()
{
    m_bLbtnDown = false;
}

DrawLine::~DrawLine()
{

}

void DrawLine::OnMouseLButtonDoubleClick(QPoint &pos, QPainter &painter)
{

}

void DrawLine::OnMouseLButtonDown(QPoint &pos, QPainter &painter)
{
    m_bLbtnDown = true;
    m_ptStart = pos;
}

void DrawLine::OnMouseLButtonUp(QPoint &pos, QPainter &painter)
{
    if (m_bLbtnDown)
    {
        painter.drawLine(m_ptStart,pos);
        m_bLbtnDown = false;
    }
}

void DrawLine::OnMouseMove(QPoint &pos, QPainter &painter)
{
    if (m_bLbtnDown)
    {
        painter.drawLine(m_ptStart,pos);
    }
}

DrawCurve::DrawCurve()
{
    m_bLbtnDown = false;
}

DrawCurve::~DrawCurve()
{

}

void DrawCurve::OnMouseLButtonDoubleClick(QPoint &pos, QPainter &painter)
{

}

void DrawCurve::OnMouseLButtonDown(QPoint &pos, QPainter &painter)
{
    m_bLbtnDown = true;
    m_ptLastPos = pos;
}

void DrawCurve::OnMouseLButtonUp(QPoint &pos, QPainter &painter)
{
    if (m_bLbtnDown)
    {
        painter.drawLine(m_ptLastPos,pos);
        m_bLbtnDown = false;
    }
}

void DrawCurve::OnMouseMove(QPoint &pos, QPainter &painter)
{
    if (m_bLbtnDown)
    {
        painter.drawLine(m_ptLastPos,pos);
        m_ptLastPos = pos;
    }
}

DrawPolygon::DrawPolygon()
{
    m_bLbtnDown = false;
    m_bDrawEnd = false;
}

DrawPolygon::~DrawPolygon()
{

}

void DrawPolygon::OnMouseLButtonDoubleClick(QPoint &pos, QPainter &painter)
{
    if (m_ptList.size() >= 2 && !m_bDrawEnd)
    {
        QPoint ptLast = *m_ptList.rbegin();
        QPoint ptFirst = *m_ptList.begin();
        painter.drawLine(ptLast,pos);
        painter.drawLine(pos,ptFirst);
        m_bDrawEnd = true;
        qDebug()<<"DrawPolygon::OnMouseLButtonDoubleClick ("<<ptLast.x()<<","<<ptLast.y()<<") ("<<ptFirst.x()<<","
               <<ptFirst.y()<<") ("<<pos.x()<<","<<pos.y()<<")"<<endl;
    }
    qDebug()<<"DrawPolygon::OnMouseLButtonDoubleClick "<<m_ptList.size()<<endl;
}

void DrawPolygon::OnMouseLButtonDown(QPoint &pos, QPainter &painter)
{
    if (m_bDrawEnd)
    {
        m_bDrawEnd = false;
        m_ptList.clear();
    }
    if (m_ptList.size() == 0)
    {
        m_ptList.push_back(pos);
    }
    else if (m_ptList.size() == 1)
    {
        m_ptList.clear();
        m_ptList.push_back(pos);
    }
    else if (m_ptList.size() > 2)
    {
        QPoint ptLast = *m_ptList.rbegin();
        painter.drawLine(ptLast,pos);
    }
    m_bLbtnDown = true;
}

void DrawPolygon::OnMouseLButtonUp(QPoint &pos, QPainter &painter)
{
    if (m_bLbtnDown)
    {
        m_bLbtnDown = false;
        if (m_ptList.size() == 1)
        {
            QPoint ptLast = *m_ptList.rbegin();
            QPoint ptDis = pos - ptLast;
            int len = ptDis.x() * ptDis.x() + ptDis.y() * ptDis.y();
            if (len > 100)
            {
                m_ptList.push_back(pos);
                painter.drawLine(ptLast,pos);
            }
            else
            {
                m_ptList.clear();
            }
        }
        else if (m_ptList.size() >= 2)
        {
            QPoint ptLast = *m_ptList.rbegin();
            if (ptLast != pos)
            {
                m_ptList.push_back(pos);
                painter.drawLine(ptLast,pos);
            }
        }
    }
}

void DrawPolygon::OnMouseMove(QPoint &pos, QPainter &painter)
{
    if (m_bLbtnDown && m_ptList.size() > 0)
    {
        QPoint ptLast = *m_ptList.rbegin();
        painter.drawLine(ptLast,pos);
    }
}

DrawRect::DrawRect()
{
    m_bLbtnDown = false;
}

DrawRect::~DrawRect()
{

}

void DrawRect::OnMouseLButtonDoubleClick(QPoint &pos, QPainter &painter)
{

}

void DrawRect::OnMouseLButtonDown(QPoint &pos, QPainter &painter)
{
    m_bLbtnDown = true;
    m_ptStart = pos;
}

void DrawRect::OnMouseLButtonUp(QPoint &pos, QPainter &painter)
{
    if (m_bLbtnDown)
    {
        m_bLbtnDown = false;
        if (pos != m_ptStart)
        {
            painter.drawRect(QRect(m_ptStart,pos));
        }
    }
}

void DrawRect::OnMouseMove(QPoint &pos, QPainter &painter)
{
    if (m_bLbtnDown)
    {
        painter.drawRect(QRect(m_ptStart,pos));
    }
}

DrawEllipse::DrawEllipse()
{
    m_bLbtnDown = false;
}

DrawEllipse::~DrawEllipse()
{

}

void DrawEllipse::OnMouseLButtonDoubleClick(QPoint &pos, QPainter &painter)
{

}

void DrawEllipse::OnMouseLButtonDown(QPoint &pos, QPainter &painter)
{
    m_bLbtnDown = true;
    m_ptStart = pos;
}

void DrawEllipse::OnMouseLButtonUp(QPoint &pos, QPainter &painter)
{
    if (m_bLbtnDown)
    {
        m_bLbtnDown = false;
        if (pos != m_ptStart)
        {
            QPoint ptCenter = pos + m_ptStart;
            ptCenter = ptCenter / 2;
            int rx = (pos.x() - m_ptStart.x()) / 2;
            if (rx < 0)
            {
                rx = -rx;
            }
            int ry = (pos.y() - m_ptStart.y()) / 2;
            if (ry < 0)
            {
                ry = -ry;
            }
            painter.drawEllipse(ptCenter,rx,ry);
        }
    }
}

void DrawEllipse::OnMouseMove(QPoint &pos, QPainter &painter)
{
    if (m_bLbtnDown)
    {
        if (pos != m_ptStart)
        {
            QPoint ptCenter = pos + m_ptStart;
            ptCenter = ptCenter / 2;
            int rx = (pos.x() - m_ptStart.x()) / 2;
            if (rx < 0)
            {
                rx = -rx;
            }
            int ry = (pos.y() - m_ptStart.y()) / 2;
            if (ry < 0)
            {
                ry = -ry;
            }
            painter.drawEllipse(ptCenter,rx,ry);
        }
    }
}
