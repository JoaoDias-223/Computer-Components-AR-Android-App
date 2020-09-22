package com.ctr_fei.rosas_de_ouro;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.app.ActionBar;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.Toast;

import com.google.android.material.floatingactionbutton.FloatingActionButton;

public class MainActivity extends AppCompatActivity{

    FloatingActionButton floatingActionButton;
    FloatingActionButton ARButton;
    String TAG = "Main Activity";
    private static final String CAMERA = Manifest.permission.CAMERA;
    private static final int CAMERA_REQUEST_CODE = 1234;
    boolean mCameraPermission = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        getCameraPermission();

        if(mCameraPermission) {

            Log.d(TAG, "onCreate: Camera permission granted");

            if(ActivityCompat.checkSelfPermission(this, Manifest.permission.CAMERA)
                    != PackageManager.PERMISSION_GRANTED){
                return;
            }

            Toolbar toolbar = findViewById(R.id.ToolbarWidget);
            setSupportActionBar(toolbar);
            getSupportActionBar().setDisplayShowTitleEnabled(false);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
            getSupportActionBar().setIcon(R.mipmap.golden_roses_first_launcher_round);

            floatingActionButton = (FloatingActionButton) findViewById(R.id.openMaps_fab);
            floatingActionButton.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    startActivity(new Intent(MainActivity.this, MapsActivity.class));
                }
            });

            ARButton = (FloatingActionButton) findViewById(R.id.openUnity_fab);
            ARButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Toast.makeText(MainActivity.this, "Opening Unity 3D", Toast.LENGTH_LONG).show();

                    Intent unityIntent = new Intent(MainActivity.this, MiddleActivity.class);
                    MainActivity.this.startActivity(unityIntent);
                }
            });
        }else{
            Log.d(TAG, "onCreate: Camera permission NOT granted");
        }
    }

    private void getCameraPermission(){
        Log.d(TAG, "getCameraPermission: getting camera permisson");

        String[] permissions = {Manifest.permission.CAMERA};

        if(ContextCompat.checkSelfPermission(this.getApplicationContext(), CAMERA)
                == PackageManager.PERMISSION_GRANTED){
            mCameraPermission = true;
        }else{
            ActivityCompat.requestPermissions(this, permissions, CAMERA_REQUEST_CODE);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        Log.d(TAG, "onRequestPermissionResult: called.");

        mCameraPermission = false;

        switch (requestCode){
            case CAMERA_REQUEST_CODE:{
                if(grantResults.length > 0){
                    for(int i = 0; i < grantResults.length; i++){
                        if(grantResults[i] != PackageManager.PERMISSION_GRANTED){
                            mCameraPermission = false;
                            Log.d(TAG, "onRequestPermissionsResult: permissions failed");
                            return;
                        }
                    }
                    Log.d(TAG, "onRequestPermissionsResult: permissions granted");
                    mCameraPermission = true;
                }
            }
        }
    }
}
