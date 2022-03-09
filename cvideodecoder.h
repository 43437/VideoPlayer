#ifndef CVIDEODECODER_H
#define CVIDEODECODER_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#ifdef __cplusplus
};
#endif

#include <string>

class CVideoDecoder
{
public:
    CVideoDecoder(const std::string& strFile);
    void GetDecodedData(unsigned char*& pBuffer, int& iFrameDataPitch);
    void GetPixInfo(int& PixW, int& PixH);

private:
    int Decode();
    void Init();

private:
    std::string     m_VideoFileName;
    AVFormatContext	*m_pFormatCtx;
    AVCodecContext	*m_pCodecCtx;
    const AVCodec	*m_pCodec;
    AVFrame         *m_pFrame, *m_pFrameYUV;
    AVPacket        *m_pPacket;
    SwsContext      *m_ConvertCtx;
    int				m_iVideoIndex;
    int             m_PixelW;
    int             m_PixelH;
};

#endif // CVIDEODECODER_H
