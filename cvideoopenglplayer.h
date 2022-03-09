#ifndef CVIDEOOPENGLPLAYER_H
#define CVIDEOOPENGLPLAYER_H

#include "copenglplayer.h"
#include "cvideodecoder.h"

class CVideoOpenglPlayer : public COpenglPlayer
{
public:
    CVideoOpenglPlayer();

protected:
    virtual void InitFrameData();
    virtual void GetRenderData(uint8_t**& pBuffer);

private:
    CVideoDecoder       m_VideoDecoder;
    uint8_t             *m_pPlane[3];
};

#endif // CVIDEOOPENGLPLAYER_H
