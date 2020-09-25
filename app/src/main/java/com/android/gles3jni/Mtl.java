package com.android.gles3jni;

import android.content.Context;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Vector;

public class Mtl {
    private final String TAG = "Material";

    private Vector<Material> Materials;
    private HashMap<String, Material> ListOfMaterials;

    private boolean loaded = false;

    public Mtl(Context context, String file){
        Materials = new Vector<>();
        ListOfMaterials = new HashMap<>();

        BufferedReader reader = null;

        int mtl = 0;
        String name = null;
        float[] ambient = null;
        float[] diffuse = null;
        float[] specular = null;
        float specCoef = 0;

        try{
            InputStreamReader in = new InputStreamReader(context.getAssets().open(file));
            reader = new BufferedReader(in);

            String line;
            boolean readingMtl = false;

            while((line = reader.readLine()) != null){
                String[] parts = line.split(" ");

                if(parts[0].equals("newmtl")){
                    if(mtl == 4){
                        Log.d(TAG, "name: " + name + "ambient: " + ambient + "diffuse: " + diffuse + "specular: " + specular);
                        Materials.add(new Material(name, ambient, diffuse, specular, specCoef));
                        ListOfMaterials.put(name, new Material(name, ambient, diffuse, specular, specCoef));
                        mtl = 0;
                    }

                    name = parts[1];

                }else if(parts[0].equals("Ns")){
                    specCoef = Float.valueOf(parts[1]);
                    mtl++;

                }else if(parts[0].equals("Ka")){
                    ambient =  ConvertToFloatArray(parts);
                    mtl++;

                }else if(parts[0].equals("Kd")){
                    diffuse = ConvertToFloatArray(parts);
                    mtl++;

                }else if(parts[0].equals("Ks")){
                    specular =  ConvertToFloatArray(parts);
                    mtl++;
                }
            }

            loaded = true;
        }
        catch (IOException e){
            Log.d(TAG, "Couldn't load '" + file + "'");
            e.printStackTrace();

        }finally{
            if (loaded) Log.d(TAG, "'" + file + "' was loaded successfully");
            if(mtl == 4){
                Materials.add(new Material(name, ambient, diffuse, specular, specCoef));
                ListOfMaterials.put(name, new Material(name, ambient, diffuse, specular, specCoef));
            }
            if(reader != null){
                try{
                    reader.close();
                }catch(IOException e){
                    Log.d(TAG, "Couldn't close reader");
                    e.printStackTrace();
                }
            }
        }
    }

    private float[] ConvertToFloatArray(String[] parts){
        float[] array = new float[3];

        array[0] = Float.valueOf(parts[1]);
        array[1] = Float.valueOf(parts[2]);
        array[2] = Float.valueOf(parts[3]);

        return array;
    }

    public Vector<Material> getMaterials() {
        return Materials;
    }

    public HashMap<String, Material> getListOfMaterials() {
        return ListOfMaterials;
    }
}
