#include "copenglplayer.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

static const GLfloat vertexVertices[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f,  0.0f,
    1.0f,  1.0f,  0.0f,
    -1.0f,  1.0f,  0.0f,
};

static const GLfloat textureVertices[] = {
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f,  0.0f,
    0.0f,  0.0f,
};

COpenglPlayer::COpenglPlayer()
{
}

int COpenglPlayer::CreateWindow()
{
    if( !glfwInit() )
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    m_Screen = glfwCreateWindow( 1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
    if( m_Screen == NULL )
    {
        std::cerr << "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials." << std::endl;
        getchar();
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(m_Screen);

    // Initialize GLEW
    glewExperimental = true;        // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(m_Screen, GLFW_STICKY_KEYS, GL_TRUE);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    return 0;
}

bool COpenglPlayer::InitShader(const std::string& shaderFile, GLint shaderID)
{
    bool bRet = false;
    std::ifstream shaderStream(shaderFile, std::ios::in);
    if (shaderStream.is_open())
    {
        std::stringstream svStream;
        svStream << shaderStream.rdbuf();
        std::string strShader = svStream.str();
        shaderStream.close();

        const char* shader = strShader.c_str();
        glShaderSource(shaderID, 1, &shader, NULL);
        //Shader: step3
        glCompileShader(shaderID);
        //Debug
        GLint Result = GL_FALSE;
        int InfoLogLength = 0;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 )
        {
            std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(shaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            std::cerr << "shader [" << shaderID << "] compile " << &VertexShaderErrorMessage[0];
        }
        else
        {
            bRet = true;
        }
    }
    else
    {
        std::cerr << shaderFile << " shader open failed." << std::endl;
    }
    return bRet;
}

void COpenglPlayer::InitTexture()
{
    glUseProgram(m_uiProgram);

    //Get Uniform Variables Location
    m_uiTextureUniformY = glGetUniformLocation(m_uiProgram, "tex_y");
    m_uiTextureUniformU = glGetUniformLocation(m_uiProgram, "tex_u");
    m_uiTextureUniformV = glGetUniformLocation(m_uiProgram, "tex_v");

    //Init Texture
    glGenTextures(1, &m_uiTextureIDY);
    glBindTexture(GL_TEXTURE_2D, m_uiTextureIDY);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &m_uiTextureIDU);
    glBindTexture(GL_TEXTURE_2D, m_uiTextureIDU);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &m_uiTextureIDV);
    glBindTexture(GL_TEXTURE_2D, m_uiTextureIDV);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void COpenglPlayer::CreateProgram(GLint vShaderID, GLint fShaderID)
{
    //Program: Step1
    m_uiProgram = glCreateProgram();
    //Program: Step2
    glAttachShader(m_uiProgram, vShaderID);
    glAttachShader(m_uiProgram, fShaderID);
    //Program: Step3
    glLinkProgram(m_uiProgram);
    //Debug
    GLint Result = GL_FALSE;
    int InfoLogLength = 0;
    glGetShaderiv(m_uiProgram, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(m_uiProgram, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(m_uiProgram, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        std::cerr << "CreateProgram %s" << VertexShaderErrorMessage[0] << std::endl;
    }
}

void COpenglPlayer::InitBufferData()
{
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glGenBuffers(1, &m_uiPosBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_uiPosBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexVertices), vertexVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_uiFragBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_uiFragBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureVertices), textureVertices, GL_STATIC_DRAW);
}

void COpenglPlayer::InitOpenglEnv()
{
    InitBufferData();

    GLint vShaderID, fShaderID;
    vShaderID = glCreateShader(GL_VERTEX_SHADER);
    InitShader("/home/ubuntu/qt-workspace/VideoPlayer/Shader.vsh", vShaderID);

    fShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    InitShader("/home/ubuntu/qt-workspace/VideoPlayer/Shader.fsh", fShaderID);

    CreateProgram(vShaderID, fShaderID);

    InitTexture();
}

void COpenglPlayer::Init()
{
    InitFrameData();
    CreateWindow();
    InitOpenglEnv();
}

void COpenglPlayer::UpdateTexture(uint8_t**& pBuffer)
{
    if (nullptr == pBuffer)
    {
        std::cerr << "UpdateTexture nullptr" << std::endl;
        return;
    }
    glLinkProgram(m_uiProgram);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindBuffer(GL_ARRAY_BUFFER, m_uiPosBuffer);
    glVertexAttribPointer(
    0,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    3,                                // size : U+V => 2
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
    );
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, m_uiFragBuffer);
    glVertexAttribPointer(
    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    2,                                // size : U+V => 2
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
    );
    glEnableVertexAttribArray(1);

    //Y
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_uiTextureIDY);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_iPixelW, m_iPixelH, 0, GL_RED, GL_UNSIGNED_BYTE, pBuffer[0]);
    glUniform1i(m_uiTextureUniformY, 0);

    //U
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_uiTextureIDU);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_iPixelW / 2, m_iPixelH / 2, 0, GL_RED, GL_UNSIGNED_BYTE, pBuffer[1]);
    glUniform1i(m_uiTextureUniformU, 1);

    //V
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_uiTextureIDV);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_iPixelW / 2, m_iPixelH / 2, 0, GL_RED, GL_UNSIGNED_BYTE, pBuffer[2]);
    glUniform1i(m_uiTextureUniformV, 2);

    // Draw
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    // Show
    glfwSwapBuffers(m_Screen);

}

void COpenglPlayer::Run()
{
    do
    {
        uint8_t** pData = nullptr;
        GetRenderData(pData);
        UpdateTexture(pData);

        glfwPollEvents();

    }while( glfwGetKey(m_Screen, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
        glfwWindowShouldClose(m_Screen) == 0 );
}

void COpenglPlayer::Play()
{
    Init();
    Run();
}
