#ifndef CRAWDATAPLAYER_H
#define CRAWDATAPLAYER_H

extern "C"{
#include "SDL2/SDL.h"
}
#include <string>
#include "csdlplayer.h"

class CRawDataPlayer : public CSDLPlayer
{
public:
    CRawDataPlayer();

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

protected:
    virtual void InitFrameData();
    virtual void GetRenderData(unsigned char*& pBuffer, int& iFrameDataPitch);

private:
    unsigned char*  m_pBuffer;          //read data
    unsigned char*  m_pBufferConvert;   //convert data, rgb24 & bgr24 need convert to 32bit
    std::string     m_VideoFileName;
    FILE*           m_fp;
    int             m_FrameDataSize;
    int             m_FrameDataPitch;
    EFrameDataType  m_eFrameDataType;
};

#endif // CRAWDATAPLAYER_H
