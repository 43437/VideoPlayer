#include "crawdataopenglplayer.h"
#include <iostream>

CRawDataOpenglPlayer::CRawDataOpenglPlayer() : COpenglPlayer()
{

}

void CRawDataOpenglPlayer::InitFrameData()
{
    if((m_fp = std::fopen("test1280x542.yuv", "rb")) == NULL )
    {
        std::cerr << "cannot open this file" << std::endl;
    }

    m_iPixelW = 1280;
    m_iPixelH = 542;
    m_pBuf = new uint8_t[m_iPixelW * m_iPixelH * 3 / 2];
    m_pPlane[0] = m_pBuf;
    m_pPlane[1] = m_pPlane[0] + m_iPixelW * m_iPixelH;
    m_pPlane[2] = m_pPlane[1] + m_iPixelW * m_iPixelH / 4;
}

void CRawDataOpenglPlayer::GetRenderData(uint8_t**& pBuffer)
{
    if (std::fread(m_pBuf, 1, m_iPixelW * m_iPixelH * 3 / 2, m_fp) != m_iPixelW * m_iPixelH * 3 / 2)
    {
        std::cerr<<"read byte not enough. "<<std::endl;
        std::fseek(m_fp, 0, 0);
        std::fread(m_pBuf, 1, m_iPixelW * m_iPixelH * 3 / 2, m_fp);
    }
    pBuffer = m_pPlane;
}
