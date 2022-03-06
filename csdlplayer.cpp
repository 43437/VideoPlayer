#include "csdlplayer.h"
#include <iostream>


CSDLPlayer::CSDLPlayer() : m_SDLTexture(nullptr)
                        ,m_SDLRenderer(nullptr)
                        ,m_Screen(nullptr)
                        ,m_WindowW(852)
                        ,m_WindowH(480)
{
}

void CSDLPlayer::InitSDL()
{
    std::cout << "SDL init begin." << std::endl;
    if(0 != SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout<< "Could not initialize SDL - " << SDL_GetError() << std::endl;
        return;
    }

    //SDL 2.0 Support for multiple windows
    m_Screen = SDL_CreateWindow("Simplest Video Play SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        m_WindowW, m_WindowH, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if( nullptr == m_Screen )
    {
        std::cout<<"SDL: could not create window - exiting:"<<SDL_GetError();
        return;
    }
    m_SDLRenderer = SDL_CreateRenderer(m_Screen, -1, 0);

    m_SDLTexture = SDL_CreateTexture(m_SDLRenderer, m_Pixformat, SDL_TEXTUREACCESS_STREAMING, m_PixelW, m_PixelH);

    std::cout << "SDL init finished." << std::endl;
}

void CSDLPlayer::Init()
{
    InitFrameData();
    InitSDL();
}

void CSDLPlayer::UpdateTexture(const unsigned char* pBuffer, int& iFrameDataPitch)
{
    if (nullptr == pBuffer)
        return;
    SDL_UpdateTexture( m_SDLTexture, NULL, pBuffer, iFrameDataPitch);
    SDL_RenderClear( m_SDLRenderer );
    SDL_RenderCopy( m_SDLRenderer, m_SDLTexture, NULL, &m_SDLRect);
    SDL_RenderPresent( m_SDLRenderer );
}

int CSDLPlayer::RefreshTimer(void *param)
{
    while ( true )
    {
        SDL_Event event;
        event.type = REFRESH_EVENT;
        SDL_PushEvent(&event);
        SDL_Delay(40);
        bool bRefreshing = *(bool*)param;
        if (!bRefreshing)
            break;
    }
    std::cout << "exit RefreshTimer thread." << std::endl;
    return 0;
}

void CSDLPlayer::Run()
{
    bool bRefreshing = true;
    SDL_Thread *hdRefreshThd = SDL_CreateThread(CSDLPlayer::RefreshTimer, NULL, &bRefreshing);
    SDL_Event event;
    unsigned char* pData = nullptr;
    int iFrameDataPitch = 0;
    while(1)
    {
        //Wait
        SDL_WaitEvent(&event);
        if( REFRESH_EVENT == event.type )
        {
            GetRenderData(pData, iFrameDataPitch);
            UpdateTexture(pData, iFrameDataPitch);

            //Delay 40ms
            SDL_Delay(40);

        }
        else if( SDL_WINDOWEVENT == event.type )
        {
            //If Resize
            SDL_GetWindowSize(m_Screen, &m_WindowW, &m_WindowH);
            m_SDLRect.x = 0;
            m_SDLRect.y = 0;
            m_SDLRect.w = m_WindowW;
            m_SDLRect.h = m_WindowH;
        }
        else if( SDL_QUIT == event.type )
        {
            bRefreshing = false;
            break;
        }
    }
    int iRetThd = 0;
    SDL_WaitThread(hdRefreshThd, &iRetThd);
    std::cout << "hdRefreshThd ret: " << iRetThd << std::endl;
}

void CSDLPlayer::Play()
{
    Init();
    Run();
}
