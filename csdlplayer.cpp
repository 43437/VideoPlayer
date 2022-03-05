#include "csdlplayer.h"
#include <iostream>

//Refresh Event
#define REFRESH_EVENT  (SDL_USEREVENT + 1)

void ConvertRGB24toRGB32(unsigned char *image_in, unsigned char *image_out, int w, int h)
{
    for(int i =0; i<h; i++)
        for(int j=0; j<w; j++)
        {
            //Big Endian or Small Endian?
            //"ARGB" order:high bit -> low bit.
            //ARGB Format Big Endian (low address save high MSB, here is A) in memory : A|R|G|B
            //ARGB Format Little Endian (low address save low MSB, here is B) in memory : B|G|R|A
            if(SDL_BYTEORDER==SDL_LIL_ENDIAN)
            {
                //Little Endian (x86): R|G|B --> B|G|R|A
                image_out[(i*w+j)*4+0]=image_in[(i*w+j)*3+2];
                image_out[(i*w+j)*4+1]=image_in[(i*w+j)*3+1];
                image_out[(i*w+j)*4+2]=image_in[(i*w+j)*3];
                image_out[(i*w+j)*4+3]='0';
            }
            else
            {
                //Big Endian: R|G|B --> A|R|G|B
                image_out[(i*w+j)*4]='0';
                memcpy(image_out+(i*w+j)*4+1,image_in+(i*w+j)*3,3);
            }
        }
}

#define SEEK_SET	0

int RefreshTimer(void *param)
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

CSDLPlayer::CSDLPlayer() : m_SDLTexture(nullptr)
                        ,m_SDLRenderer(nullptr)
                        ,m_Screen(nullptr)
                        ,m_WindowW(852)
                        ,m_WindowH(480)
                        ,m_eFrameDataType(eFrameDataType_YUV420P)
{
    Init();
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

void CSDLPlayer::InitFrameData()
{
    int SizePerPixel = 32;
    switch (m_eFrameDataType)
    {
    case eFrameDataType_BGRA:
        m_VideoFileName = "test_bgra_320x180.rgb";
        m_PixelW = 320;
        m_PixelH = 180;
        //Note: ARGB8888 in "Little Endian" system stores as B|G|R|A
        m_Pixformat = SDL_PIXELFORMAT_ARGB8888;
        SizePerPixel = 32;
        m_FrameDataPitch = m_PixelW * 4;
        break;
    case eFrameDataType_RGB24:
        m_VideoFileName = "test_rgb24_320x180.rgb";
        m_PixelW = 320;
        m_PixelH = 180;
        m_Pixformat = SDL_PIXELFORMAT_RGB888;
        SizePerPixel = 24;
        m_FrameDataPitch = m_PixelW * 4;
        break;
    case eFrameDataType_BGR24:
        m_VideoFileName = "test_bgr24_320x180.rgb";
        m_PixelW = 320;
        m_PixelH = 180;
        m_Pixformat = SDL_PIXELFORMAT_BGR888;
        SizePerPixel = 24;
        m_FrameDataPitch = m_PixelW * 4;
        break;
    case eFrameDataType_YUV420P:
        m_VideoFileName = "test_yuv420p_426x240.yuv";
        //IYUV: Y + U + V  (3 planes)
        //YV12: Y + V + U  (3 planes)
        m_PixelW = 426;
        m_PixelH = 240;
        m_Pixformat = SDL_PIXELFORMAT_IYUV;
        SizePerPixel = 12;
        m_FrameDataPitch = m_PixelW;
        break;
    default:
        break;
    }

    m_fp = std::fopen(m_VideoFileName.c_str(),"rb+");
    if(nullptr == m_fp)
    {
        std::cout << "cannot open this file" << std::endl;;
        return;
    }

    m_FrameDataSize = m_PixelW * m_PixelH * SizePerPixel / 8;
    m_pBuffer = new unsigned char[m_FrameDataSize];
    m_pBufferConvert = new unsigned char[m_PixelW * m_PixelH * 32 / 8];
}

void CSDLPlayer::Init()
{
    InitFrameData();
    InitSDL();
}

void CSDLPlayer::UpdateTexture(const unsigned char* pBuffer)
{
    if (nullptr == pBuffer)
        return;
    SDL_UpdateTexture( m_SDLTexture, NULL, pBuffer, m_FrameDataPitch);
    SDL_RenderClear( m_SDLRenderer );
    SDL_RenderCopy( m_SDLRenderer, m_SDLTexture, NULL, &m_SDLRect);
    SDL_RenderPresent( m_SDLRenderer );
}

void CSDLPlayer::GetRenderData(unsigned char*& pBuffer)
{
    if (std::fread(m_pBuffer, 1, m_FrameDataSize, m_fp) != m_FrameDataSize)
    {
        // Loop
        std::fseek(m_fp, 0, SEEK_SET);
        std::fread(m_pBuffer, 1, m_FrameDataSize, m_fp);
    }
    if (eFrameDataType_RGB24 == m_eFrameDataType
            || eFrameDataType_BGR24 == m_eFrameDataType)
    {
        ConvertRGB24toRGB32(m_pBuffer, m_pBufferConvert, m_PixelW, m_PixelH);
        pBuffer = m_pBufferConvert;
    }
    else
    {
        pBuffer = m_pBuffer;
    }
}

void CSDLPlayer::Play()
{
    bool bRefreshing = true;
    SDL_Thread *hdRefreshThd = SDL_CreateThread(RefreshTimer, NULL, &bRefreshing);
    SDL_Event event;
    unsigned char* pData = nullptr;
    while(1)
    {
        //Wait
        SDL_WaitEvent(&event);
        if( REFRESH_EVENT == event.type )
        {
            GetRenderData(pData);
            UpdateTexture(pData);

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
