#include "cvideodecoder.h"
#include <iostream>

CVideoDecoder::CVideoDecoder(const std::string& strFile)
{
    m_VideoFileName = strFile;
    Init();
}

void CVideoDecoder::GetPixInfo(int& PixW, int& PixH)
{
    PixW = m_PixelW;
    PixH = m_PixelH;
}

void CVideoDecoder::Init()
{
    m_pFormatCtx = avformat_alloc_context();
    if(avformat_open_input(&m_pFormatCtx, m_VideoFileName.c_str(), NULL, NULL)!=0)
    {
        std::cout << "Couldn't open input stream" << std::endl;
        return;
    }
    if(avformat_find_stream_info(m_pFormatCtx, NULL) < 0)
    {
        printf("Couldn't find stream information.\n");
        return;
    }
    m_iVideoIndex = -1;
    for(int i = 0; i < m_pFormatCtx->nb_streams; ++i)
    {
        if(m_pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            m_iVideoIndex = i;
            break;
        }
    }
    if(m_iVideoIndex == -1)
    {
        std::cout << "Didn't find a video stream" << std::endl;
        return;
    }
    m_pCodec = avcodec_find_decoder(m_pFormatCtx->streams[m_iVideoIndex]->codecpar->codec_id);
    if(m_pCodec == NULL)
    {
        std::cout << "Codec not found." << std::endl;
        return;
    }
    m_pCodecCtx = avcodec_alloc_context3(m_pCodec);
    avcodec_parameters_to_context(m_pCodecCtx, m_pFormatCtx->streams[m_iVideoIndex]->codecpar);
    if( avcodec_open2(m_pCodecCtx, m_pCodec, NULL) < 0 )
    {
        std::cout << "Could not open codec." << std::endl;
        return;
    }
    m_pFrame = av_frame_alloc();
    m_pFrameYUV = av_frame_alloc();
    m_pPacket = (AVPacket *)av_malloc(sizeof(AVPacket));

    m_PixelW = m_pFormatCtx->streams[m_iVideoIndex]->codecpar->width;
    m_PixelH = m_pFormatCtx->streams[m_iVideoIndex]->codecpar->height;
    int iPixFormat = m_pFormatCtx->streams[m_iVideoIndex]->codecpar->format;

    unsigned char *YUVBuffer=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  m_PixelW, m_PixelH, 1));
        av_image_fill_arrays(m_pFrameYUV->data, m_pFrameYUV->linesize, YUVBuffer,
            AV_PIX_FMT_YUV420P, m_PixelW, m_PixelH, 1);

    m_ConvertCtx = sws_getContext(m_PixelW, m_PixelH, (AVPixelFormat)iPixFormat,
        m_PixelW, m_PixelH, AV_PIX_FMT_YUV420P, SWS_BICUBIC, nullptr, nullptr, nullptr);
}

int CVideoDecoder::Decode()
{
    int iRet = 0;
    AVPacket packet;
    if( av_read_frame(m_pFormatCtx, &packet) >= 0 )
    {
        if(packet.stream_index == m_iVideoIndex )
        {
            iRet = avcodec_send_packet(m_pCodecCtx, &packet);
            if (iRet < 0)
            {
                std::cerr << "Error sending a packet for decoding" << std::endl;
                return iRet;
            }

            while (iRet >= 0)
            {
                iRet = avcodec_receive_frame(m_pCodecCtx, m_pFrame);
                if (iRet == AVERROR(EAGAIN) || iRet == AVERROR_EOF)
                    break;
                else if (iRet < 0)
                {
                    std::cerr << "Error during decoding" << std::endl;
                    return iRet;
                }
                else
                {
                    sws_scale(m_ConvertCtx, (const unsigned char* const*)m_pFrame->data, m_pFrame->linesize, 0, m_pFormatCtx->streams[m_iVideoIndex]->codecpar->height
                              , m_pFrameYUV->data, m_pFrameYUV->linesize);
                }
            }
        }
        av_packet_unref(&packet);
    }
    return iRet;
}

void CVideoDecoder::GetDecodedData(unsigned char*& pBuffer, int& iFrameDataPitch)
{
    Decode();
    pBuffer = m_pFrameYUV->data[0];
    iFrameDataPitch = m_pFrameYUV->linesize[0];
}
