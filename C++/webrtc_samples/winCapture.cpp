// winCapture.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#define WEBRTC_WIN

#include "webrtc/modules/desktop_capture/window_capturer.h"
#include "webrtc/modules/desktop_capture/desktop_capture_options.h"
#include "webrtc/modules/desktop_capture/desktop_frame.h"
#include "webrtc/modules/desktop_capture/desktop_region.h"
#include "webrtc/system_wrappers/include/logging.h"
#include "webrtc\media\devices\videorendererfactory.h"
#include "webrtc\media\base\videoframe.h"
#include "webrtc\media\base\videoframefactory.h"
#include "webrtc\media\base\videocapturer.h"
#include "webrtc\media\engine\webrtcvideoframe.h"

#pragma comment(lib,"rtc_base.lib")
#pragma comment(lib,"desktop_capture.lib")
#pragma comment(lib,"system_wrappers.lib")
#pragma comment(lib,"rtc_base_approved.lib")
#pragma comment(lib,"webrtc_common.lib")
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"rtc_media.lib")
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"libyuv.lib")
#pragma comment(lib,"libjpeg.lib")
//#pragma comment(lib,"webrtc_i420.lib")
#pragma comment(lib,"common_video.lib")


class MyWindowCapturer : public webrtc::DesktopCapturer::Callback
{
public:
	MyWindowCapturer() 
	{ 
		videorender_ = NULL;
		capturer_.reset(webrtc::WindowCapturer::Create(webrtc::DesktopCaptureOptions::CreateDefault())); 
		if (capturer_)
		{
			capturer_->Start(this);
		}
	}
	virtual ~MyWindowCapturer() 
	{
		capturer_.reset(NULL);
	}

	virtual void OnCaptureCompleted(webrtc::DesktopFrame* frame)
	{
		frame_.reset(frame);
		printf("OnCaptureCompleted w=%d,h=%d\n",frame->size().width(), frame->size().height());
		if (videorender_ == NULL)
		{
			videorender_ = cricket::VideoRendererFactory::CreateGuiVideoRenderer(0, 0);
			//videorender_->SetSize();
		}
		if (videorender_)
		{
			cricket::CapturedFrame capframe;
			capframe.data = (void*)frame->data();
			capframe.data_size = frame->stride() * frame->size().height();
			capframe.fourcc = cricket::FOURCC_ARGB;
			capframe.height = frame->size().height();
			capframe.width = frame->size().width();

			cricket::WebRtcVideoFrame rtcframe;
			rtcframe.Init(&capframe, capframe.width, capframe.height, false);
			videorender_->OnFrame(rtcframe);
		}
	}

	bool GetWindowList(webrtc::WindowCapturer::WindowList *winList)
	{
		return capturer_->GetWindowList(winList);
	}

	bool StartCapture(webrtc::WindowCapturer::Window & win)
	{
		if (capturer_->SelectWindow(win.id))
		{
			capturer_->Capture(webrtc::DesktopRegion());
		}
		return true;
	}

protected:
	std::unique_ptr<webrtc::WindowCapturer> capturer_;
	std::unique_ptr<webrtc::DesktopFrame> frame_;
	rtc::VideoSinkInterface<cricket::VideoFrame>* videorender_;
};

int main()
{
	MyWindowCapturer capturer;
	webrtc::WindowCapturer::WindowList windows;
	if (!capturer.GetWindowList(&windows))
	{
		printf("get window list fail\n");
		return -1;
	}

	webrtc::WindowCapturer::WindowList::iterator iter = windows.begin();
	while (iter != windows.end())
	{
		printf("win_hwnd=%Ix,title=%s\n",iter->id,iter->title.c_str());
		iter++;
	}

	if (!windows.empty())
	{
		capturer.StartCapture(*windows.begin());
	}

	while (true)
	{
		MSG msg;
		if (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
	getchar();

    return 0;
}

