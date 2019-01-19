#include "mp3filedecoder.h"
#include "lame/include/lame.h"
#include <stdlib.h>
#include <SDL2/SDL.h>

MP3FileDecoder::MP3FileDecoder()
{

}

MP3FileDecoder::~MP3FileDecoder()
{

}

int MP3FileDecoder::LoadMP3File(const char *filename, wavedata *data)
{
    if (filename == NULL || data == NULL)
    {
        return -1;
    }

    FILE *fp = fopen(filename,"rb");
    if (NULL == fp)
    {
        return -2;
    }

//    hip_t fhdl = hip_decode_init();
//    if (NULL == fhdl)
//    {
//        return -3;
//    }

    int nReadBufSize = 128 * 1024;
    unsigned char *pBuf = new unsigned char[nReadBufSize];

    int nRead = fread(pBuf,1,nReadBufSize,fp);
    unsigned char* pMp3Frame = pBuf;
    int frameSize = 0;
    short *pcm_left = new short[4096];
    short *pcm_right = new short[4096];
    int framenum = 0;
    while (nRead > 0)
    {
        mp3data_struct ds;
//        int nout = hip_decode1_headers(fhdl,pMp3Frame,nRead,pcm_left,pcm_right,&ds);
//        if (nout > 0)
        {
//            if (frameSize <= 0)
//            {
//                framesize = ds.framesize;
//                data->spec.freq = ds.samplerate;
//                data->sepc.format = AUDIO_S16LSB;
//                data->spec.channels = 2;
//                data->spec.silence = 0;
//                data->spec.samples = nout;
//                data->spec.size = nout * 4;

//                if (data->soundlen < ds.totalframes * nout * 4)
//                {
//                    if (data->sound)
//                    {
//                        delete data->sound;
//                    }
//                    data->sound = new unsigned char[ds.totalframes * nout * 4];
//                }
//            }
//            //while (nout > 0)
//            int offset = data->spec.size * framenum;
//            short* soundptr = (short*)(data->sound + offset);
//            for (int i = 0; i < nout; i++)
//            {
//                soundptr[i * 2] = pcm_left[i];
//                soundptr[i * 2 + 1] = pcm_right[i];
//            }
//            framenum++;
//            pMp3Frame += ds.framesize;
        }

    }

}
