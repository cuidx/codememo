#ifndef CAPWND_H
#define CAPWND_H

#include <QWidget>
#include "drawwnd.h"

class CapWnd : public QWidget
{
    Q_OBJECT
public:
    explicit CapWnd(QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
signals:

public slots:

private:
    DrawWnd*        m_pDrawWnd;
};

#endif // CAPWND_H
