#include "playlist.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QDebug>

PlayList::PlayList()
{
    m_playlistSource = NULL;
}

PlayList::~PlayList()
{
    if (m_playlistSource)
    {
        delete m_playlistSource;
        m_playlistSource = NULL;
    }
}

bool PlayList::LoadPlayList(const std::string &filename)
{
    bool bret = false;
    if (m_playlistSource == NULL)
    {
        m_playlistSource = new PlayListFileSource(filename);
    }
    if (m_playlistSource)
    {
        m_objectList.clear();
        bret = m_playlistSource->LoadPlayList(m_objectList);
    }
    return bret;
}

bool PlayList::SavePlayList()
{
    bool bret = false;
    if (m_playlistSource)
    {
        bret = m_playlistSource->SavePlayList(m_objectList);
    }
    return bret;
}

int PlayList::GetCount()
{
    return m_objectList.size();
}

bool PlayList::AddObject(const ObjectInfo &obj)
{
    bool bFind = false;
    std::list<ObjectInfo>::iterator iter = m_objectList.begin();
    for (; iter != m_objectList.end(); iter++)
    {
        if (iter->Key() == obj.Key())
        {
            bFind = true;
            *iter = obj;
            break;
        }
    }
    if (!bFind)
    {
        m_objectList.push_back(obj);
    }
    return true;
}

bool PlayList::RemoveObject(const ObjectInfo &obj)
{
    std::list<ObjectInfo>::iterator iter = m_objectList.begin();
    for (; iter != m_objectList.end(); iter++)
    {
        if (iter->Key() == obj.Key())
        {
            m_objectList.erase(iter);
            break;
        }
    }
    return true;
}

PlayList::PlayListIterator PlayList::begin()
{
    return m_objectList.begin();
}

PlayList::PlayListIterator PlayList::end()
{
    return m_objectList.end();
}

bool PlayListFileSource::LoadPlayList(std::list<ObjectInfo> &playlist)
{
    if (m_filename.empty())
    {
        qDebug() << "LoadPlayList fail, filename is empty";
        return false;
    }

    QFile file(QString::fromStdString(m_filename));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Load PlayList fail, open file fail";
        return false;
    }
    QXmlStreamReader reader(&file);

    while (!reader.atEnd())
    {
        if (reader.readNextStartElement())
        {
            QString strElementName = reader.name().toString();
            if (strElementName.compare("PlayList") == 0)
            {
                qDebug()<<QString("Read PlayList Start");
            }
            else if (strElementName.compare("item") == 0)
            {
                ObjectInfo item;
                QXmlStreamAttributes::Iterator iter = reader.attributes().begin();
                for (; iter != reader.attributes().end(); iter++)
                {
                    QString attName = iter->name().toString();
                    if (attName.compare("title") == 0)
                    {
                        item.m_title = iter->value().toString().toStdString();
                    }
                    else if (attName.compare("picPath") == 0)
                    {
                        item.m_picPath = iter->value().toString().toStdString();
                    }
                    else if (attName.compare("soundPath") == 0)
                    {
                        item.m_soundPath = iter->value().toString().toStdString();
                    }
                }
                playlist.push_back(item);
                reader.skipCurrentElement();
            }
        }
        else
        {
            break;
        }
    }

    file.close();

    return true;
}

bool PlayListFileSource::SavePlayList(const std::list<ObjectInfo> &playlist)
{
    if (m_filename.empty())
    {
        return false;
    }

    QFile file(QString::fromStdString(m_filename));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument("1.0",true);
    writer.writeStartElement("PlayList");

    std::list<ObjectInfo>::const_iterator iter = playlist.begin();
    for (; iter != playlist.end(); iter++)
    {
        writer.writeStartElement("item");
        writer.writeAttribute("title",QString::fromLocal8Bit(iter->m_title.c_str()));
        writer.writeAttribute("picPath",QString::fromLocal8Bit(iter->m_picPath.c_str()));
        writer.writeAttribute("soundPath",QString::fromLocal8Bit(iter->m_soundPath.c_str()));
        writer.writeEndElement();
    }

    writer.writeEndElement();
    writer.writeEndDocument();

    file.close();

    return true;
}
