#include "objectview.h"
#include <QPixmap>
#include "soundplayer.h"

ObjectView::ObjectView(QWidget *parent) : QWidget(parent)
{
    m_bgPic = new QLabel(this);
    m_playBtn = new QPushButton(this);
    m_playBtn->setStyleSheet(QString::fromUtf8("QPushButton{border:none;background-color:rgba(0,0,0,0.01);}"));
    m_playBtn->resize(size());
    m_playBtn->raise();
    connect(m_playBtn,SIGNAL(clicked(bool)),this,SLOT(onPlay()));
}

bool ObjectView::SetResource(const QString &picPath,const QString &soundPath,const QString &text)
{
    m_picPath = picPath;
    m_soundPath = soundPath;
    m_text = text;
    if (m_bgPic)
    {
       QPixmap pic(picPath);
       m_bgPic->setPixmap(pic);
       m_bgPic->resize(size());
    }
    if (m_playBtn)
    {
        m_playBtn->resize(size());
    }
    return true;
}

void ObjectView::onPlay()
{
    if (!m_soundPath.isEmpty())
    {
        SoundPlayer::Instance()->PlayFile(m_soundPath);
    }
    else if(!m_text.isEmpty())
    {
        SoundPlayer::Instance()->PlayText(m_text.toStdString().c_str());
    }
}
