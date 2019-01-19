#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <string>
#include <list>

struct ObjectInfo
{
    std::string     m_picPath;
    std::string     m_soundPath;
    std::string     m_title;

    std::string Key() const
    {
        return m_title;
    }
};

class PlayListDataSource
{
public:
    PlayListDataSource() {}
    virtual ~PlayListDataSource() {}

public:
    virtual bool LoadPlayList(std::list<ObjectInfo> &playlist) = 0;
    virtual bool SavePlayList(const std::list<ObjectInfo> &playlist) = 0;
};

class PlayListFileSource : public PlayListDataSource
{
public:
    PlayListFileSource(const std::string &filename)
    {
        m_filename = filename;
    }

    virtual ~PlayListFileSource() {}

public:
    virtual bool LoadPlayList(std::list<ObjectInfo> &playlist);
    virtual bool SavePlayList(const std::list<ObjectInfo> &playlist);

private:
    std::string     m_filename;
};

class PlayList
{
public:
    PlayList();
    virtual ~PlayList();

    typedef std::list<ObjectInfo>::iterator PlayListIterator;
    bool LoadPlayList(const std::string &filename);
    bool SavePlayList();

    int  GetCount();
    bool AddObject(const ObjectInfo &obj);
    bool RemoveObject(const ObjectInfo &obj);

    PlayListIterator begin();
    PlayListIterator end();

private:
    std::list<ObjectInfo>      m_objectList;
    PlayListDataSource*         m_playlistSource;
};

#endif // PLAYLIST_H
