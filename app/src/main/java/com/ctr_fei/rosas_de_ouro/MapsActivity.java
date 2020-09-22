package com.ctr_fei.rosas_de_ouro;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.FragmentActivity;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.location.Location;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Toast;

import com.google.android.gms.location.LocationServices;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.gms.location.FusedLocationProviderClient;

public class MapsActivity extends FragmentActivity implements OnMapReadyCallback {

    String TAG = "Map Activity";

    FloatingActionButton ARButton;

    private FusedLocationProviderClient mFusedLocationProviderClient;
    private Boolean mLocationPermissionGranted = false;
    private GoogleMap mMap;

    private static final String FINE_LOCATION = Manifest.permission.ACCESS_FINE_LOCATION;
    private static final String COARSE_LOCATION = Manifest.permission.ACCESS_COARSE_LOCATION;
    private static final int LOCATION_PERMISSION_REQUEST_CODE = 1234;
    private static final float DEFAULT_ZOOM = 15f;

    private int number_of_landmarks = 0;
    private Marker marker1;
    private Marker marker2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_maps);

        getLocationPermission();

        ARButton = (FloatingActionButton) findViewById(R.id.UnityLauncher_fromFragment);
        ARButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v){
                Toast.makeText(MapsActivity.this, "Opening Unity 3D", Toast.LENGTH_LONG).show();

                Intent unityIntent = new Intent(MapsActivity.this, MiddleActivity.class);
                MapsActivity.this.startActivity(unityIntent);
            }
        });
    }


    /**
     * Manipulates the map once available.
     * This callback is triggered when the map is ready to be used.
     * This is where we can add markers or lines, add listeners or move the camera. In this case,
     * we just add a marker near Sydney, Australia.
     * If Google Play services is not installed on the device, the user will be prompted to install
     * it inside the SupportMapFragment. This method will only be triggered once the user has
     * installed Google Play services and returned to the app.
     */
    @Override
    public void onMapReady(GoogleMap googleMap) {
        Toast.makeText(this, "Map is ready", Toast.LENGTH_SHORT).show();
        Log.d(TAG, "onMapReady: map is ready");
        mMap = googleMap;

        if (mLocationPermissionGranted) {
            getDeviceLocation();

            if(ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION)
            != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(this,
                    Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED){
                return;
            }

            mMap.setMyLocationEnabled(true);
            mMap.getUiSettings().setZoomControlsEnabled(true);

            // Add a marker in Sydney and move the camera
            LatLng carroAlegorico_1 = new LatLng(-23.725358, -46.581336);
            marker1 = mMap.addMarker(new MarkerOptions().position(carroAlegorico_1).title("Carro Alegórico 1"));
            number_of_landmarks += 1;

            mMap.setOnMapClickListener(new GoogleMap.OnMapClickListener() {
                @Override
                public void onMapClick(LatLng latLng) {
                    if (number_of_landmarks < 2) {
                        marker2 = mMap.addMarker(new MarkerOptions().position(latLng).title("Carro Alegórico 2"));
                        number_of_landmarks++;

                    }else if (number_of_landmarks == 2){
                        marker2.remove();
                        marker2 = mMap.addMarker(new MarkerOptions().position(latLng).title("Carro Alegórico 2"));
                    }
                }
            });

        }
    }
    private void getDeviceLocation(){
        Log.d(TAG, "getDeviceLocation: getting the device's current location");

        mFusedLocationProviderClient= LocationServices.getFusedLocationProviderClient(this);

        try{
            if(mLocationPermissionGranted){
                final Task location = mFusedLocationProviderClient.getLastLocation();

                location.addOnCompleteListener(new OnCompleteListener() {
                    @Override
                    public void onComplete(@NonNull Task task) {
                        if(task.isSuccessful()){
                            Log.d(TAG, "onComplete: found location");

                            Location currentLocation = (Location) task.getResult();

                            if (currentLocation != null) {
                                double currentLatitude = currentLocation.getLatitude();
                                double currentLongitude = currentLocation.getLongitude();
                                LatLng LatLngCoords = new LatLng(currentLatitude, currentLongitude);

                                moveCamera(LatLngCoords, DEFAULT_ZOOM);
                            }
                        }else{
                            Log.d(TAG, "onComplete: current location is null");
                            Toast.makeText(MapsActivity.this, "Unable to get current location", Toast.LENGTH_SHORT).show();
                        }
                    }
                });
            }

        }catch (SecurityException e){
            Log.d(TAG, "getDeviceLocation: SecurityException" + e.getMessage());
        }
    }


    private void moveCamera(LatLng latLng, float zoom){
        Log.d(TAG, "moveCamera: moving camera to: (" + latLng.latitude + ", " + latLng.longitude + ")");
        mMap.moveCamera(CameraUpdateFactory.newLatLngZoom(latLng, zoom));
    }

    private void initMap(){
        Log.d(TAG, "initMap: initializing map");
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager().findFragmentById(R.id.map);

        mapFragment.getMapAsync(MapsActivity.this);
    }

    /*Location Permissions*/

    private void getLocationPermission(){
        Log.d(TAG, "getLocationPermission: getting location permission");

        String[] permissions = {Manifest.permission.ACCESS_FINE_LOCATION,
                Manifest.permission.ACCESS_COARSE_LOCATION};

        if(ContextCompat.checkSelfPermission(this.getApplicationContext(),
                FINE_LOCATION) == PackageManager.PERMISSION_GRANTED){
            if(ContextCompat.checkSelfPermission(this.getApplicationContext(), COARSE_LOCATION) == PackageManager.PERMISSION_GRANTED) {
                mLocationPermissionGranted = true;
                initMap();

            }else{
                ActivityCompat.requestPermissions(this, permissions, LOCATION_PERMISSION_REQUEST_CODE);
            }
        }else{
            ActivityCompat.requestPermissions(this, permissions, LOCATION_PERMISSION_REQUEST_CODE);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        Log.d(TAG, "onRequestPermissionsResult: called");

        mLocationPermissionGranted = false;

        switch(requestCode){
            case LOCATION_PERMISSION_REQUEST_CODE:{
                if(grantResults.length > 0){
                    for(int i = 0; i < grantResults.length; i++){
                        if(grantResults[i] != PackageManager.PERMISSION_GRANTED){
                            mLocationPermissionGranted = false;
                            Log.d(TAG, "onRequestPermissionsResult: permissions failed");
                            return;
                        }
                    }
                    Log.d(TAG, "onRequestPermissionsResult: permissions granted");
                    mLocationPermissionGranted = true;
                    initMap();
                }
            }
        }
    }

}
