#ifndef MP3FILEDECODER_H
#define MP3FILEDECODER_H

#include <string>

struct wavedata;

class MP3FileDecoder
{
public:
    MP3FileDecoder();
    virtual ~MP3FileDecoder();

public:
    int LoadMP3File(const char* filename,struct wavedata *data);

private:
    std::string m_filename;
};

#endif // MP3FILEDECODER_H

