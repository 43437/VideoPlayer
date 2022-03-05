#ifndef CSDLPLAYER_H
#define CSDLPLAYER_H

extern "C"{
#include "SDL2/SDL.h"
}

#include <string>

class CSDLPlayer
{
public:
    CSDLPlayer();
    void Play();

private:
    enum EFrameDataType
    {
        eFrameDataType_From = 0,
        eFrameDataType_BGRA = eFrameDataType_From,
        eFrameDataType_RGB24,
        eFrameDataType_BGR24,
        eFrameDataType_YUV420P,
        eFrameDataType_To,
    };

private:
    void Init();
    void InitSDL();
    void InitFrameData();
    void UpdateTexture(const unsigned char* pBuffer);
    void GetRenderData(unsigned char*& pBuffer);

private:
    SDL_Texture*    m_SDLTexture;
    SDL_Renderer*   m_SDLRenderer;
    SDL_Window*     m_Screen;
    int             m_WindowW;
    int             m_WindowH;
    int             m_PixelW;
    int             m_PixelH;
    Uint32          m_Pixformat;
    SDL_Rect        m_SDLRect;
    unsigned char*  m_pBuffer;          //read data
    unsigned char*  m_pBufferConvert;   //convert data, rgb24 & bgr24 need convert to 32bit
    int             m_BufferPixelW;
    std::string     m_VideoFileName;
    FILE*           m_fp;
    int             m_FrameDataSize;
    int             m_FrameDataPitch;
    EFrameDataType  m_eFrameDataType;
};

#endif // CSDLPLAYER_H
