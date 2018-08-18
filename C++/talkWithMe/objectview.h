#ifndef OBJECTVIEW_H
#define OBJECTVIEW_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QPushButton>

class ObjectView : public QWidget
{
    Q_OBJECT
public:
    explicit ObjectView(QWidget *parent = 0);

    bool SetResource(const QString &picPath,const QString &soundPath);

signals:

public slots:
    void onPlay();

private:
    QPushButton* m_playBtn;
    QLabel*     m_bgPic;
    QString     m_picPath;
    QString     m_soundPath;
};

#endif // OBJECTVIEW_H
