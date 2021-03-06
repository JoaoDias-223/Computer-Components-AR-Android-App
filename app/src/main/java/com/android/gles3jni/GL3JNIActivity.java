package com.android.gles3jni;

import android.Manifest;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewConfiguration;
import android.view.Window;
import android.view.WindowManager;
import android.widget.RelativeLayout;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Mat;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Locale;

/**
 * Main application for Android
 */
public class GL3JNIActivity extends AppCompatActivity implements CameraBridgeViewBase.CvCameraViewListener2, View.OnTouchListener {

    private static final String TAG = "GL3JNIActivity";
    private static final int PERMISSION_REQUEST_CODE = 200;

    private GL3JNIView mView = null;
    private Context context;
    private float[] mRotationMatrix = new float[16];
    private boolean gl3_loaded = false;
    private CameraBridgeViewBase mOpenCvCameraView;
    private RelativeLayout l_layout;

    public Object CarroSub;
    public boolean changeOrient = false;

    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS: {
                    Log.i(TAG, "OpenCV loaded successfully");

                    System.loadLibrary("main_simple");
                    System.loadLibrary("opencv_java3");
                    System.loadLibrary("c++_shared");

                    gl3_loaded = true;

                    mView = new GL3JNIView(this.mAppContext);
                    l_layout.addView(mView);
                    setContentView(l_layout);

                    mOpenCvCameraView.enableView();
                }
                break;
                default: {
                    super.onManagerConnected(status);
                }
                break;
            }
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "called onCreate");
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.opencv_layout);

        if (checkAllPermissions()) {

            //setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
            l_layout = (RelativeLayout) findViewById(R.id.linearLayoutRest);
            mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.opencv_camera_surface_view);
            mOpenCvCameraView.setOnTouchListener(this);
            mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
            mOpenCvCameraView.setMaxFrameSize(1920, 1080); /* cap it at 720 for performance issue */
            mOpenCvCameraView.setCvCameraViewListener(this);
            mOpenCvCameraView.disableView();

        } else {
            requestAllPermissions();
        }


        /* // A SIMPLER OPTION IF YOU DON'T NEED TO DETECT EACH FINGER
           // OR IF ALL YOU CARE ABOUT IS DETECTING A 2 FINGER PINCH.
        mScaleDetector = new ScaleGestureDetector(this, new MyPinchListener());
        mGestureView.setOnTouchListener(new View.OnTouchListener() {
          @Override
          public boolean onTouch(View v, MotionEvent event) {
            //inspect the event.
            mScaleDetector.onTouchEvent(event);
            return true;
          }
        });
        */

        final ViewConfiguration viewConfig = ViewConfiguration.get(this);
        mViewScaledTouchSlop = viewConfig.getScaledTouchSlop();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (OpenCVLoader.initDebug()) {
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        } else {
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_3_0, this, mLoaderCallback);
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        //stop the camera
        if (mView != null) {
            mView.onPause();
        }
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
        gl3_loaded = false;
    }

    public void onDestroy() {
        super.onDestroy();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    @Override
    public void onCameraViewStarted(int width, int height) {
        Log.d(TAG, "onCameraViewStarted: width = " + width + " height = " + height);
        GL3JNILib.setWidthAndHeight(width, height);
    }

    @Override
    public void onCameraViewStopped() {

    }

    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        //Log.d(TAG, "onCameraFrame: called!");
        Mat input = inputFrame.rgba();
        //if (gl3_loaded) {
          //  GL3JNILib.setNewImage(input.nativeObj);
        //}
        //don't show on the java side
        return null;
    }

    private int mPtrCount = 0;

    private float mPrimStartTouchEventX = -1;
    private float mPrimStartTouchEventY = -1;
    private float mSecStartTouchEventX = -1;
    private float mSecStartTouchEventY = -1;
    private float mPrimSecStartTouchDistance = 0;
    private float mPreviousDistance = 0;
    private float mCurrentDistance = 0;

    private int mViewScaledTouchSlop = 0;

    private ScaleGestureDetector mScaleDetector;

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        int action = (event.getAction() & MotionEvent.ACTION_MASK);

        switch (action) {
            case MotionEvent.ACTION_POINTER_DOWN:
            case MotionEvent.ACTION_DOWN:
                mPtrCount++;
                if (mPtrCount == 1 && mPrimStartTouchEventY == -1 && mPrimStartTouchEventY == -1) {
                    mPrimStartTouchEventX = event.getX(0);
                    mPrimStartTouchEventY = event.getY(0);
                    Log.d("TAG", String.format("POINTER ONE X = %.5f, Y = %.5f", mPrimStartTouchEventX, mPrimStartTouchEventY));
                }
                if (mPtrCount == 2) {
                    // Starting distance between fingers
                    mSecStartTouchEventX = event.getX(1);
                    mSecStartTouchEventY = event.getY(1);
                    mPrimSecStartTouchDistance = distance(event, 0, 1);
                    Log.d("TAG", String.format("POINTER TWO X = %.5f, Y = %.5f", mSecStartTouchEventX, mSecStartTouchEventY));
                }

                break;
            case MotionEvent.ACTION_POINTER_UP:
            case MotionEvent.ACTION_UP:
                mPtrCount--;
                if (mPtrCount < 2) {
                    mSecStartTouchEventX = -1;
                    mSecStartTouchEventY = -1;
                }
                if (mPtrCount < 1) {
                    mPrimStartTouchEventX = -1;
                    mPrimStartTouchEventY = -1;
                }
                break;

            case MotionEvent.ACTION_MOVE:
                boolean isPrimMoving = isScrollGesture(event, 0, mPrimStartTouchEventX, mPrimStartTouchEventY);
                boolean isSecMoving = (mPtrCount > 1 && isScrollGesture(event, 1, mSecStartTouchEventX, mSecStartTouchEventY));

                // There is a chance that the gesture may be a scroll
                if (mPtrCount > 1 && isPinchGesture(event)) {
                    //Log.d("TAG", "PINCH! OUCH!");
                    float distance = (mCurrentDistance - mPreviousDistance)/1000;
                    if (distance < 0.0f){
                        distance = Math.max(-1.0f, distance);
                    }
                    else{
                        distance = Math.min(1.0f, distance);
                    }

                    Log.d(TAG, String.format(Locale.ENGLISH, "Difference: %.5f", distance));
                    mView.renderer.scaleFactor += distance;

                } else if (isPrimMoving || isSecMoving) {
                    // A 1 finger or 2 finger scroll.
                    if (isPrimMoving && isSecMoving) {
                        assert true;
                        //Log.d("TAG", "Two finger scroll");
                    } else {
                        assert true;
                        //Log.d("TAG", "One finger scroll");
                    }
                }
                break;
        }

        return true;
    }

    private boolean isScrollGesture(MotionEvent event, int ptrIndex, float originalX, float originalY){
        float moveX = Math.abs(event.getX(ptrIndex) - originalX);
        float moveY = Math.abs(event.getY(ptrIndex) - originalY);

        if (moveX > mViewScaledTouchSlop || moveY > mViewScaledTouchSlop) {
            return true;
        }
        return false;
    }

    private boolean isPinchGesture(MotionEvent event) {
        if (event.getPointerCount() == 2) {
            //final float distanceCurrent = distance(event, 0, 1);
            mPreviousDistance = mCurrentDistance;
            mCurrentDistance = distance(event, 0, 1);

            if (mPreviousDistance == 0.0f){
                mPreviousDistance = mCurrentDistance;
            }

            final float diffPrimX = mPrimStartTouchEventX - event.getX(0);
            final float diffPrimY = mPrimStartTouchEventY - event.getY(0);
            final float diffSecX = mSecStartTouchEventX - event.getX(1);
            final float diffSecY = mSecStartTouchEventY - event.getY(1);

            if (// if the distance between the two fingers has increased past
                // our threshold
                Math.abs(mCurrentDistance - mPrimSecStartTouchDistance) > mViewScaledTouchSlop

                // and the fingers are moving in opposing directions
                && (diffPrimY * diffSecY) <= 0
                && (diffPrimX * diffSecX) <= 0)
            {
                // mPinchClamp = false; // don't clamp initially

                String msg = String.format(Locale.ENGLISH, "Previous distance: %.4f Current distance: %.4f", mPreviousDistance, mCurrentDistance);
                Log.d(TAG, msg);

                return true;
            }
        }

        return false;
    }

    private float distance(MotionEvent event, int first, int second) {
        if (event.getPointerCount() >= 2) {
            final float x = event.getX(first) - event.getX(second);
            final float y = event.getY(first) - event.getY(second);

            return (float) Math.sqrt(x * x + y * y);
        } else {
            return 0;
        }
    }

    static class MyPinchListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {
        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            Log.d("TAG", "PINCH! OUCH!");
            return true;
        }
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

    /* -------------------------------- Camera Permissions ---------------------------------- */

    private boolean checkAllPermissions() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            return false;
        }
        return true;
    }

    private void requestAllPermissions() {
        ActivityCompat.requestPermissions(this,
                new String[]{Manifest.permission.CAMERA, Manifest.permission.WRITE_EXTERNAL_STORAGE},
                PERMISSION_REQUEST_CODE);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        switch (requestCode) {
            case PERMISSION_REQUEST_CODE:
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {

                    Toast.makeText(getApplicationContext(), TAG + "requested Camera Permission Granted", Toast.LENGTH_SHORT);

                } else {
                    Toast.makeText(getApplicationContext(), TAG + "requested Camera Permission Denied", Toast.LENGTH_SHORT).show();
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED ||
                                ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                            showMessageOKCancel("You need to allow access permissions",
                                    new DialogInterface.OnClickListener() {
                                        @Override
                                        public void onClick(DialogInterface dialog, int which) {
                                            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                                                requestAllPermissions();
                                            }
                                        }
                                    });
                        }
                    }
                }
                break;
        }
    }

    private void showMessageOKCancel(String message, DialogInterface.OnClickListener okListener) {
        new AlertDialog.Builder(GL3JNIActivity.this)
                .setMessage(message)
                .setPositiveButton("OK", okListener)
                .setNegativeButton("Cancel", null)
                .create()
                .show();
    }

}