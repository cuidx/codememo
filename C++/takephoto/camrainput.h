#ifndef CAMRAINPUT_H
#define CAMRAINPUT_H

#include <list>
#include <string>
#include <thread>

typedef void (*FrameDataCallBack)(unsigned char* data, unsigned int len,int width,int height,int format,void* userdata);

class CamraInput
{
public:
    CamraInput();
    virtual ~CamraInput();

public:
    bool Init(FrameDataCallBack callback,void *userData);
    void UnInit();

    int GetCamraDevList(std::list<std::string> &devs);

    int SetDefaultCamra(const std::string &camraName);

    int StartCapture();

    int StopCapture();

    static void capThread(void* param);

protected:
    int OpenCamra(std::string &camraName);
    int CloseCamra();

private:
    std::string m_camraName;
    std::thread m_captureThread;
    FrameDataCallBack m_callBack;
    void*   m_userdata;
};

#endif // CAMRAINPUT_H

