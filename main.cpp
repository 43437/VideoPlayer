#include <iostream>

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

using namespace std;
//Refresh Event
#define SFM_REFRESH_EVENT  (SDL_USEREVENT + 1)

#define SFM_BREAK_EVENT  (SDL_USEREVENT + 2)

int thread_exit=0;
int thread_pause=0;
AVFrame	*pFrame,*pFrameYUV;
struct SwsContext *img_convert_ctx;

int sfp_refresh_thread(void *opaque){
    thread_exit=0;
    thread_pause=0;

    while (!thread_exit) {
        if(!thread_pause){
            SDL_Event event;
            event.type = SFM_REFRESH_EVENT;
            SDL_PushEvent(&event);
        }
        SDL_Delay(40);
    }
    thread_exit=0;
    thread_pause=0;
    //Break
    SDL_Event event;
    event.type = SFM_BREAK_EVENT;
    SDL_PushEvent(&event);

    return 0;
}

void Refresh(AVFrame *pFrame, AVCodecContext *pCodecCtx, SDL_Renderer* sdlRenderer, SDL_Texture* sdlTexture){
    sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
    //SDL---------------------------
    SDL_UpdateTexture( sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0] );
    SDL_RenderClear( sdlRenderer );
    //SDL_RenderCopy( sdlRenderer, sdlTexture, &sdlRect, &sdlRect );
    SDL_RenderCopy( sdlRenderer, sdlTexture, NULL, NULL);
    SDL_RenderPresent( sdlRenderer );
    //SDL End-----------------------
}

int Decode(AVFormatContext	*pFormatCtx, AVCodecContext	*pCodecCtx, int iStream, AVPacket& packet, SDL_Renderer* sdlRenderer, SDL_Texture* sdlTexture, void (*Callback)(AVFrame*, AVCodecContext*, SDL_Renderer*, SDL_Texture*)){
    int iRet = 0;
    AVFrame	*pFrame = nullptr;
    if( av_read_frame(pFormatCtx, &packet) >= 0 ){
        if(packet.stream_index == iStream ){
            std::cout<<"video stream"<<std::endl;

            iRet = avcodec_send_packet(pCodecCtx, &packet);
            if (iRet < 0){
                std::cerr << "Error sending a packet for decoding" << std::endl;
                exit(1);
            }

            while (iRet >= 0) {
                iRet = avcodec_receive_frame(pCodecCtx, pFrame);
                if (iRet == AVERROR(EAGAIN) || iRet == AVERROR_EOF)
                    break;
                else if (iRet < 0) {
                    std::cerr << "Error during decoding" << std::endl;
                    exit(1);
                }
                Callback(pFrame, pCodecCtx, sdlRenderer, sdlTexture);
//                CCommonTool::WriteJPEG(pFrame, pCodecCtx->width, pCodecCtx->height, ++iCnt, "h264");
            }
        }else{
            std::cout<<packet.stream_index << "is not video stream. "<<std::endl;
        }
//        av_free_packet(&packet);
    }
    return iRet;
}

int main(int argc, char* argv[])
{

    AVFormatContext	*pFormatCtx;
    int				i, videoindex;
    const AVCodec			*pCodec;
    unsigned char *out_buffer;
    AVPacket *packet;
    AVCodecContext *pCodecCtx;
    SDL_Renderer* sdlRenderer;
    SDL_Texture* sdlTexture;
    int ret;

    //------------SDL----------------
    int screen_w,screen_h;
    SDL_Window *screen;
    SDL_Rect sdlRect;
    SDL_Thread *video_tid;
    SDL_Event event;

    char filepath[]="test.mp4";

    pFormatCtx = avformat_alloc_context();

    if(avformat_open_input(&pFormatCtx,filepath,NULL,NULL)!=0){
        printf("Couldn't open input stream.\n");
        return -1;
    }
    if(avformat_find_stream_info(pFormatCtx,NULL)<0){
        printf("Couldn't find stream information.\n");
        return -1;
    }
    videoindex=-1;
    for(i=0; i<pFormatCtx->nb_streams; i++)
        if (AVMEDIA_TYPE_VIDEO == pFormatCtx->streams[i]->codecpar->codec_type ){
            videoindex=i;
            break;
        }
    if(videoindex==-1){
        printf("Didn't find a video stream.\n");
        return -1;
    }
    pCodec = avcodec_find_decoder(pFormatCtx->streams[videoindex]->codecpar->codec_id);
    if(pCodec==NULL){
        printf("Codec not found.\n");
        return -1;
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);

    if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){
        printf("Could not open codec.\n");
        return -1;
    }
    pFrame=av_frame_alloc();
    pFrameYUV=av_frame_alloc();

    int iWidth = pFormatCtx->streams[videoindex]->codecpar->width;
    int iHeight = pFormatCtx->streams[videoindex]->codecpar->height;
    int iPixFormat = pFormatCtx->streams[videoindex]->codecpar->format;

    out_buffer=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  iWidth, iHeight,1));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
        AV_PIX_FMT_YUV420P, iWidth, iHeight, 1);

    //Output Info-----------------------------
    printf("---------------- File Information ---------------\n");
    av_dump_format(pFormatCtx,0,filepath,0);
    printf("-------------------------------------------------\n");

    img_convert_ctx = sws_getContext(iWidth, iHeight, (AVPixelFormat)iPixFormat,
        iWidth, iHeight, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);


    SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        printf( "Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }
    //SDL 2.0 Support for multiple windows
    screen_w = iWidth;
    screen_h = iHeight;
    screen = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        screen_w, screen_h,SDL_WINDOW_OPENGL);

    if(!screen) {
        printf("SDL: could not create window - exiting:%s\n",SDL_GetError());
        return -1;
    }
    sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
    //IYUV: Y + U + V  (3 planes)
    //YV12: Y + V + U  (3 planes)
    sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,pCodecCtx->width,pCodecCtx->height);

    sdlRect.x=0;
    sdlRect.y=0;
    sdlRect.w=screen_w;
    sdlRect.h=screen_h;

    packet=(AVPacket *)av_malloc(sizeof(AVPacket));

    video_tid = SDL_CreateThread(sfp_refresh_thread,NULL,NULL);
    //------------SDL End------------
    //Event Loop

    for (;;) {
        //Wait
        SDL_WaitEvent(&event);
        if(event.type == SFM_REFRESH_EVENT){
            while(1){
                if(av_read_frame(pFormatCtx, packet)<0)
                    thread_exit=1;

                if(packet->stream_index == videoindex)
                    break;
            }
            ret = Decode(pFormatCtx, pCodecCtx, videoindex, *packet, sdlRenderer, sdlTexture, Refresh);
//            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
            if(ret < 0){
                printf("Decode Error.\n");
                return -1;
            }
//            if(got_picture){
//                sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
//                //SDL---------------------------
//                SDL_UpdateTexture( sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0] );
//                SDL_RenderClear( sdlRenderer );
//                //SDL_RenderCopy( sdlRenderer, sdlTexture, &sdlRect, &sdlRect );
//                SDL_RenderCopy( sdlRenderer, sdlTexture, NULL, NULL);
//                SDL_RenderPresent( sdlRenderer );
//                //SDL End-----------------------
//            }
//            av_free_packet(packet);
        }else if(event.type==SDL_KEYDOWN){
            //Pause
            if(event.key.keysym.sym==SDLK_SPACE)
                thread_pause=!thread_pause;
        }else if(event.type==SDL_QUIT){
            thread_exit=1;
        }else if(event.type==SFM_BREAK_EVENT){
            break;
        }

    }

    sws_freeContext(img_convert_ctx);

    SDL_Quit();
    //--------------
    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    return 0;
}
