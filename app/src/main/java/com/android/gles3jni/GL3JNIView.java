package com.android.gles3jni;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.PixelFormat;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.util.Log;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;

import androidx.core.content.ContextCompat;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * A simple application that uses OpenGL ES3 and GLSurface
 */
class GL3JNIView extends GLSurfaceView {

    private static final String TAG = "GL3JNIView";

    public GLRenderer renderer;

    public GL3JNIView(Context context) {
        super(context);
        Log.i(TAG, "GL3JNIView: called");

        // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil
        setZOrderOnTop(true);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        setEGLContextClientVersion(3);
        getHolder().setFormat(PixelFormat.TRANSLUCENT);
        setLayerType(View.LAYER_TYPE_SOFTWARE, null);

        renderer = new GLRenderer(context, (GL3JNIActivity) context);
        setRenderer(renderer);

        //handle gesture input
        Log.i(TAG, "GL3JNIView: successfully constructed!");
    }


    private String FileToString(Context context, String fileName){
        String file = "";

        BufferedReader reader = null;

        try{
            InputStreamReader in = new InputStreamReader(context.getAssets().open(fileName));
            reader = new BufferedReader(in);

            String line;

            while((line = reader.readLine()) != null){
                file = file.concat(line);
                file = file.concat("\n");
            }
        }catch (IOException e){
            Log.d(TAG, "Couldn't open shader file");
            e.printStackTrace();
        }finally {
            Log.i(TAG, "FileToString: " + fileName + " successfully read!");
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException e) {
                    Log.d(TAG, "Couldn't close reader!");
                    e.printStackTrace();
                }
            }
        }

        return file;
    }

}