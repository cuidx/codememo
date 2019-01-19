// tutorial03.c
// A pedagogical video player that will stream through every video frame as fast as it can
// and play audio (out of sync).
//
// This tutorial was written by Stephen Dranger (dranger@gmail.com).
//
// Code based on FFplay, Copyright (c) 2003 Fabrice Bellard,
// and a tutorial by Martin Bohme (boehme@inb.uni-luebeckREMOVETHIS.de)
// Tested on Gentoo, CVS version 5/01/07 compiled with GCC 4.1.1
//
// Updates tested on:
// Mac OS X 10.11.6
// Apple LLVM version 8.0.0 (clang-800.0.38)
//
// Use
//
// $ gcc -o tutorial03 tutorial03.c -lavutil -lavformat -lavcodec -lswscale -lz -lm `sdl-config --cflags --libs`
//
// to build (assuming libavutil/libavformat/libavcodec/libswscale are correctly installed your system).
//
// Run using
//
// $ tutorial03 myvideofile.mpg
//
// to play the stream on your screen with voice.

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_video.h>
}
#ifdef __MINGW32__
#undef main // Prevents SDL from overriding main().
#endif

#include <stdio.h>

#define SDL_AUDIO_BUFFER_SIZE 1024
#define MAX_AUDIO_FRAME_SIZE 192000


typedef struct PacketQueue {
  AVPacketList *first_pkt, *last_pkt;
  int nb_packets;
  int size;
  SDL_mutex *mutex;
  SDL_cond *cond;
} PacketQueue;

AVFrame wanted_frame;
PacketQueue audioq;

int quit = 0;

void packet_queue_init(PacketQueue *q) {
    memset(q, 0, sizeof(PacketQueue));
    q->mutex = SDL_CreateMutex();
    q->cond = SDL_CreateCond();
}

int packet_queue_put(PacketQueue *q, AVPacket *pkt) {
    AVPacketList *pkt1;
    if (av_packet_ref(pkt, pkt) < 0) {
        return -1;
    }
    pkt1 = (AVPacketList *)av_malloc(sizeof(AVPacketList));
    if (!pkt1) {
        return -1;
    }
    pkt1->pkt = *pkt;
    pkt1->next = NULL;


    SDL_LockMutex(q->mutex);

    if (!q->last_pkt) {
        q->first_pkt = pkt1;
    }
    else {
        q->last_pkt->next = pkt1;
    }
    q->last_pkt = pkt1;
    q->nb_packets++;
    q->size += pkt1->pkt.size;
    SDL_CondSignal(q->cond);

    SDL_UnlockMutex(q->mutex);
    return 0;
}

static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block) {
    AVPacketList *pkt1;
    int ret;

    SDL_LockMutex(q->mutex);

    for (;;) {
        if (quit) {
            ret = -1;
            break;
        }

        pkt1 = q->first_pkt;
        if (pkt1) {
            q->first_pkt = pkt1->next;
            if (!q->first_pkt) {
                q->last_pkt = NULL;
            }
            q->nb_packets--;
            q->size -= pkt1->pkt.size;
            *pkt = pkt1->pkt;
            av_free(pkt1);
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            SDL_CondWait(q->cond, q->mutex);
        }
    }
    SDL_UnlockMutex(q->mutex);
    return ret;
}

int audio_decode_frame(AVCodecContext *aCodecCtx, uint8_t *audio_buf, int buf_size) {
    static AVPacket pkt;
    static uint8_t *audio_pkt_data = NULL;
    static int audio_pkt_size = 0;
    static AVFrame frame;
    SwrContext* swr_ctx = nullptr;

    int len1, data_size = 0;
    //memset(&pkt,0,sizeof(pkt));

    //fprintf(stderr, "audio_decode_frame aCodecCtx - %p,buf - %p,buf_size = %d ,audio_pkt_size = %d\n",aCodecCtx, audio_buf,buf_size,audio_pkt_size);

    for (;;) {
        while(audio_pkt_size > 0) {
            int got_frame = 0;

#if 1
            len1 = avcodec_decode_audio4(aCodecCtx, &frame, &got_frame, &pkt);

            if (len1 < 0) {
                // if error, skip frame.
                audio_pkt_size = 0;
                break;
            }
            audio_pkt_data += len1;
            audio_pkt_size -= len1;
            if (got_frame) {
                data_size = av_samples_get_buffer_size(NULL, aCodecCtx->channels, frame.nb_samples, aCodecCtx->sample_fmt, 1);
                memcpy(audio_buf, frame.data[0], data_size);
            }
            if (data_size <= 0) {
                // No data yet, get more frames.
                continue;
            }

            if (frame.channels > 0 && frame.channel_layout == 0)
                frame.channel_layout = av_get_default_channel_layout(frame.channels);
            else if (frame.channels == 0 && frame.channel_layout > 0)
                frame.channels = av_get_channel_layout_nb_channels(frame.channel_layout);

            if (swr_ctx)
            {
                swr_free(&swr_ctx);
                swr_ctx = nullptr;
            }

            swr_ctx = swr_alloc_set_opts(nullptr, wanted_frame.channel_layout, (AVSampleFormat)wanted_frame.format, wanted_frame.sample_rate,
                frame.channel_layout, (AVSampleFormat)frame.format, frame.sample_rate, 0, nullptr);

            if (!swr_ctx || swr_init(swr_ctx) < 0)
            {
                //cout << "swr_init failed:" << endl;
                break;
            }

            int dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, frame.sample_rate) + frame.nb_samples,
                wanted_frame.sample_rate, wanted_frame.format,AVRounding(1));
            int len2 = swr_convert(swr_ctx, &audio_buf, dst_nb_samples,
                (const uint8_t**)frame.data, frame.nb_samples);
            if (len2 < 0)
            {
                //cout << "swr_convert failed\n";
                break;
            }

            return wanted_frame.channels * len2 * av_get_bytes_per_sample((AVSampleFormat)wanted_frame.format);

            if (data_size <= 0)
                continue; // No data yet,get more frames
#else
            int ret = avcodec_send_packet(aCodecCtx,&pkt);
            //fprintf(stderr, "avcodec_send_packet,ret = %d\n",ret);

            ret = avcodec_receive_frame(aCodecCtx,&frame);
            int dsize = 0;
            while (ret >= 0)
            {
                data_size = av_samples_get_buffer_size(NULL, aCodecCtx->channels, frame.nb_samples, aCodecCtx->sample_fmt, 1);
                if (data_size >0)
                {
                    memcpy(audio_buf + dsize,frame.data,data_size);
                    dsize += data_size;
                }
                ret = avcodec_receive_frame(aCodecCtx,&frame);
            }

            //fprintf(stderr, "avcodec_receive_frame ret = %d, aCodecCtx - %p,pkt.data - %p,pkt.size = %d,data_size = %d \n",ret,aCodecCtx, pkt.data,pkt.size,data_size);
            audio_pkt_size = 0;
            av_packet_unref(&pkt);

            // We have data, return it and come back for more later.
            return dsize;
 #endif
        }
        if (pkt.data) {
            av_packet_unref(&pkt);
        }

        if (quit) {
            return -1;
        }

        if (packet_queue_get(&audioq, &pkt, 1) < 0) {
            audio_pkt_size = 0;
            return -1;
        }
        audio_pkt_data = pkt.data;
        audio_pkt_size = pkt.size;
    }
}

void audio_callback(void *userdata, Uint8 *stream, int len) {
    AVCodecContext *aCodecCtx = (AVCodecContext *)userdata;
    int len1, audio_size;

    static uint8_t audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];
    static unsigned int audio_buf_size = 0;
    static unsigned int audio_buf_index = 0;

    while (len > 0) {
        if (audio_buf_index >= audio_buf_size) {
            // We have already sent all our data; get more.
            audio_size = audio_decode_frame(aCodecCtx, audio_buf, audio_buf_size);
            if (audio_size < 0) {
                // If error, output silence.
                audio_buf_size = 1024; // arbitrary?
                memset(audio_buf, 0, audio_buf_size);
            } else {
                audio_buf_size = audio_size;
            }
            audio_buf_index = 0;
        }
        len1 = audio_buf_size - audio_buf_index;
        if (len1 > len) {
            len1 = len;
        }
        memcpy(stream, (uint8_t *) audio_buf + audio_buf_index, len1);
        len -= len1;
        stream += len1;
        audio_buf_index += len1;
    }
}

int PlayFile(const char* file) {
    AVFormatContext *pFormatCtx = NULL;
    int i, videoStream, audioStream;
    AVCodecContext *pCodecCtx = NULL;
    AVCodec *pCodec = NULL;
    AVFrame *pFrame = NULL;
    AVFrame* pFrameYUV = NULL;
    SDL_Renderer *renderer = NULL;
    AVPacket packet;
    int frameFinished;

    AVCodecContext *aCodecCtx = NULL;
    AVCodec *aCodec = NULL;

    SDL_Texture *bmp = NULL;
    SDL_Window *screen = NULL;
    SDL_Rect rect;
    SDL_Event event;
    SDL_AudioSpec wanted_spec, spec;

    struct SwsContext *sws_ctx = NULL;
    AVDictionary *videoOptionsDict = NULL;
    AVDictionary *audioOptionsDict = NULL;

    if (NULL == file || strlen(file) <= 0) {
        fprintf(stderr, "Usage: test <file>\n");
        return -1;
    }
    // Register all formats and codecs.
    av_register_all();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
        return -2;
    }

    // Open video file.
    if (avformat_open_input(&pFormatCtx, file, NULL, NULL) != 0) {
        return -1; // Couldn't open file.
    }

    // Retrieve stream information.
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        return -1; // Couldn't find stream information.
    }

    // Dump information about file onto standard error.
    av_dump_format(pFormatCtx, 0, file, 0);

    // Find the first video stream.
    videoStream = -1;
    audioStream = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO && videoStream < 0) {
            videoStream = i;
        }
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO && audioStream < 0) {
            audioStream = i;
        }
    }
    if (videoStream == -1) {
        //return -1; // Didn't find a video stream.
    }
    if (audioStream == -1) {
        //return -1;
    }

    fprintf(stderr,"videoStream =%d,audioStream = %d\n",videoStream,audioStream);

    if (audioStream != -1)
    {

        aCodecCtx = pFormatCtx->streams[audioStream]->codec;
        aCodec = avcodec_find_decoder(aCodecCtx->codec_id);
        if (!aCodec) {
            fprintf(stderr, "Unsupported codec!\n");
            return -1;
        }

        fprintf(stderr,"Audio format aCodecCtx->sample_fmt =%d,aCodecCtx->sample_rate = %d\n",aCodecCtx->sample_fmt,aCodecCtx->sample_rate);

        // Set audio settings from codec info.
        wanted_spec.freq = aCodecCtx->sample_rate;
        wanted_spec.format = AUDIO_S16SYS;
        wanted_spec.channels = aCodecCtx->channels;
        wanted_spec.silence = 0;
        wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
        wanted_spec.callback = audio_callback;
        wanted_spec.userdata = aCodecCtx;

        if (SDL_OpenAudio(&wanted_spec, &spec) < 0) {
            fprintf(stderr, "SDL_OpenAudio: %s\n", SDL_GetError());
            return -1;
        }

        wanted_frame.format = AV_SAMPLE_FMT_S16;
        wanted_frame.sample_rate = spec.freq;
        wanted_frame.channel_layout = av_get_default_channel_layout(spec.channels);
        wanted_frame.channels = spec.channels;

        avcodec_open2(aCodecCtx, aCodec, &audioOptionsDict);

        // audio_st = pFormatCtx->streams[index].
        packet_queue_init(&audioq);
        SDL_PauseAudio(0);

    }
    if (videoStream != -1)
    {
        // Get a pointer to the codec context for the video stream.
        pCodecCtx = pFormatCtx->streams[videoStream]->codec;

        // Find the decoder for the video stream.
        pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
        if (pCodec == NULL) {
            fprintf(stderr, "Unsupported codec!\n");
            return -1; // Codec not found.
        }
        // Open codec.
        if (avcodec_open2(pCodecCtx, pCodec, &videoOptionsDict) < 0)
            return -1; // Could not open codec.

        // Allocate video frame.
        pFrame = av_frame_alloc();

        pFrameYUV = av_frame_alloc();
        if( pFrameYUV == NULL )
            return -1;

        // Make a screen to put our video.

//    #ifndef __DARWIN__
//        screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 0, 0);
//    #else
//        screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 24, 0);
//    #endif
        screen = SDL_CreateWindow("My Game Window",
                                      SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      pCodecCtx->width,  pCodecCtx->height,
                                      SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
        renderer = SDL_CreateRenderer(screen, -1, 0);

        if (!screen) {
            fprintf(stderr, "SDL: could not set video mode - exiting\n");
            exit(1);
        }

        // Allocate a place to put our YUV image on that screen.
        //bmp = SDL_CreateYUVOverlay(pCodecCtx->width, pCodecCtx->height, SDL_YV12_OVERLAY, screen);
        bmp = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_YV12,SDL_TEXTUREACCESS_STREAMING,pCodecCtx->width,pCodecCtx->height);
        sws_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);

        int numBytes = avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width,
                                      pCodecCtx->height);
        uint8_t* buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

        avpicture_fill((AVPicture *)pFrameYUV, buffer, AV_PIX_FMT_YUV420P,
                       pCodecCtx->width, pCodecCtx->height);

    }

    // Read frames and save first five frames to disk.
    i = 0;
    while (av_read_frame(pFormatCtx, &packet) >= 0) {
        // Is this a packet from the video stream?
        if (packet.stream_index == videoStream) {
            // Decode video frame.
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // Did we get a video frame?
            if (frameFinished) {
                //SDL_LockYUVOverlay(bmp);

//                AVFrame pict;
//                pict.data[0] = bmp->pixels[0];
//                pict.data[1] = bmp->pixels[2];
//                pict.data[2] = bmp->pixels[1];

//                pict.linesize[0] = bmp->pitches[0];
//                pict.linesize[1] = bmp->pitches[2];
//                pict.linesize[2] = bmp->pitches[1];

                // Convert the image into YUV format that SDL uses.
                sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,pFrameYUV->data, pFrameYUV->linesize);

                //SDL_UnlockYUVOverlay(bmp);

                rect.x = 0;
                rect.y = 0;
                rect.w = pCodecCtx->width;
                rect.h = pCodecCtx->height;
                //SDL_DisplayYUVOverlay(bmp, &rect);
                //iPitch 计算yuv一行数据占的字节数
                SDL_UpdateTexture( bmp, &rect, pFrameYUV->data[0], pFrameYUV->linesize[0] );
                SDL_RenderClear( renderer );
                SDL_RenderCopy( renderer, bmp, &rect, &rect );
                SDL_RenderPresent( renderer );

                av_packet_unref(&packet);
            }
        } else if (packet.stream_index == audioStream) {
            packet_queue_put(&audioq, &packet);
        } else {
            // Free the packet that was allocated by av_read_frame.
            av_packet_unref(&packet);
        }


        SDL_PollEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                quit = 1;
                SDL_Quit();
                exit(0);
                break;
            default:
                break;
        }

    }

    // Free the YUV frame.
    av_free(pFrame);

    // Close the codec.
    avcodec_close(pCodecCtx);

    // Close the video file.
    avformat_close_input(&pFormatCtx);

    return 0;
}

