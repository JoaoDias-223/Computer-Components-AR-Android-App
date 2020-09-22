package com.ctr_fei.rosas_de_ouro;

import androidx.appcompat.app.AppCompatActivity;
import android.content.Intent;
import android.os.Bundle;

import androidx.core.app.ActivityCompat;
import androidx.fragment.app.Fragment;

public class MiddleActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        Intent intent = new Intent(MiddleActivity.this, UnityPlayerActivity.class);
        MiddleActivity.this.startActivity(intent);

    }
}
