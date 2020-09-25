package com.android.gles3jni;

import android.content.Context;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import static android.opengl.GLES30.*;

public class GLRenderer implements GLSurfaceView.Renderer {

    private static final String TAG = "GLRenderer";

    public static int[] texturesID = new int[20];

    Object carro;
    Context context;

    private final float[] mMVPMatrix = new float[16]; // model view projection matrix
    private final float[] mProjectionMatrix = new float[16]; // World Perspective
    private final float[] mViewMatrix = new float[16]; // Camera Position
    private final float[] mModelMatrix = new float[16]; // Object Position
    private final float[] rotationMatrix = new float[16]; // rotation matrix
    private final float[] mAccumulatedRotation = new float[16]; // Object accumulated rotation
    private final float[] mCurrentRotation = new float[16]; // Object current rotation
    private final float[] mTemporaryMatrix = new float[16];
    private final float[] mLightModelMatrix = new float[16];
    private final float[] mLightPosInModelSpace = new float[] {0.0f, 0.0f, 0.0f, 1.0f}; //Used to hold a light centered on the origin in model space.
    private final float[] mLightPosInWorldSpace = new float[4]; //Used to hold the current position of the light in world space
    private final float[] mLightPosInEyeSpace = new float[4]; //Used to hold the transformed position of the light in eye space

    float car_x = 0.0f;
    float car_y = 0.0f;
    float car_z = -3.5f;
    float scaleFactor = 0.005f;
    float rotXangle = -45;
    float rotYangle = -45;

    GL3JNIActivity activityRef;

    public GLRenderer(Context context, GL3JNIActivity delegate){
        this.context = context;
        activityRef = delegate;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // Position the eye behind the origin.
        final float eyeX = 0.0f;
        final float eyeY = 0.0f;
        final float eyeZ = 0.0f;

        // We are looking toward the distance
        final float lookX = 0.0f;
        final float lookY = 0.0f;
        final float lookZ = -5.0f;

        // Set our up vector. This is where our head would be pointing were we holding the camera.
        final float upX = 0.0f;
        final float upY = 1.0f;
        final float upZ = 0.0f;

        Matrix.setLookAtM(mViewMatrix, 0, eyeX, eyeY, eyeZ, lookX, lookY, lookZ, upX, upY, upZ);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {

        //GL3JNILib.init(width, height);

        if(!activityRef.changeOrient) {
            carro = new Object(context, "chip_centro_certo.obj", "chip_centro_certo.mtl");
            activityRef.CarroSub = carro;
        }

        glViewport(0, 0, width, height);

        final float ratio = (float) width / height;
        final float left = -ratio;
        final float right = ratio;
        final float bottom = -1.0f;
        final float top = 1.0f;
        final float near = 1.0f;
        final float far = 10.0f;

        Matrix.frustumM(mProjectionMatrix, 0, left, right, bottom, top, near, far);

        activityRef.changeOrient = true;
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        glEnable(GL_DEPTH_TEST | GL_TEXTURE_2D);

        //GL3JNILib.step();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //if(GL3JNILib.isReadyToDraw()) {
            Matrix.setIdentityM(mModelMatrix, 0);
            Matrix.translateM(mModelMatrix, 0, 0, 0, car_z);

            //rotXangle = 90;
            //rotYangle = 180;
            rotXangle += 1;
            rotYangle += 1;
            Matrix.rotateM(mModelMatrix, 0, rotXangle, 1.0f, 0.0f, 0.0f);
            Matrix.rotateM(mModelMatrix, 0, rotYangle, 0.0f, 1.0f, 0.0f);

            Matrix.multiplyMM(mMVPMatrix, 0, mModelMatrix, 0, mViewMatrix, 0);
            Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mMVPMatrix, 0);

            if (!activityRef.changeOrient) {
                for (String Key : carro.getObj().getListOfMaterialsString()) {
                    carro.Draw(mMVPMatrix, mModelMatrix, Key, scaleFactor);
                }

                carro.setFirstRun(false);
            } else {
                for (String Key : activityRef.CarroSub.getObj().getListOfMaterialsString()) {
                    activityRef.CarroSub.Draw(mMVPMatrix, mModelMatrix, Key, scaleFactor);
                }

                activityRef.CarroSub.setFirstRun(false);
            }
        //}
    }
}
