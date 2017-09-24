// device.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define WEBRTC_WIN

#include "talk\media\devices\devicemanager.h"
#include "talk\media\base\videocapturer.h"
#include "talk\media\devices\videorendererfactory.h"
#include "talk\media\webrtc\webrtcvideoframe.h"

//#define BUILDING_LIBICONV_LIB
#include "third_party/libiconv/iconv.h"

using namespace cricket;

#pragma comment(lib,"iconv.lib")

#pragma comment(lib,"Ws2_32.lib")

#pragma comment(lib,"libjpeg.lib")
#pragma comment(lib,"libyuv.lib")
#pragma comment(lib,"directshow_baseclasses.lib")
#pragma comment(lib,"system_wrappers.lib")
#pragma comment(lib,"common_video.lib")
#pragma comment(lib,"webrtc_base.lib")
#pragma comment(lib,"video_capture_module.lib")
#pragma comment(lib,"libjingle_media.lib")

//#pragma comment(lib,"webrtc.lib")
//#pragma comment(lib,"webrtc_common.lib")
//#pragma comment(lib,"video_engine_core.lib")
//#pragma comment(lib,"webrtc_i420.lib")


std::string Utf8ToGBK(std::string utf8str)
{
	std::string sOut;
	iconv_t cd = iconv_open("gb2312", "utf-8");
	if (NULL != cd)
	{
		size_t inBufSize = utf8str.length() + 1;
		size_t outBufSize = inBufSize * 2;
		char *outBuf = new char[outBufSize];
		if (outBuf != NULL)
		{
			memset(outBuf, 0, outBufSize);
			const char *inBuf = utf8str.data();
			char *pOut = outBuf;
			size_t nSize = iconv(cd, &inBuf, &inBufSize, &pOut, &outBufSize);
			if (nSize != -1)
			{
				sOut = outBuf;
			}
			
			delete []outBuf;
			outBuf = NULL;
		}

		iconv_close(cd);
	}
	return sOut;
}

class MyFrameFactory : public VideoFrameFactory, public sigslot::has_slots<>
{
public:
	MyFrameFactory() 
	{
		m_pVideoRender = NULL;
	}
	virtual ~MyFrameFactory() 
	{
		printf("\nMyFrameFactory destructed\n");
	}

	void ResetVideoRender(VideoRenderer* pVideoRender)
	{
		m_pVideoRender = pVideoRender;
	}

	// The returned frame aliases the aliased_frame if the input color
	// space allows for aliasing, otherwise a color conversion will
	// occur.  For safety, |input_frame| must outlive the returned
	// frame.  Returns NULL if conversion fails.
	virtual VideoFrame* CreateAliasedFrame(
		const CapturedFrame* input_frame, int width, int height) const
	{
		//这个函数貌似没有回调，还没研究这个怎么用
		printf("\nGetVideoFrame from callback:w=%d,h=%d\n",width,height);
		return NULL;
	}

	void OnFrameCaptured(VideoCapturer* video_capturer,
		const CapturedFrame* captured_frame)
	{
		if (captured_frame)
		{
			printf("\nGetVideoFrame from callback,width=%d,height=%d,pix_width=%d,pix_height=%d,datasize=%u,fourcc=%x,timestamp=%lld\n", 
				captured_frame->width,captured_frame->height,
				captured_frame->pixel_width, captured_frame->pixel_height,
				captured_frame->data_size,
				captured_frame->fourcc, captured_frame->time_stamp);

			if (m_pVideoRender)
			{
				WebRtcVideoFrame frame;
				frame.Init(captured_frame, captured_frame->width, captured_frame->height);
				m_pVideoRender->RenderFrame(&frame);
			}
		}
		
	}

private:
	VideoRenderer* m_pVideoRender;
};

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "chs");
	DeviceManagerInterface *pDevManager = DeviceManagerFactory::Create();
	if (pDevManager != NULL)
	{
		bool bRet = pDevManager->Init();
		if (!bRet)
		{
			printf("deviceManager Init fail\n");
			return -1;
		}

		printf("\nGet Audio Input devices:\n");
		std::vector<Device> audioInDevs;
		if (!pDevManager->GetAudioInputDevices(&audioInDevs))
		{
			printf("Get AudioInDevices fail\n");
		}
		std::vector<Device>::iterator itAudioIn = audioInDevs.begin();
		for (; itAudioIn != audioInDevs.end(); itAudioIn++)
		{
			printf("AudioInput Dev[id:%s]:%s\n", itAudioIn->id.c_str(), Utf8ToGBK(itAudioIn->name).c_str());
		}

		printf("\nGet Audio Output devices:\n");
		std::vector<Device> audioOutDevs;
		if (!pDevManager->GetAudioOutputDevices(&audioOutDevs))
		{
			printf("Get GetAudioOutputDevices fail\n");
		}
		std::vector<Device>::iterator itAudioOut = audioOutDevs.begin();
		for (; itAudioOut != audioOutDevs.end(); itAudioOut++)
		{
			printf("AudioOutput Dev[id:%s]:%s\n", itAudioOut->id.c_str(), Utf8ToGBK(itAudioOut->name).c_str());
		}

		printf("\nGet Video Capture Devices:\n");
		std::vector<Device> videoCapDevs;
		if (!pDevManager->GetVideoCaptureDevices(&videoCapDevs))
		{
			printf("Get Video Capture Devices fail\n");
		}
		std::vector<Device>::iterator itVideoCap = videoCapDevs.begin();
		for (; itVideoCap != videoCapDevs.end(); itVideoCap++)
		{
			printf("VideoCap Dev[id:%s]:%s\n", itVideoCap->id.c_str(), Utf8ToGBK(itVideoCap->name).c_str());
		}

		int capbilities = pDevManager->GetCapabilities();
		printf("\nwebrtc-media capbilities:%d\n",capbilities);

		printf("\nGet Desktops:\n");
		std::vector<rtc::DesktopDescription> desktops;
		if (!pDevManager->GetDesktops(&desktops))
		{
			printf("GetDesktops fail\n");
		}
		std::vector<rtc::DesktopDescription>::iterator itDesk = desktops.begin();
		for (; itDesk != desktops.end(); itDesk++)
		{
			printf("Desktop:%s[id:%d][isPrimary:%d]\n", (itDesk->title()).c_str(),(itDesk->id()).index(),itDesk->primary() ? 1 : 0);
		}

		printf("\nGet Windows:\n");
		std::vector<rtc::WindowDescription> wins;
		if (!pDevManager->GetWindows(&wins))
		{
			printf("GetWindows fail\n");
		}
		std::vector<rtc::WindowDescription>::iterator itWin = wins.begin();
		for (; itWin != wins.end(); itWin++)
		{
			printf("Window:[title:%s, id:%d]\n",Utf8ToGBK(itWin->title()).c_str(), itWin->id().id());
		}

		printf("\nTest Videocapture\n");
		Device camDev;
		std::string camName;
		if (pDevManager->GetVideoCaptureDevice(camName, &camDev))
		{
			VideoCapturer *camCapture = pDevManager->CreateVideoCapturer(camDev);
			if (NULL == camCapture)
			{
				printf("Create video capture fail\n");
			}
			else
			{
				MyFrameFactory *frameFactoryObj = new MyFrameFactory();
				
				VideoFormat bestFormat;
				if (!camCapture->GetBestCaptureFormat(VideoFormat(640, 480,
					cricket::VideoFormat::FpsToInterval(15), FOURCC_YUY2), &bestFormat))
				{
					printf("GetBestCaptureFormat Fail\n");
				}
				else
				{
					VideoRenderer *pVideoRender = VideoRendererFactory::CreateGuiVideoRenderer(640, 480);
					if (NULL == pVideoRender)
					{
						printf("\nCreateGuiVideoRenderer fail\n");
					}
					pVideoRender->SetSize(bestFormat.width, bestFormat.height, 0);
					frameFactoryObj->ResetVideoRender(pVideoRender);
					camCapture->SignalFrameCaptured.connect(frameFactoryObj, &MyFrameFactory::OnFrameCaptured);
					//camCapture->set_frame_factory(frameFactoryObj);
					CaptureState state = camCapture->Start(bestFormat);
					printf("Start Capture state:%d",state);
					rtc::Event we(true,false);
					we.Wait(30000);

					camCapture->Stop();
					camCapture->SignalFrameCaptured.disconnect_all();
					//camCapture->set_frame_factory(NULL);
					frameFactoryObj->ResetVideoRender(NULL);
					if (pVideoRender)
					{
						delete pVideoRender;
						pVideoRender = NULL;
					}
					
				}

				delete camCapture;
				camCapture = NULL;

				delete frameFactoryObj;
				frameFactoryObj = NULL;
			}
		}
		else
		{
			printf("GetVideoCaptureDevice fail\n");
		}

		pDevManager->Terminate();
		pDevManager = NULL;
	}

	getchar();

	return 0;
}

