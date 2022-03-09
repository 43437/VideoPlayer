#include "cvideoopenglplayer.h"

CVideoOpenglPlayer::CVideoOpenglPlayer() : COpenglPlayer()
                                        ,m_VideoDecoder("test.mp4")
{

}

void CVideoOpenglPlayer::InitFrameData()
{
    m_VideoDecoder.GetPixInfo(m_iPixelW, m_iPixelH);
}

void CVideoOpenglPlayer::GetRenderData(uint8_t**& pBuffer)
{
    unsigned char* m_pBuf = nullptr;
    int iFrameDataPitch = 0;
    m_VideoDecoder.GetDecodedData(m_pBuf, iFrameDataPitch);
    m_pPlane[0] = m_pBuf;
    m_pPlane[1] = m_pPlane[0] + m_iPixelW * m_iPixelH;
    m_pPlane[2] = m_pPlane[1] + m_iPixelW * m_iPixelH / 4;
    pBuffer = m_pPlane;
}
