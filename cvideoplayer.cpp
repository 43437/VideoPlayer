#include "cvideoplayer.h"
#include <iostream>

CVideoPlayer::CVideoPlayer() : CSDLPlayer()
                            ,m_VideoDecoder("test.mp4")
{

}

void CVideoPlayer::InitFrameData()
{
    m_VideoDecoder.GetPixInfo(m_PixelW, m_PixelH);
    m_Pixformat = SDL_PIXELFORMAT_IYUV;
}

void CVideoPlayer::GetRenderData(unsigned char*& pBuffer, int& iFrameDataPitch)
{
    m_VideoDecoder.GetDecodedData(pBuffer, iFrameDataPitch);
}
