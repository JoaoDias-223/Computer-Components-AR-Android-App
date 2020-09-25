#include "main_simple.h"
#include <typeinfo>

pthread_mutex_t count_mutex;

int width = 1080;
int height = 1920;

float scale = 1.0f;

bool enable_process = true;
bool draw = false;

//all shader related code
Shader shader;
//for video rendering
VideoRenderer videorenderer;

//main camera feed from the Java side
cv::Mat cameraFrame;

int imgWidth = 400;
int imgHeight = 400;

bool setupGraphics(int w, int h) {
    shader.printGLString("Version", GL_VERSION);
    shader.printGLString("Vendor", GL_VENDOR);
    shader.printGLString("Renderer", GL_RENDERER);
    shader.printGLString("Extensions", GL_EXTENSIONS);

    LOGI("setupGraphics(%d, %d)", w, h);

    videorenderer.setup();

    glViewport(0, 0, w, h);
    shader.checkGlError("glViewport");

    width = w;
    height = h;

    //template for the first texture
    cv::Mat frameM(imgHeight, imgWidth, CV_8UC4, cv::Scalar(0,0,0,255));
    videorenderer.initTexture(frameM);
    cameraFrame = frameM;

    //finally we enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return true;
}

void renderFrame() {
    shader.checkGlError("glClearColor");
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.checkGlError("glClear");

    pthread_mutex_lock(&count_mutex);
    cv::Mat frame_local = cameraFrame.clone();
    pthread_mutex_unlock(&count_mutex);

    //render the video feed on the screen
    videorenderer.render(frame_local);
}


//link to internal calls
extern "C" {
JNIEXPORT void JNICALL Java_com_android_gles3jni_GL3JNILib_init(JNIEnv *env, jclass obj, jint width, jint height) {
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_android_gles3jni_GL3JNILib_step(JNIEnv *env, jclass obj) {
    renderFrame();
}

JNIEXPORT void JNICALL Java_com_android_gles3jni_GL3JNILib_toggleFeatures(JNIEnv *env, jclass obj) {
    //toggle the processing on/off
    enable_process = !enable_process;
}

JNIEXPORT void JNICALL Java_com_android_gles3jni_GL3JNILib_setImage(JNIEnv *jenv, jclass obj, jlong imageRGBA) {
    cv::Mat *image = (cv::Mat *) imageRGBA;
    //use mutex lock to ensure the write/read operations are synced (to avoid corrupting the frame)
    pthread_mutex_lock(&count_mutex);
    cameraFrame = image->clone();
    pthread_mutex_unlock(&count_mutex);
    //LOGI("Got Image: %dx%d\n", frame.rows, frame.cols);
}

JNIEXPORT void JNICALL Java_com_android_gles3jni_GL3JNILib_setNewImage(JNIEnv *jenv, jclass obj, jlong imageRGBA) {
    cv::Mat *image = (cv::Mat *) imageRGBA;
    //draw = false;
    pthread_mutex_lock(&count_mutex);
    cameraFrame = image->clone();
    pthread_mutex_unlock(&count_mutex);
    draw= true;
}

JNIEXPORT void JNICALL Java_com_android_gles3jni_GL3JNILib_setWidthAndHeight(JNIEnv *jenv, jclass obj, jint width, jint height) {
    imgWidth = width;
    imgHeight = height;
}

JNIEXPORT jboolean JNICALL Java_com_android_gles3jni_GL3JNILib_isReadyToDraw(JNIEnv *jenv, jclass obj) {
    jboolean isReady = (jboolean) draw;
    return isReady;
}


}
//end of file