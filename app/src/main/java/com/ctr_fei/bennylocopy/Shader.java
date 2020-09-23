package com.ctr_fei.bennylocopy;

import android.content.Context;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class Shader {

    private static final String TAG = "Shader";

    private String mCode = "";

    public Shader(Context context, String file){
        BufferedReader reader = null;

        try{
            InputStreamReader in = new InputStreamReader(context.getAssets().open(file));
            reader = new BufferedReader(in);

            String line;

            while((line = reader.readLine()) != null){
                mCode = mCode.concat(line);
                mCode = mCode.concat("\n");
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
    }

    public String getShaderCode() {
        return mCode;
    }
}
