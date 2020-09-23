package com.ctr_fei.bennylocopy;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

//import android.graphics.Camera;
import android.Manifest;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.hardware.Camera;
import android.graphics.SurfaceTexture;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.TextureView;
import android.widget.Toast;

import java.io.IOException;

public class MainActivity extends AppCompatActivity implements SurfaceTexture.OnFrameAvailableListener {

    private static final String TAG = "MainActivity";

    private static final int PERMISSION_REQUEST_CODE = 200;

    private GLSurfaceView GLView;
    private J_OpenGLSurfaceView OpenGLSurfaceView;
    private Camera mCamera;
    private SurfaceTexture surface;
    private J_OpenGLRenderer renderer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "onCreate: called");

        super.onCreate(savedInstanceState);

        //setContentView(R.layout.activity_main);
        //OpenGLSurfaceView = (J_OpenGLSurfaceView) findViewById(R.id.SurfaceView);
        //GLView = new GLSurfaceView(this);

        OpenGLSurfaceView = new J_OpenGLSurfaceView(this);
        setContentView(OpenGLSurfaceView);
        renderer = OpenGLSurfaceView.getRenderer();
    }


    @Override
    protected void onPause() {
        Log.d(TAG, "onPause: called");
        super.onPause();
        mCamera.stopPreview();
        mCamera.release();
        System.exit(0);
    }

    public void startCamera(int texture) {
        surface = new SurfaceTexture(texture);
        surface.setOnFrameAvailableListener(this);
        renderer.setSurface(surface);

        mCamera = Camera.open();

        try
        {
            mCamera.setPreviewTexture(surface);
            mCamera.startPreview();
        }
        catch (IOException ioe)
        {
            Log.w("MainActivity","CAM LAUNCH FAILED");
        }
    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        OpenGLSurfaceView.requestRender();
    }

    //---------------------------- Camera Permission functions ----------------------------------

    private boolean checkPermission(){
        if(ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA)
                != PackageManager.PERMISSION_GRANTED){
            return false;
        }
        return true;
    }

    private void requestPermission(){
        ActivityCompat.requestPermissions(this,
                new String[]{Manifest.permission.CAMERA},
                PERMISSION_REQUEST_CODE);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        switch(requestCode){
            case PERMISSION_REQUEST_CODE:
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {

                    Toast.makeText(getApplicationContext(), TAG + "requested Camera Permission Granted", Toast.LENGTH_SHORT );

                } else{
                    Toast.makeText(getApplicationContext(), TAG + "requested Camera Permission Denied", Toast.LENGTH_SHORT).show();
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA)
                                != PackageManager.PERMISSION_GRANTED) {
                            showMessageOKCancel("You need to allow access permissions",
                                    new DialogInterface.OnClickListener() {
                                        @Override
                                        public void onClick(DialogInterface dialog, int which) {
                                            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                                                requestPermission();
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
        new AlertDialog.Builder(MainActivity.this)
                .setMessage(message)
                .setPositiveButton("OK", okListener)
                .setNegativeButton("Cancel", null)
                .create()
                .show();
    }

}
