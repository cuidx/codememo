// AudioCapture.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define WEBRTC_WIN

#include "webrtc\base\event.h"
#include "talk\media\devices\devicemanager.h"
#include "webrtc\modules\audio_device\include\audio_device.h"
#include "third_party/libiconv/iconv.h"
#include <list>

using namespace cricket;
using namespace webrtc;

#pragma comment(lib,"iconv.lib")
#pragma comment(lib,"Ws2_32.lib")
//MoInitMediaType
#pragma comment(lib,"Msdmo.lib")
//IID_IMediaBuffer
#pragma comment(lib,"dmoguids.lib")
//CLSID_CWMAudioAEC
#pragma comment(lib,"wmcodecdspuuid.lib")

#pragma comment(lib,"libjpeg.lib")
#pragma comment(lib,"libyuv.lib")
#pragma comment(lib,"directshow_baseclasses.lib")
#pragma comment(lib,"system_wrappers.lib")
#pragma comment(lib,"common_video.lib")
#pragma comment(lib,"webrtc_base.lib")
#pragma comment(lib,"video_capture_module.lib")
#pragma comment(lib,"libjingle_media.lib")
#pragma comment(lib,"audio_device.lib")
#pragma comment(lib,"common_audio.lib")

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

			delete[]outBuf;
			outBuf = NULL;
		}

		iconv_close(cd);
	}
	return sOut;
}

class MyAudioFrame
{
public:
	MyAudioFrame(uint8_t* samples, uint32_t nSamples, uint8_t nBytesPerSample, uint8_t nChannels, uint32_t nSamplesPerSec)
	{
		m_nSamples = nSamples;
		m_nBytesPerSample = nBytesPerSample;
		m_nChannels = nChannels;
		m_nSamplesPerSec = nSamplesPerSec;
		int nSampleBytes = m_nSamples * m_nBytesPerSample;
		if (nSampleBytes > 0)
		{
			m_audioSamples = new uint8_t[nSampleBytes];
			if (m_audioSamples != NULL)
			{
				memcpy(m_audioSamples, samples, nSampleBytes);
			}
		}
		else
		{
			m_audioSamples = NULL;
		}
	}
	virtual ~MyAudioFrame()
	{
		if (NULL != m_audioSamples)
		{
			delete m_audioSamples;
			m_audioSamples = NULL;
		}
	}

	uint8_t* m_audioSamples;
	uint32_t m_nSamples;
	uint8_t  m_nBytesPerSample;
	uint8_t  m_nChannels;
	uint32_t m_nSamplesPerSec;
};

class MyAudioTransport : public AudioTransport
{
public:
	MyAudioTransport() {}
	virtual ~MyAudioTransport() {}

	virtual int32_t RecordedDataIsAvailable(const void* audioSamples,
		const uint32_t nSamples,
		const uint8_t nBytesPerSample,
		const uint8_t nChannels,
		const uint32_t samplesPerSec,
		const uint32_t totalDelayMS,
		const int32_t clockDrift,
		const uint32_t currentMicLevel,
		const bool keyPressed,
		uint32_t& newMicLevel)
	{
		printf("\nRecordedDataIsAvailable nSamples=%u,nBytesPerSample=%u,nChannels=%u,samplesPerSec=%u,totalDelayMS=%u,clockDrift=%d,currentMicLevel=%u,keyPressed=%u",
			nSamples, nBytesPerSample, nChannels, samplesPerSec, totalDelayMS, clockDrift, currentMicLevel, keyPressed);

		newMicLevel = currentMicLevel;

		m_audioDatalist.push_back(new MyAudioFrame((uint8_t*)audioSamples, nSamples, nBytesPerSample, nChannels, samplesPerSec));

		return 0;
	}

	virtual int32_t NeedMorePlayData(const uint32_t nSamples,
		const uint8_t nBytesPerSample,
		const uint8_t nChannels,
		const uint32_t samplesPerSec,
		void* audioSamples,
		uint32_t& nSamplesOut,
		int64_t* elapsed_time_ms,
		int64_t* ntp_time_ms)
	{
		printf("\nNeedMorePlayData nSamples=%u,nBytesPerSample=%u,nChannels=%u,samplesPerSec=%u\n",
			nSamples, nBytesPerSample, nChannels, samplesPerSec);

		if (m_audioDatalist.empty())
		{
			nSamplesOut = nSamples;
			memset(audioSamples, 0, nSamplesOut * nBytesPerSample);
		}
		else
		{
			MyAudioFrame* pFrame = m_audioDatalist.front();
			m_audioDatalist.pop_front();
			if (pFrame && pFrame->m_audioSamples != NULL)
			{
				nSamplesOut = pFrame->m_nSamples <= nSamples ? pFrame->m_nSamples : nSamples;
				memcpy(audioSamples, pFrame->m_audioSamples, nBytesPerSample * nSamplesOut);
			}
			else
			{
				nSamplesOut = nSamples;
				memset(audioSamples, 0, nSamplesOut * nBytesPerSample);
			}
			if (pFrame)
			{
				delete pFrame;
				pFrame = NULL;
			}
		}
		return 0;
	}

protected:
	std::list<MyAudioFrame*> m_audioDatalist;
};

int _tmain(int argc, _TCHAR* argv[])
{
	DeviceManagerInterface *pDevManager = DeviceManagerFactory::Create();
	if (pDevManager != NULL)
	{
		bool bRet = pDevManager->Init();
		if (!bRet)
		{
			printf("deviceManager Init fail\n");
			return -1;
		}

		printf("\nTest Audiocapture\n");
		Device micDev;
		std::string micName;
		if (pDevManager->GetAudioInputDevice(micName, &micDev))
		{
			AudioDeviceModule *pDevModule = CreateAudioDeviceModule(1, AudioDeviceModule::AudioLayer::kPlatformDefaultAudio);
			if (NULL != pDevModule)
			{
				int nRet = pDevModule->Init();
				printf("AudioDeviceModule::Init return %d\n",nRet);

				nRet = pDevModule->SetRecordingDevice(AudioDeviceModule::kDefaultDevice);
				printf("AudioDeviceModule::SetRecordingDevice return %d\n", nRet);

				nRet = pDevModule->InitMicrophone();
				printf("AudioDeviceModule::InitMicrophone return %d\n", nRet);

				nRet = pDevModule->SetRecordingChannel(webrtc::AudioDeviceModule::kChannelBoth);
				printf("AudioDeviceModule::SetRecordingChannel return %d\n", nRet);

				nRet = pDevModule->InitRecording();
				printf("AudioDeviceModule::InitRecording return %d\n", nRet);

				nRet = pDevModule->SetRecordingSampleRate(48000);
				printf("AudioDeviceModule::SetRecordingSampleRate return %d\n", nRet);

				nRet = pDevModule->SetPlayoutDevice(webrtc::AudioDeviceModule::kDefaultDevice);
				printf("AudioDeviceModule::SetPlayoutDevice return %d\n", nRet);

				nRet = pDevModule->InitSpeaker();
				printf("AudioDeviceModule::InitSpeaker return %d\n",nRet);

				nRet = pDevModule->InitPlayout();
				printf("AudioDeviceModule::InitPlayout return %d\n",nRet);

				nRet = pDevModule->SetPlayoutSampleRate(48000);
				printf("AudioDeviceModule::SetPlayoutSampleRate return %d\n", nRet);

				nRet = pDevModule->StartRecording();
				printf("AudioDeviceModule::StartRecording return %d\n", nRet);

				nRet = pDevModule->StartPlayout();
				printf("AudioDeviceModule::StartPlayout return %d\n", nRet);

				//录制到文件
				char lszOutfile[512] = "C:\\Users\\cuidx\\Desktop\\bfq\\record.pcm";
				nRet = pDevModule->StartRawInputFileRecording(lszOutfile);
				printf("AudioDeviceModule::StartRawInputFileRecording return %d\n", nRet);

				MyAudioTransport *pCallBack = new MyAudioTransport();
				pDevModule->RegisterAudioCallback(pCallBack);

				rtc::Event we(true, false);
				we.Wait(30000);

				nRet = pDevModule->StopRawInputFileRecording();
				printf("AudioDeviceModule::StopRawInputFileRecording return %d\n", nRet);

				nRet = pDevModule->StopRecording();
				printf("AudioDeviceModule::StopRecording return %d\n",nRet);

				nRet = pDevModule->StopPlayout();
				printf("AudioDeviceModule::StopPlayout return %d\n",nRet);

				pDevModule->Release();

				if (pCallBack)
				{
					delete pCallBack;
					pCallBack = NULL;
				}
				
			}
			else
			{
				printf("CreateAudioDeviceModule fail\n");
			}
		}
		else
		{
			printf("GetAudioInputDevice fail\n");
		}
	}
	getchar();
	return 0;
}

