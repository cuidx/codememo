#ifndef DRAWOBJECT_H
#define DRAWOBJECT_H

#include <QPoint>
#include <QPainter>
#include <list>

class DrawObject
{
public:
    DrawObject();
    virtual ~DrawObject() {}

public:
    virtual void OnMouseLButtonDown(QPoint &pos,QPainter &painter) = 0;
    virtual void OnMouseLButtonUp(QPoint &pos,QPainter &painter) = 0;
    virtual void OnMouseMove(QPoint &pos,QPainter &painter) = 0;
    virtual void OnMouseLButtonDoubleClick(QPoint &pos,QPainter &painter) = 0;

};

class DrawLine : public DrawObject
{
public:
    DrawLine();
    virtual ~DrawLine();

public:
    virtual void OnMouseLButtonDown(QPoint &pos,QPainter &painter);
    virtual void OnMouseLButtonUp(QPoint &pos,QPainter &painter);
    virtual void OnMouseMove(QPoint &pos,QPainter &painter);
    virtual void OnMouseLButtonDoubleClick(QPoint &pos,QPainter &painter);

private:
    QPoint  m_ptStart;
    bool    m_bLbtnDown;
};

class DrawCurve : public DrawObject
{
public:
    DrawCurve();
    virtual ~DrawCurve();

public:
    virtual void OnMouseLButtonDown(QPoint &pos,QPainter &painter);
    virtual void OnMouseLButtonUp(QPoint &pos,QPainter &painter);
    virtual void OnMouseMove(QPoint &pos,QPainter &painter);
    virtual void OnMouseLButtonDoubleClick(QPoint &pos,QPainter &painter);

private:
    QPoint  m_ptLastPos;
    bool    m_bLbtnDown;
};

class DrawPolygon : public DrawObject
{
public:
    DrawPolygon();
    virtual ~DrawPolygon();

public:
    virtual void OnMouseLButtonDown(QPoint &pos,QPainter &painter);
    virtual void OnMouseLButtonUp(QPoint &pos,QPainter &painter);
    virtual void OnMouseMove(QPoint &pos,QPainter &painter);
    virtual void OnMouseLButtonDoubleClick(QPoint &pos,QPainter &painter);

private:
    std::list<QPoint>   m_ptList;
    bool                m_bLbtnDown;
    bool                m_bDrawEnd;
};

class DrawRect : public DrawObject
{
public:
    DrawRect();
    virtual ~DrawRect();

public:
    virtual void OnMouseLButtonDown(QPoint &pos,QPainter &painter);
    virtual void OnMouseLButtonUp(QPoint &pos,QPainter &painter);
    virtual void OnMouseMove(QPoint &pos,QPainter &painter);
    virtual void OnMouseLButtonDoubleClick(QPoint &pos,QPainter &painter);

private:
    QPoint              m_ptStart;
    bool                m_bLbtnDown;
};

class DrawEllipse : public DrawObject
{
public:
    DrawEllipse();
    virtual ~DrawEllipse();

public:
    virtual void OnMouseLButtonDown(QPoint &pos,QPainter &painter);
    virtual void OnMouseLButtonUp(QPoint &pos,QPainter &painter);
    virtual void OnMouseMove(QPoint &pos,QPainter &painter);
    virtual void OnMouseLButtonDoubleClick(QPoint &pos,QPainter &painter);

private:
    QPoint              m_ptStart;
    bool                m_bLbtnDown;
};

#endif // DRAWOBJECT_H
