TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += /home/ubuntu/tools/ffmpeg_build/include/

LIBS += -L/home/ubuntu/tools/ffmpeg_build/lib/ -lavcodec \
        -lavdevice \
        -lavfilter \
        -lavformat \
        -lavutil \
        -lswresample \
        -lswscale

LIBS += -lSDL2

SOURCES += \
    crawdataplayer.cpp \
    csdlplayer.cpp \
    cvideoplayer.cpp \
    main.cpp

HEADERS += \
    crawdataplayer.h \
    csdlplayer.h \
    cvideoplayer.h
