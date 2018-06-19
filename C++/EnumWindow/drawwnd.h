#ifndef DRAWWND_H
#define DRAWWND_H

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QPen>
#include "drawobject.h"

enum DRAW_TYPE
{
    DT_NONE     = 0,
    DT_LINE     = 1,
    DT_CURVE    = 2,
    DT_POLYGON  = 3,
    DT_RECT     = 4,
    DT_ELLIPSE  = 5,
};

class DrawWnd : public QWidget
{
    Q_OBJECT
public:
    explicit DrawWnd(QWidget *parent = nullptr);

    int SetDrawType(DRAW_TYPE type);
    DrawObject* CreateDrawObject(int type);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);

signals:

public slots:

private:
    bool    m_bLbtnDown;
    QPoint  m_ptLbtnDown;
    QLabel  *m_bg;
    QPixmap *m_pixmap;
    QPixmap *m_tmppix;
    QPainter *m_painter;
    QPainter *m_tmppainter;
    QPen    *m_pen;
    DrawObject *m_drawObj;
    int     m_drawType;
};

#endif // DRAWWND_H
