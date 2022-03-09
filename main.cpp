#include <iostream>
#include "crawdataplayer.h"
#include "cvideoplayer.h"
#include "crawdataopenglplayer.h"
#include "cvideoopenglplayer.h"

int main()
{
//    CRawDataPlayer cdp;
//    cdp.Play();

//    CVideoPlayer cp;
//    cp.Play();

//    CRawDataOpenglPlayer cpr;
//    cpr.Play();

    CVideoOpenglPlayer  cvop;
    cvop.Play();

    return 0;
}
