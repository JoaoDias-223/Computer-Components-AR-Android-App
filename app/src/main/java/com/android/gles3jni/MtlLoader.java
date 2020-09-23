package com.android.gles3jni;

import android.content.Context;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class MtlLoader {

    private static final String TAG = "MtlLoader";

    private String[] key;
    private float[] ambient;
    private float[] diffuse;
    private float[] specular;
    private float[] ni;
    private float[] ns;
    private float[] di;
    private int[] illum;

    public MtlLoader(Context context, String fileName){

        BufferedReader reader = null;

        try{
            InputStreamReader in = new InputStreamReader(context.getAssets().open(fileName));
            reader = new BufferedReader(in);

            String line;

            while((line = reader.readLine()) != null){
                String[] parts = line.split(" ");

                if(parts[0].equals("newmtl")){


                }else if(parts[0].equals("Ns")){


                }else if(parts[0].equals("Ka")){


                }else if(parts[0].equals("Kd")){


                }else if(parts[0].equals("Ks")){


                }else if(parts[0].equals("Ni")){


                }else if(parts[0].equals("d")){


                }else if(parts[0].equals("illum")){


                }
            }

        }catch (IOException e){
            Log.i(TAG, "MtlLoader: couldn't read " + fileName);
            e.printStackTrace();
        }
    }

}
