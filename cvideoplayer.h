#ifndef CVIDEOPLAYER_H
#define CVIDEOPLAYER_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <SDL2/SDL.h>
#ifdef __cplusplus
};
#endif
#include <string>
#include "csdlplayer.h"
#include "cvideodecoder.h"

class CVideoPlayer : public CSDLPlayer
{
public:
    CVideoPlayer();

protected:
    virtual void InitFrameData();
    virtual void GetRenderData(unsigned char*& pBuffer, int& iFrameDataPitch);

private:
    CVideoDecoder       m_VideoDecoder;
};

#endif // CVIDEOPLAYER_H
