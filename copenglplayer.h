#ifndef COPENGLPLAYER_H
#define COPENGLPLAYER_H

// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
// Include GLM
#include <glm/glm.hpp>
#include <string>

class COpenglPlayer
{
public:
    COpenglPlayer();
    virtual void Play();

protected:
    virtual void Init();
    virtual void InitFrameData() = 0;
    virtual void GetRenderData(uint8_t**& pBuffer) = 0;
    virtual void UpdateTexture(uint8_t**& pBuffer);
    virtual void Run();

private:
    int CreateWindow();
    bool InitShader(const std::string& shaderFile, GLint shaderID);
    void CreateProgram(GLint vShaderID, GLint fShaderID);
    void InitTexture();
    void InitBufferData();
    void InitOpenglEnv();

protected:
    GLFWwindow*     m_Screen;
    GLuint          m_uiPosBuffer;
    GLuint          m_uiFragBuffer;
    GLuint          m_uiProgram;
    GLuint          m_uiTextureIDY;
    GLuint          m_uiTextureIDU;
    GLuint          m_uiTextureIDV;             // Texture id
    GLuint          m_uiTextureUniformY;
    GLuint          m_uiTextureUniformU;
    GLuint          m_uiTextureUniformV;
    int             m_iPixelW;
    int             m_iPixelH;
};

#endif // COPENGLPLAYER_H
