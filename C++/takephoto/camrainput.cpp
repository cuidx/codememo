#include "camrainput.h"
#include <condition_variable>
#include <mutex>
using namespace std::chrono_literals;
#include <QDebug>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
}

CamraInput::CamraInput()
{
    m_callBack = NULL;
    m_userdata = NULL;
}

CamraInput::~CamraInput()
{

}

bool CamraInput::Init(FrameDataCallBack callback,void *userData)
{
    m_callBack = callback;
    m_userdata = userData;
    av_register_all();
    avdevice_register_all();

    return true;
}

void CamraInput::UnInit()
{

}

int CamraInput::GetCamraDevList(std::list<std::string> &devs)
{
    AVInputFormat *pDevice = NULL;
    pDevice = av_input_video_device_next(pDevice);
    while (NULL != pDevice)
    {
        //QString name = QString::fromUtf8(pDevice->name);
        devs.push_back(pDevice->long_name);
        pDevice = av_input_video_device_next(pDevice);
    }
    return 0;
}

int CamraInput::SetDefaultCamra(const std::string &camraName)
{
    m_camraName = camraName;
    return 0;
}

int CamraInput::StartCapture()
{
    m_captureThread = std::thread(&CamraInput::capThread,(void*)this);
    return 0;
}

int CamraInput::StopCapture()
{
    return 0;
}

int CamraInput::OpenCamra(std::string &camraName)
{
    return 0;
}

int CamraInput::CloseCamra()
{
    return 0;
}

void CamraInput::capThread(void* param)
{
    CamraInput* pThis = (CamraInput*)param;
    if (pThis)
    {
        AVInputFormat *ifmt = av_find_input_format("video4linux2");
        AVFormatContext* fmtCtx = NULL;
        if (avformat_open_input(&fmtCtx,"/dev/video0",ifmt,NULL) != 0)
        {
            qDebug() << "avformat_open_input fail";
            return;
        }

        if (avformat_find_stream_info(fmtCtx,NULL) < 0)
        {
            qDebug() << "avformat_find_stream_info fail";
            return;
        }

        int videoIndex = -1;
        for (int i = 0; i < fmtCtx->nb_streams; i++)
        {
            if (fmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                videoIndex = i;
                break;
            }
        }

        if (videoIndex == -1)
        {
            qDebug() << "could not find a video stream";
            return;
        }

        AVCodecContext	*pCodecCtx = fmtCtx->streams[videoIndex]->codec;
        AVCodec			*pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
        if (NULL == pCodec)
        {
            qDebug() << "decoder not found";
            return;
        }

        if (avcodec_open2(pCodecCtx,pCodec,NULL) < 0)
        {
            qDebug() << "open codec fail";
            return;
        }

        AVFrame	*pFrame,*pFrameRGB;
        pFrame=av_frame_alloc();
        pFrameRGB = av_frame_alloc();
        pFrameRGB->format = AV_PIX_FMT_RGB24;
        int numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width,pCodecCtx->height);
        uint8_t* rgbData = (uint8_t*)av_malloc(numBytes);
        avpicture_fill((AVPicture*)pFrameRGB,rgbData,AV_PIX_FMT_RGB24, pCodecCtx->width,pCodecCtx->height);
        AVPacket *packet=(AVPacket *)av_malloc(sizeof(AVPacket));
        std::condition_variable cv;
        std::mutex mtex;
        struct SwsContext *img_convert_ctx;
        img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                                         pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
                                         AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

        for (;;)
        {
            if (av_read_frame(fmtCtx,packet) >= 0)
            {
                if (packet->stream_index == videoIndex)
                {
                    int get_pic = 0;
                    int ret = avcodec_decode_video2(pCodecCtx,pFrame,&get_pic,packet);
                    if (ret < 0)
                    {
                        qDebug() << "avcodec_decode_video2 fail";
                        return;
                    }
                    ret = sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0,
                              pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
                    //qDebug() <<"get frame w=" <<pFrame->width<<", h="<<pFrame->height<<", size="<< pFrame->format<<"rgblinesize="<<pFrameRGB->linesize[0] << "ret="<<ret;
                    if (pThis->m_callBack)
                    {
                        pThis->m_callBack((uint8_t*)rgbData,pFrameRGB->pkt_size,pFrame->width,pFrame->height,pFrameRGB->format,pThis->m_userdata);
                    }
                }
            }
            av_free_packet(packet);
            {
                std::unique_lock<std::mutex> lk(mtex);
                cv.wait_for(lk,40ms);
            }
        }

    }
}
