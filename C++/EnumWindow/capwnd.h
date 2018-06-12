#ifndef CAPWND_H
#define CAPWND_H

#include <QWidget>

class CapWnd : public QWidget
{
    Q_OBJECT
public:
    explicit CapWnd(QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *event);

signals:

public slots:
};

#endif // CAPWND_H
