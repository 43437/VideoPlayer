#ifndef CRAWDATAOPENGLPLAYER_H
#define CRAWDATAOPENGLPLAYER_H

#include "copenglplayer.h"

class CRawDataOpenglPlayer : public COpenglPlayer
{
public:
    CRawDataOpenglPlayer();

protected:
    virtual void InitFrameData();
    virtual void GetRenderData(uint8_t**& pBuffer);

private:
    FILE            *m_fp;
    uint8_t         *m_pBuf;
    uint8_t         *m_pPlane[3];
};

#endif // CRAWDATAOPENGLPLAYER_H
