#ifndef CSDLPLAYER_H
#define CSDLPLAYER_H

extern "C"{
#include "SDL2/SDL.h"
}

#include <string>

#define REFRESH_EVENT  (SDL_USEREVENT + 1)

class CSDLPlayer
{
public:
    CSDLPlayer();
    virtual void Play();

protected:
    virtual void Init();
    virtual void InitSDL();
    virtual void InitFrameData() = 0;
    virtual void UpdateTexture(const unsigned char* pBuffer, int& iFrameDataPitch);
    virtual void GetRenderData(unsigned char*& pBuffer, int& iFrameDataPitch) = 0;
    static int RefreshTimer(void *param);
    virtual void Run();

protected:
    SDL_Texture*    m_SDLTexture;
    SDL_Renderer*   m_SDLRenderer;
    SDL_Window*     m_Screen;
    SDL_Rect        m_SDLRect;
    int             m_WindowW;
    int             m_WindowH;
    int             m_PixelW;
    int             m_PixelH;
    Uint32          m_Pixformat;
};

#endif // CSDLPLAYER_H
