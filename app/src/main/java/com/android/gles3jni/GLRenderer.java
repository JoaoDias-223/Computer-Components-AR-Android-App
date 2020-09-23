package com.android.gles3jni;

import android.content.Context;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLRenderer implements GLSurfaceView.Renderer {

    private static final String TAG = "GLRenderer";

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
    float scaleFactor = 0.01f;
    float rotXangle = 0;
    float rotYangle = 0;

    GL3JNIActivity activityRef;

    public GLRenderer(Context context, GL3JNIActivity delegate){
        this.context = context;
        activityRef = delegate;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {

        GLES30.glEnable(GLES30.GL_DEPTH_TEST);
        GLES30.glEnable(GLES30.GL_CULL_FACE);
        GLES30.glCullFace(GLES30.GL_BACK);

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

        if(!activityRef.changeOrient) {
            carro = new Object(context, "chip_centro_certo.obj", "chip_centro_certo.mtl");
            activityRef.CarroSub = carro;
        }
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {

        GL3JNILib.init(width, height);

        GLES30.glViewport(0, 0, width, height);

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

        GL3JNILib.step();


        if(GL3JNILib.isReadyToDraw()) {
            //Log.d(TAG, "onDrawFrame: DRAWING CAR...");
            car_x = GL3JNILib.getCarX();
            car_y = GL3JNILib.getCarY();
            rotXangle = GL3JNILib.getPFProjZero();
            rotYangle = GL3JNILib.getPFProjOne();

            //car_x = (car_x - (400/2.0f));
            //car_y = ((400/2.0f) - car_y);

            Matrix.setIdentityM(mModelMatrix, 0);
            Matrix.translateM(mModelMatrix, 0, 0, 0, car_z);

            Matrix.rotateM(mModelMatrix, 0, rotXangle, 1.0f, 0.0f, 0.0f);
            Matrix.rotateM(mModelMatrix, 0, rotYangle, 0.0f, 1.0f, 0.0f);

            Matrix.multiplyMM(mMVPMatrix, 0, mModelMatrix, 0, mViewMatrix, 0);
            Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mMVPMatrix, 0);

            if (!activityRef.changeOrient) {
                for (String Key : carro.getObj().getListOfMaterialsString()) {
                    carro.Draw(mMVPMatrix, mModelMatrix, Key, scaleFactor);
                    //Log.d(TAG, "onDrawFrame: draw car");
                }

                carro.setFirstRun(false);
            } else {
                for (String Key : activityRef.CarroSub.getObj().getListOfMaterialsString()) {
                    activityRef.CarroSub.Draw(mMVPMatrix, mModelMatrix, Key, scaleFactor);
                    //Log.d(TAG, "onDrawFrame: draw car_sub");
                }

                activityRef.CarroSub.setFirstRun(false);
            }
        }
    }
}
