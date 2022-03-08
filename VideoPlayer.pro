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

LIBS += -lGL    \
        -lGLEW  \
        -lglfw

SOURCES += \
    copenglplayer.cpp \
    crawdataopenglplayer.cpp \
    crawdataplayer.cpp \
    csdlplayer.cpp \
    cvideoplayer.cpp \
    main.cpp

HEADERS += \
    copenglplayer.h \
    crawdataopenglplayer.h \
    crawdataplayer.h \
    csdlplayer.h \
    cvideoplayer.h

DISTFILES += \
    Shader.fsh \
    Shader.vsh
