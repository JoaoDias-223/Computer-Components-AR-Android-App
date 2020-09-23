package com.ctr_fei.bennylocopy;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Camera;
import android.graphics.SurfaceTexture;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLES20;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;
import android.opengl.Matrix;
import android.os.SystemClock;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class J_OpenGLRenderer implements GLSurfaceView.Renderer {

    private final String TAG = "J_OpenGLRenderer";

    private Context mContext;
    private final float[] mvpMatrix = new float[16];//model view projection matrix
    private final float[] projectionMatrix = new float[16];//projection matrix
    private final float[] viewMatrix = new float[16];//view matrix
    private final float[] rotationMatrix = new float[16];//rotation matrix

    private Object CarroAlegorico;
    private Background CameraScene;
    private SurfaceTexture surface;
    int texture;
    MainActivity delegate;
    private float mAngle = 0;

    public J_OpenGLRenderer(MainActivity delegate){
        Log.d(TAG, "Called constructor");
        mContext = delegate.getApplicationContext();
        this.delegate = delegate;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.d(TAG, "onSurfaceCreated");

        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        GLES20.glEnable(GLES20.GL_DEPTH_TEST);
        GLES20.glEnable(GLES20.GL_CULL_FACE);
        GLES20.glCullFace(GLES20.GL_BACK);
        //GLES20.glDepthFunc(GLES20.GL_LESS);
        //GLES20.glFrontFace(GLES20.GL_CCW);

        texture = createTexture();
        CameraScene = new Background(texture, mContext);
        delegate.startCamera(texture);

        CarroAlegorico = new Object(mContext, "carro_alegorico.obj", "carro_alegorico.mtl");
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        GLES20.glViewport(0, 0, width, height);
        float ratio = (float) width / height;
        float left=-ratio,right=ratio;
        Matrix.frustumM(projectionMatrix, 0, left,right, -1.0f, 1.0f, 0.5f, 8.0f);
    }

    @Override
    public void onDrawFrame(GL10 gl) {

        float[] scratch = new float[16];

        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);


        float[] mtx = new float[16];

        surface.updateTexImage();
        surface.getTransformMatrix(mtx);

        //CameraScene.draw();

        if (!CarroAlegorico.isFirstRun()){
            Log.d(TAG, "=========== DESENHANDO CARRO =============");
        }

        for(String Key : CarroAlegorico.getObj().getListOfMaterialsString()){
            CarroAlegorico.Draw(scratch, rotationMatrix, Key, 0.12f);
        }

        CarroAlegorico.setFirstRun(true);
    }

    //---------------------------------------- Utils -----------------------------------------------

    public static int loadShader(int type, String shaderCode){

        int shader = GLES20.glCreateShader(type);

        GLES20.glShaderSource(shader, shaderCode);
        GLES20.glCompileShader(shader);

        return shader;
    }

    static private int createTexture() {
        int[] texture = new int[1];

        GLES20.glGenTextures(1,texture, 0);
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, texture[0]);
        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES,
                GL10.GL_TEXTURE_MIN_FILTER,GL10.GL_LINEAR);
        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES,
                GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES,
                GL10.GL_TEXTURE_WRAP_S, GL10.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES,
                GL10.GL_TEXTURE_WRAP_T, GL10.GL_CLAMP_TO_EDGE);

        return texture[0];
    }

    //--------------------------------------- Setters ----------------------------------------------

    public void setAngle(float mAngle) {
        this.mAngle = mAngle;
    }

    public void setSurface(SurfaceTexture surface) {
        this.surface = surface;
    }

    //--------------------------------------- Setters ----------------------------------------------

    public float getAngle() {
        return mAngle;
    }
}
