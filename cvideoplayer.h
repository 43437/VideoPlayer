#ifndef CVIDEOPLAYER_H
#define CVIDEOPLAYER_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <SDL2/SDL.h>
#ifdef __cplusplus
};
#endif
#include <string>
#include "csdlplayer.h"

class CVideoPlayer : public CSDLPlayer
{
public:
    CVideoPlayer();

protected:
    virtual void InitFrameData();
    virtual void GetRenderData(unsigned char*& pBuffer, int& iFrameDataPitch);

private:
    int Decode();

private:
    std::string     m_VideoFileName;
    AVFormatContext	*m_pFormatCtx;
    AVCodecContext	*m_pCodecCtx;
    const AVCodec	*m_pCodec;
    AVFrame         *m_pFrame, *m_pFrameYUV;
    AVPacket        *m_pPacket;
    SwsContext      *m_ConvertCtx;
    int				m_iVideoIndex;
};

#endif // CVIDEOPLAYER_H
