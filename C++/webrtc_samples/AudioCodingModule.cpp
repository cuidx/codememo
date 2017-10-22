// AudioCodingModule.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "webrtc\modules\audio_coding\main\interface\audio_coding_module.h"
#define WEBRTC_WIN
#include "webrtc\base\thread.h"
#include "webrtc\system_wrappers\interface\trace.h"
#include <string>

using namespace webrtc;

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"webrtc_base.lib")
//neteq.lib
#pragma comment(lib,"opus.lib")
#pragma comment(lib,"webrtc_opus.lib")
#pragma comment(lib,"audio_coding_module.lib")
//webrtc::SincResampler::Convolve_SSE
#pragma comment(lib,"common_audio_sse2.lib")
//PushResampler
#pragma comment(lib,"common_audio.lib")
#pragma comment(lib,"system_wrappers.lib")
//timeBeginPeriod  timeGetTime   timeEndPeriod timeSetEvent timeKillEvent
#pragma comment(lib,"Winmm.lib")
//WebRtcCng_InitEnc
#pragma comment(lib,"CNG.lib")
//webrtc::NetEq
#pragma comment(lib,"neteq.lib")
//neteq.lib  WebRtcPcm16b_DecodeW16
#pragma comment(lib,"PCM16B.lib")
//neteq.lib
#pragma comment(lib,"iLBC.lib")
//neteq.lib
#pragma comment(lib,"G722.lib")
#pragma comment(lib,"G711.lib")
#pragma comment(lib,"iSAC.lib")

class CodecAudioCallBack : public AudioPacketizationCallback
{
public:
	CodecAudioCallBack()
	{
		m_decoder = NULL;
		m_fp = NULL;
		aux_rtp_header = NULL;
		count = 0;
	}
	virtual ~CodecAudioCallBack()
	{
		if (m_fp)
		{
			fclose(m_fp);
			m_fp = NULL;
		}
		if (m_decoder)
		{
			delete m_decoder;
			m_decoder = NULL;
		}
		if (aux_rtp_header)
		{
			delete aux_rtp_header;
			aux_rtp_header = NULL;
		}
	}
	virtual int32_t SendData(
		FrameType frame_type,
		uint8_t payload_type,
		uint32_t timestamp,
		const uint8_t* payload_data,
		uint16_t payload_len_bytes,
		const RTPFragmentationHeader* fragmentation)
	{
		//printf("recv packeted audio data :frame_type = %d,payload_type = %d,timestamp=%u,playload_len_bytes=%d\n",
		//	frame_type, payload_type, timestamp, payload_len_bytes);

		//编完码。立即解码，验证编码的正确性
		if (m_decoder)
		{
			count++;

#if 1
			//这里直接用IncomingPayload 也可以
			m_decoder->IncomingPayload(payload_data, payload_len_bytes, payload_type, timestamp);
#else
			if (aux_rtp_header == NULL) {
				// This is the first time that we are using |dummy_rtp_header_|
				// so we have to create it.
				aux_rtp_header = new WebRtcRTPHeader;
				aux_rtp_header->header.payloadType = payload_type;
				// Don't matter in this case.
				aux_rtp_header->header.ssrc = 0;
				aux_rtp_header->header.markerBit = false;
				// Start with random numbers.
				aux_rtp_header->header.sequenceNumber = 0x1234;  // Arbitrary.
				aux_rtp_header->type.Audio.channel = 2;
			}

			aux_rtp_header->header.timestamp = timestamp;
			//IncomingPacket(incoming_payload, payload_length, *aux_rtp_header_);
			m_decoder->IncomingPacket(payload_data, payload_len_bytes, *aux_rtp_header);
			// Get ready for the next payload.
			aux_rtp_header->header.sequenceNumber++;
#endif
			AudioFrame audioframe;
			int nRet = m_decoder->PlayoutData10Ms(48000, &audioframe);
			if (nRet == 0)
			{
				if (m_fp == NULL)
				{
					m_fp = fopen("C:\\Users\\cuidx\\Desktop\\bfq\\decode.pcm", "wb");
				}
				if (m_fp)
				{
					fwrite(audioframe.data_, 2, audioframe.samples_per_channel_ * audioframe.num_channels_, m_fp);
				}
			}
			else
			{
				printf("PlayoutData10Ms fail return %d\n");
			}
			
		}
		return 0;
	}

	int InitDecoder(CodecInst &codec)
	{
		if (NULL == m_decoder)
		{
			m_decoder = AudioCodingModule::Create(2);
		}
		if (m_decoder)
		{
			int nRet = m_decoder->InitializeReceiver();
			printf("InitializeReceiver return %d\n", nRet);

			nRet = m_decoder->ResetDecoder();

			nRet = m_decoder->RegisterReceiveCodec(codec);
			printf("RegisterReceiveCodec return %d",nRet);

			nRet = m_decoder->SetPlayoutMode(webrtc::AudioPlayoutMode::off);
			printf("SetPlayoutMode return %d\n",nRet);

			m_decoder->SetREDStatus(false);

		}
		return 0;
	}

	void DecodeRecvData()
	{
		//AudioFrame audioframe;
		//int nRet = m_decoder->PlayoutData10Ms(48000, &audioframe);
		//while (nRet == 0)
		//{
		//	if (m_fp == NULL)
		//	{
		//		m_fp = fopen("C:\\Users\\cuidx\\Desktop\\bfq\\decode.pcm", "wb");
		//	}
		//	if (m_fp)
		//	{
		//		fwrite(audioframe.data_, 2, audioframe.samples_per_channel_ * audioframe.num_channels_, m_fp);
		//	}
		//	nRet = m_decoder->PlayoutData10Ms(48000, &audioframe);
		//}
		printf("total recv packet count:%d\n",count);
	}

private:
	AudioCodingModule *m_decoder;
	FILE * m_fp;
	WebRtcRTPHeader * aux_rtp_header;
	int count;
};

class PcmFileEncoder : public rtc::Runnable
{
public:
	PcmFileEncoder(AudioCodingModule* pCodingMo)
	{
		m_pCodingMod = pCodingMo;
		m_samplerate = 0;
		m_bits_per_sample = 0;
		m_channel = 0;
	}
	virtual ~PcmFileEncoder() {}

	void SetPcmFileName(std::string &file)
	{
		m_filename = file;
	}

	void SetPcmFileInfo(int samplerate,int bits_per_sample,int channel)
	{
		m_samplerate = samplerate;
		m_bits_per_sample = bits_per_sample;
		m_channel = channel;
	}

	virtual void Run(rtc::Thread* thread)
	{
		if (NULL == m_pCodingMod)
		{
			printf("audio coding module not set\n");
			if (thread)
			{
				thread->Quit();
			}
			return;
		}
		if (m_filename.empty())
		{
			printf("pcm file not set\n");
			if (thread)
			{
				thread->Quit();
			}
			return;
		}

		FILE * fp = fopen(m_filename.c_str(), "rb");
		if (NULL == fp)
		{
			printf("open pcm file fail, pcm_file:%s\n", m_filename.c_str());
			if (thread)
			{
				thread->Quit();
			}
			return;
		}

		//FILE * fp_dump = fopen("C:\\Users\\cuidx\\Desktop\\bfq\\codedata.pcm","wb");
		//测试文件是双声道的，这里简单做了，反正只验证编解码的过程
		int bytes_10_ms = m_samplerate * 2 * 2 / 100;

		webrtc::AudioFrame frame;
		frame.sample_rate_hz_ = m_samplerate;
		frame.timestamp_ = 0;
		frame.num_channels_ = 2;
		frame.samples_per_channel_ = m_samplerate / 100;
		short *pBuf = new short[bytes_10_ms / 2];
		uint32_t timestamp = 0;
		int count = 0;
		rtc::Event we(true, false);
		while (!feof(fp))
		{
			memset(frame.data_, 0, sizeof(frame.data_));
			int nRead = fread(pBuf, 2, bytes_10_ms / 2, fp);
			if (nRead > 0)
			{
				for (int iPos = 0; iPos < nRead; iPos++)
				{
					frame.data_[iPos] = pBuf[iPos];
				}

				//if (fp_dump)
				//{
				//	fwrite(frame.data_, 2, nRead / 2, fp_dump);
				//}

				int nRet = m_pCodingMod->Add10MsData(frame);
				//printf("Add10MsData return %d\n",nRet);
				nRet = m_pCodingMod->Process();
				//printf("Process return %d\n", nRet);
				frame.timestamp_ += frame.samples_per_channel_;
				count++;
				
				//不播放，不用等
				//we.Wait(10);
			}
		}

		printf("total send packet:%d\n",count);

		fclose(fp);
		fp = NULL;

		//fclose(fp_dump);
		//fp_dump = NULL;

		if (thread)
		{
			//告诉线程，我要退出
			thread->Quit();
		}
	}

private:
	std::string m_filename;
	int m_samplerate;
	int m_bits_per_sample;
	int m_channel;
	AudioCodingModule *m_pCodingMod;
};


int _tmain(int argc, _TCHAR* argv[])
{
	webrtc::Trace::CreateTrace();
	webrtc::Trace::SetTraceFile("C:\\Users\\cuidx\\Desktop\\bfq\\codingtrace.txt");
	//Get supported codec with list number.
	printf("supported codec number:%d\n", AudioCodingModule::NumberOfCodecs());
	printf("Get supported codec with list number.\n");
	int id = 0;
	CodecInst codecInst;
	while (AudioCodingModule::Codec(id, &codecInst) == 0)
	{
		printf("codec %d:[name:%s,pltype:%d,plfrep:%d,rate:%d,pacsize:%d,channels:%d]\n", id,
			codecInst.plname, codecInst.pltype,
			codecInst.plfreq, codecInst.rate, codecInst.pacsize, codecInst.channels);
		id++;
	}

	AudioCodingModule *mod = AudioCodingModule::Create(1);
	if (mod != NULL)
	{
		printf("AudioCodingModule::Create succeed \n");
		int nRet = mod->InitializeSender();
		printf("mod->InitializeSender return %d\n",nRet);

		CodecInst sendCodec;
		nRet = AudioCodingModule::Codec("opus", &sendCodec, 48000, 2);
		//pacsize =(plfreq * k) / 1000, we make a packet every 10ms
		sendCodec.pacsize = 480;
		printf("Get 48k OPUS codec return %d\n",nRet);

		nRet = mod->RegisterSendCodec(sendCodec);
		printf("RegisterSendCodec return %d\n",nRet);

		//编一路小码率的流
		//CodecInst ilbcCodec;
		//nRet = AudioCodingModule::Codec("ISAC", &ilbcCodec, 48000, 1);
		//printf("Get 48K ILBC codec return %d\n",nRet);
		//ilbcCodec.rate = 10000;

		//nRet = mod->RegisterSecondarySendCodec(ilbcCodec);
		//printf("RegisterSecondarySendCodec return %d\n",nRet);

		mod->SetVAD(false, false);

		mod->SetREDStatus(false);

		mod->SetCodecFEC(false);

		CodecAudioCallBack cb;
		nRet = mod->RegisterTransportCallback(&cb);
		printf("RegisterTransportCallback return %d\n",nRet);

		cb.InitDecoder(sendCodec);

		PcmFileEncoder fileEnc(mod);
		fileEnc.SetPcmFileName(std::string("C:\\Users\\cuidx\\Desktop\\bfq\\record.pcm"));
		fileEnc.SetPcmFileInfo(48000, 16, 2);
		//fileEnc.Run(NULL);

		rtc::Thread codingThread;
		codingThread.Start(&fileEnc);

		while (!codingThread.IsQuitting())
		{
			Sleep(10);
		}
		printf("coding thread quit\n");

		cb.DecodeRecvData();

		delete mod;
		mod = NULL;
	}
	else
	{
		printf("AudioCodingModule::Create fail \n");
	}

	getchar();

	webrtc::Trace::ReturnTrace();

	return 0;
}

