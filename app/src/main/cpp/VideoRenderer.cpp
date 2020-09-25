/*
 * VideoRenderer.cpp
 *
 */

#include "VideoRenderer.hpp"
#include <typeinfo>

#define  LOG_TAG    "VideoRenderer"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

VideoRenderer::VideoRenderer() {
    //allocate memory if needed
}

VideoRenderer::~VideoRenderer() {
    //deallocate memory if needed
}

bool VideoRenderer::setup(){
    // Vertex shader source code
    const char g_vshader_code[] =
            "#version 300 es\n"
            "layout(location = 1) in vec4 vPosition;\n"
            "layout(location = 2) in vec2 vertexUV;\n"
            "out vec2 UV;\n"
            "void main() {\n"
            "  gl_Position = vPosition;\n"
            "  UV=vertexUV;\n"
            "}\n";

    // fragment shader source code
    const char g_fshader_code[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "out vec4 color;\n"
            "uniform sampler2D textureSampler;\n"
            "in vec2 UV;\n"
            "void main() {\n"
            "  color = vec4(texture(textureSampler, UV).rgba);\n"
            "}\n";

    LOGI("setupVideoRenderer");
    gProgram = shader.createShaderProgram(g_vshader_code, g_fshader_code);
    if (!gProgram) {
        LOGE("Could not create program.");
        return false;
    }

    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    shader.checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"vPosition\") = %d\n",
         gvPositionHandle);

    vertexUVHandle = glGetAttribLocation(gProgram, "vertexUV");
    shader.checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"vertexUV\") = %d\n",
         vertexUVHandle);

    textureSamplerID = glGetUniformLocation(gProgram, "textureSampler");
    shader.checkGlError("glGetUniformLocation");
    LOGI("glGetUniformLocation(\"textureSampler\") = %d\n",
         textureSamplerID);

    return true;
}

/**
 *
 */
bool VideoRenderer::initTexture(cv::Mat frame){
    texture_id = texture.initializeTexture(frame.data, frame.size().width, frame.size().height);
    //binds our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glUniform1i(textureSamplerID, texture_id);

    LOGI("Renderer Texture ID = %d\n", texture_id);

    return true;
}

void VideoRenderer::render(cv::Mat frame){

    const GLfloat gltestVert[] = {
//            x        y
            -1.0f,   .75f,  0.9999f,   //bottom left   0
            -1.0f,  -.75f,  0.9999f,   //upper left    1
             1.0f,  -.75f,  0.9999f,   //upper right   2
            -1.0f,   .75f,  0.9999f,   //bottom left   0
             1.0f,  -.75f,  0.9999f,   //upper right   2
             1.0f,   .75f,  0.9999f,   //bottom right  3
    };

    const GLfloat gltestUv[] = {
//            x        y
             0.0f,   1.0f,  //0.99f,   //upper left     1
             1.0f,   1.0f,  //0.99f,   //upper right    2
             1.0f,   0.0f,  //0.99f,   //bottom right   3
             0.0f,   1.0f,  //0.99f,   //upper left     1
             1.0f,   0.0f,  //0.99f,   //bottom right   3
             0.0f,   0.0f,  //0.99f,   //bottom left    0
    };

    //LOGI("Renderer Texture ID = %d\n", texture_id);

    glUseProgram(gProgram);
    shader.checkGlError("glUseProgram");

    glEnableVertexAttribArray(gvPositionHandle);
    shader.checkGlError("glEnableVertexAttribArray");

    glEnableVertexAttribArray(vertexUVHandle);
    shader.checkGlError("glEnableVertexAttribArray");

    glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_TRUE, 0, gltestVert);
    shader.checkGlError("glVertexAttribPointer");

    glVertexAttribPointer(vertexUVHandle, 2, GL_FLOAT, GL_TRUE, 0, gltestUv);
    shader.checkGlError("glVertexAttribPointer");

    texture.updateTexture(frame.data, frame.size().width, frame.size().height, GL_RGBA);

    //draw the camera feed on the screen
    glDrawArrays(GL_TRIANGLES, 0, 6);
    shader.checkGlError("glDrawArrays");

    glDisableVertexAttribArray(gvPositionHandle);
    glDisableVertexAttribArray(vertexUVHandle);
}