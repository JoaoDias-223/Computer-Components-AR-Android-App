package com.ctr_fei.bennylocopy;

import android.content.Context;
import android.graphics.Camera;
import android.graphics.SurfaceTexture;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;

public class J_OpenGLSurfaceView extends GLSurfaceView {

    private static final String TAG = "J_OpenGLSurfaceView";

    J_OpenGLRenderer mRenderer;
    private final float TOUCH_SCALE_FACTOR = 180.0f / 320;
    private float previousX;
    private float previousY;

    public J_OpenGLSurfaceView(Context context) {
        super(context);
        init(context);
    }

    public J_OpenGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    private void init(Context context){
        Log.d(TAG, "called init();");

        setEGLContextClientVersion(2);
        setPreserveEGLContextOnPause(true);

        mRenderer = new J_OpenGLRenderer( (MainActivity) context);
        setRenderer(mRenderer);

        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {

        float x = event.getX();
        float y = event.getY();

        switch (event.getAction()) {
            case MotionEvent.ACTION_MOVE:

                float dx = x - previousX;
                //float dy = y - previousY;


                // reverse direction of rotation above the mid-line
                if (y > getHeight() / 2.0f) {
                    dx = dx * -1 ;
                }


                /*
                // reverse direction of rotation to left of the mid-line
                if (x < getWidth() / 2) {
                    dy = dy * -1 ;
                }
                */
                mRenderer.setAngle(mRenderer.getAngle() + ((dx /*+  dy*/) * TOUCH_SCALE_FACTOR));
                requestRender();
        }

        previousX = x;
        previousY = y;
        return true;
    }

    public J_OpenGLRenderer getRenderer() {
        return mRenderer;
    }
}
