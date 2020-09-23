package com.ctr_fei.bennylocopy;

import android.content.Context;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Vector;

public class Obj {
    private final String TAG = "Obj";

    //Coordinates
    private Vector<float[]> Vertices;
    private Vector<float[]> Normals;
    private Vector<float[]> Textures;

    //Drawing Order
    private Vector<int[]> VertPositions;
    private Vector<int[]> NormPositions;
    private Vector<int[]> TextPositions;

    //Material-Object relationship
    private Vector<Integer> MaterialSet;
    private Vector<String> ListOfMaterialsString;
    private Vector<Integer> ListOfMaterialsInt;
    private HashMap<String, Vector<Integer>>MaterialPosition;
    private Vector<Integer> CheckPoints;
    private HashMap<String, Vector<Float>> NewRelationMOV;
    private HashMap<String, Vector<Float>> NewRelationMON;
    private HashMap<String, Vector<Float>> NewRelationMOT;

    /*Constructor*/
    public Obj(Context context, String file){

        /*Coordinates*/
        Vertices = new Vector<>();
        Normals = new Vector<>();
        Textures = new Vector<>();

        /*Drawing order for each vertex*/
        VertPositions = new Vector<>();
        NormPositions = new Vector<>();
        TextPositions = new Vector<>();

        /*Material order*/
        MaterialSet = new Vector<>();
        ListOfMaterialsString = new Vector<>();
        ListOfMaterialsInt = new Vector<>();
        MaterialPosition = new HashMap<>();
        CheckPoints = new Vector<>();

        /*New material order*/
        NewRelationMOV = new HashMap<>();
        NewRelationMON = new HashMap<>();
        NewRelationMOT = new HashMap<>();

        Vector<Integer> TempMatSet = new Vector<>();

        BufferedReader reader = null;

        int currentMtl = -1;
        int check = 0;

        try{
            InputStreamReader in = new InputStreamReader(context.getAssets().open(file));
            reader = new BufferedReader(in);

            String line;
            String currentName = null;
            boolean readingFaces = false;

            //CheckPoints.add(0);

            while((line = reader.readLine()) != null){

                String[] parts = line.split(" ");

                if (parts[0].equals("v")){
                    if(readingFaces){
                        readingFaces = false; //Stopped reading faces, to go back into reading coordinates
                        MaterialPosition.put(currentName, TempMatSet);
                        TempMatSet.clear();
                    }

                    float[] vertices = new float[3];

                    vertices[0] = Float.valueOf(parts[1]);
                    vertices[1] = Float.valueOf(parts[2]);
                    vertices[2] = Float.valueOf(parts[3]);

                    Vertices.add(vertices);
                    check++;

                }
                else if (parts[0].equals("vn")){
                    float[] normals = new float[3];

                    normals[0] = Float.valueOf(parts[1]);
                    normals[1] = Float.valueOf(parts[2]);
                    normals[2] = Float.valueOf(parts[3]);

                    Normals.add(normals);
                    CheckPoints.add(check);

                }
                else if (parts[0].equals("vt")){

                    float[] textures = new float[2];

                    textures[0] = Float.valueOf(parts[1]);
                    textures[1] = Float.valueOf(parts[2]);

                    Textures.add(textures);

                }
                else if (parts[0].equals("f")){

                    if(!readingFaces){
                        readingFaces = true;
                    }

                    String[][] subparts = StrParser(parts);

                    int[] vertex = ConvertToIntArray(subparts[0]);
                    int[] texture = ConvertToIntArray(subparts[1]);
                    int[] normal = ConvertToIntArray(subparts[2]);

                    VertPositions.add(vertex);
                    TextPositions.add(texture);
                    NormPositions.add(normal);

                    MaterialSet.add(currentMtl);
                    TempMatSet.add(currentMtl);

                    //Vertices drawing order
                    NewRelationMOV.get(currentName).add(Vertices.get(vertex[0]-1)[0]);
                    NewRelationMOV.get(currentName).add(Vertices.get(vertex[0]-1)[1]);
                    NewRelationMOV.get(currentName).add(Vertices.get(vertex[0]-1)[2]);
                    NewRelationMOV.get(currentName).add(Vertices.get(vertex[1]-1)[0]);
                    NewRelationMOV.get(currentName).add(Vertices.get(vertex[1]-1)[1]);
                    NewRelationMOV.get(currentName).add(Vertices.get(vertex[1]-1)[2]);
                    NewRelationMOV.get(currentName).add(Vertices.get(vertex[2]-1)[0]);
                    NewRelationMOV.get(currentName).add(Vertices.get(vertex[2]-1)[1]);
                    NewRelationMOV.get(currentName).add(Vertices.get(vertex[2]-1)[2]);

                    //Normals drawing order
                    NewRelationMON.get(currentName).add(Normals.get(normal[0]-1)[0]);
                    NewRelationMON.get(currentName).add(Normals.get(normal[0]-1)[1]);
                    NewRelationMON.get(currentName).add(Normals.get(normal[0]-1)[2]);
                    NewRelationMON.get(currentName).add(Normals.get(normal[1]-1)[0]);
                    NewRelationMON.get(currentName).add(Normals.get(normal[1]-1)[1]);
                    NewRelationMON.get(currentName).add(Normals.get(normal[1]-1)[2]);
                    NewRelationMON.get(currentName).add(Normals.get(normal[2]-1)[0]);
                    NewRelationMON.get(currentName).add(Normals.get(normal[2]-1)[1]);
                    NewRelationMON.get(currentName).add(Normals.get(normal[2]-1)[2]);

                    //Textures drawing order
                    NewRelationMOT.get(currentName).add(Textures.get(texture[0]-1)[0]);
                    NewRelationMOT.get(currentName).add(Textures.get(texture[0]-1)[1]);
                    NewRelationMOT.get(currentName).add(Textures.get(texture[1]-1)[0]);
                    NewRelationMOT.get(currentName).add(Textures.get(texture[1]-1)[1]);
                    NewRelationMOT.get(currentName).add(Textures.get(texture[2]-1)[0]);
                    NewRelationMOT.get(currentName).add(Textures.get(texture[2]-1)[1]);

                }
                else if(parts[0].equals("usemtl")){
                    if(!checkWordStr(parts[1], ListOfMaterialsString)){
                        currentMtl = ListOfMaterialsInt.size();
                        //Log.d(TAG, "currentMtl value: " + currentMtl);

                        currentName = parts[1];
                        ListOfMaterialsString.add(parts[1]);
                        ListOfMaterialsInt.add(currentMtl);
                        NewRelationMOV.put(currentName, new Vector<Float>());
                        NewRelationMON.put(currentName, new Vector<Float>());
                        NewRelationMOT.put(currentName, new Vector<Float>());
                    }else{
                        currentMtl = checkWordInt(parts[1], ListOfMaterialsString);
                        currentName = parts[1];
                        //Log.d(TAG, "currentMtl value: " + currentMtl);
                    }
                }
            }

        }catch (IOException e){
            Log.d(TAG, "Couldn't load " + file);
            e.printStackTrace();

        }finally{
            Log.d(TAG, file + " was loaded successfully");
            if (reader != null){
                try{
                    reader.close();
                }catch (IOException e){
                    Log.d(TAG, "Couldn't close BufferedReader");
                    e.printStackTrace();
                }
            }
        }
    }

    /*Parsers the face string*/
    private String[][] StrParser(String[] parts){
        String[] vertex = new String[3];
        String[] normal = new String[3];
        String[] texture = new String[3];

        int i = 0;
        boolean first = true;
        for(String subpart : parts){
            if(!first) {
                String[] number = subpart.split("/");

                vertex[i] = number[0];
                texture[i] = number[1];
                normal[i] = number[2];

                i++;
            }else{
                first = false;
            }

        }

        String [][] subparts = new String[3][3];

        subparts[0] = vertex;
        subparts[1] = texture;
        subparts[2] = normal;

        return subparts;
    }

    private int[] ConvertToIntArray(String[] part){
        int[] array = new int[3];

        array[0] = Integer.valueOf(part[0]);
        array[1] = Integer.valueOf(part[1]);
        array[2] = Integer.valueOf(part[2]);

        return array;
    }

    private boolean checkWordStr(String word, Vector<String> Wordlist){
        for(int i = 0; i < Wordlist.size(); i++){
            if(Wordlist.get(i).equals(word)){
                return true;
            }
        }
        return false;
    }

    private int checkWordInt(String word, Vector<String> Wordlist){
        int j = -1;
        for(int i = 0; i < Wordlist.size(); i++){
            if(Wordlist.get(i).equals(word)){
                j = i;
                break;
            }
        }

        return j;
    }

    public Vector<float[]> getVertices() {
        return Vertices;
    }

    public Vector<float[]> getNormals() {
        return Normals;
    }

    public Vector<float[]> getTextures() {
        return Textures;
    }

    public Vector<int[]> getVertPositions() {
        return VertPositions;
    }

    public Vector<int[]> getNormPositions() {
        return NormPositions;
    }

    public Vector<int[]> getTextPositions() {
        return TextPositions;
    }

    public Vector<Integer> getMaterialSet() {
        return MaterialSet;
    }

    public Vector<String> getListOfMaterialsString() {
        return ListOfMaterialsString;
    }

    public HashMap<String, Vector<Integer>> getMaterialPosition() {
        return MaterialPosition;
    }

    public Vector<Integer> getCheckPoints() {
        return CheckPoints;
    }

    public Vector<Integer> getListOfMaterialsInt() {
        return ListOfMaterialsInt;
    }

    public HashMap<String, Vector<Float>> getNewRelationMOV() {
        return NewRelationMOV;
    }

    public HashMap<String, Vector<Float>> getNewRelationMON() {
        return NewRelationMON;
    }

    public HashMap<String, Vector<Float>> getNewRelationMOT() {
        return NewRelationMOT;
    }
}
