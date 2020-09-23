#include "main_simple.h"
#include <typeinfo>

pthread_mutex_t count_mutex;

int width = 1280;
int height = 720;

//pre-set image size
const int IMAGE_WIDTH = 1280;
const int IMAGE_HEIGHT = 720;

float scale = 1.0f;
float aspect_ratio=1.0f;
float aspect_ratio_frame=1.0f;

bool enable_process = true;
bool draw = false;
bool initializedGraphics = false;

//all shader related code
Shader shader;
//for video rendering
VideoRenderer videorenderer;

//main camera feed from the Java side
cv::Mat cameraFrame;
cv::Mat frameJava;

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
    aspect_ratio = (float)w/(float)h;

    //PS
    fAspect = (float)w/(float)h;;

    //template for the first texture
    cv::Mat frameM(imgHeight, imgWidth, CV_8UC4, cv::Scalar(0,0,0,255));
    videorenderer.initTexture(frameM);
    cameraFrame = frameM;

    //finally we enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return true;
}


void processFrame(cv::Mat *frame_local){
    int maxCorners = 1000;
    if( maxCorners < 1 ) { maxCorners = 1; }
    cv::RNG rng(12345);
    /// Parameters for Shi-Tomasi algorithm
    std::vector<cv::Point2f> corners;
    double qualityLevel = 0.05;
    double minDistance = 10;
    int blockSize = 3;
    bool useHarrisDetector = false;
    double k = 0.04;

    /// Copy the source image
    cv::Mat src_gray;
    cv::Mat frame_small;
    cv::resize(*frame_local, frame_small, cv::Size(), 0.5, 0.5, CV_INTER_AREA);
    cv::cvtColor(frame_small, src_gray, CV_RGB2GRAY );

    /// Apply feature extraction
    cv::goodFeaturesToTrack( src_gray,
                             corners,
                             maxCorners,
                             qualityLevel,
                             minDistance,
                             cv::Mat(),
                             blockSize,
                             useHarrisDetector,
                             k );

    // Draw corners detected on the image
    int r = 10;
    for( int i = 0; i < corners.size(); i++ )
    {
        cv::circle(*frame_local, 2*corners[i], r, cv::Scalar(rng.uniform(0,255),
                                                             rng.uniform(0,255), rng.uniform(0,255), 255), -1, 8, 0 );
    }
    //LOGI("Found %d features", corners.size());
}


void renderFrame() {
    shader.checkGlError("glClearColor");
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.checkGlError("glClear");

    pthread_mutex_lock(&count_mutex);
    cv::Mat frame_local = cameraFrame.clone();
    pthread_mutex_unlock(&count_mutex);

    if(enable_process)
        //processFrame(&frame_local);

    //render the video feed on the screen
    videorenderer.render(frame_local);

    //LOGI("Rendering OpenGL Graphics");
}

float getCarX(){
    return MODEL_POS.x;
}

float getCarY(){
    return MODEL_POS.y;
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
    frameJava = image->clone();
    cameraFrame = image->clone();
    Timer(frameJava);
    pthread_mutex_unlock(&count_mutex);
    draw= true;
}

JNIEXPORT jfloat JNICALL Java_com_android_gles3jni_GL3JNILib_getCarX(JNIEnv *jenv, jclass obj) {
    jfloat x = (jfloat) getCarX();
    return x;
}

JNIEXPORT jfloat JNICALL Java_com_android_gles3jni_GL3JNILib_getCarY(JNIEnv *jenv, jclass obj) {
    jfloat y = (jfloat) getCarY();
    return y;
}

JNIEXPORT void JNICALL Java_com_android_gles3jni_GL3JNILib_setWidthAndHeight(JNIEnv *jenv, jclass obj, jint width, jint height) {
    imgWidth = width;
    imgHeight = height;
}

JNIEXPORT jboolean JNICALL Java_com_android_gles3jni_GL3JNILib_isReadyToDraw(JNIEnv *jenv, jclass obj) {
    jboolean isReady = (jboolean) draw;
    return isReady;
}

JNIEXPORT jint JNICALL Java_com_android_gles3jni_GL3JNILib_getInitialStatus(JNIEnv *jenv, jclass obj) {
    jint status = (jint) INITIAL_STATUS;

    return status;
}

JNIEXPORT jfloat JNICALL Java_com_android_gles3jni_GL3JNILib_getPFAspectRatio(JNIEnv *jenv, jclass obj){
    jfloat AspectRatio = (jfloat) fAspect;

    return AspectRatio;
}

JNIEXPORT jfloat JNICALL Java_com_android_gles3jni_GL3JNILib_getPFAngle(JNIEnv *jenv, jclass obj){
    jfloat Angle = (jfloat) angle;

    return Angle;
}

JNIEXPORT jfloat JNICALL Java_com_android_gles3jni_GL3JNILib_getPFProjZero(JNIEnv *jenv, jclass obj){
    jfloat num = (jfloat) PROJ[0];

    return num;
}

JNIEXPORT jfloat JNICALL Java_com_android_gles3jni_GL3JNILib_getPFProjOne(JNIEnv *jenv, jclass obj){
    jfloat num = (jfloat) PROJ[1];

    return num;
}

}
//end of file