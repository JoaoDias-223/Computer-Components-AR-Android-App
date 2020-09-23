package com.android.gles3jni;

import android.content.Context;
import android.opengl.GLES30;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.util.HashMap;
import java.util.Vector;

public class Object {

    private final String TAG = "Object";

    private Obj Obj;
    private Material Mtl;

    private int PositionHandle;
    private int NormalHandle;
    private int ModelMatrixHandle;
    private int MVPMatrixHandle;
    private int AmbientHandle;
    private int DiffuseHandle;
    private int SpecularHandle;
    private int LightPosHandle;

    private Shader VertexShader;
    private Shader FragmentShader;
    private int Program;

    private HashMap<String, FloatBuffer> VerticesBufferToMaterial;
    private HashMap<String, FloatBuffer> NormalsBufferToMaterials;
    private HashMap<String, FloatBuffer> TexturesBufferToMaterials;

    private boolean firstRun = true;
    private RunnableFloatBuffer vertBuffer;
    private RunnableFloatBuffer normBuffer;

    private HashMap<String, FloatBuffer> vertMap;
    private HashMap<String, FloatBuffer> normMap;
    private HashMap<String, Integer> vertSizes;

    public Object(Context context, String objName, String mtlName){
        Log.d(TAG, "Called constructor");

        Obj = new Obj(context, objName);
        Mtl = new Material(context, mtlName);

        VertexShader = new Shader (context, "SimpleVert.vsh");
        FragmentShader = new Shader (context, "SimpleFrag.fsh");

        int VertComp = loadShader(GLES30.GL_VERTEX_SHADER, VertexShader.getShaderCode());
        int FragComp = loadShader(GLES30.GL_FRAGMENT_SHADER, FragmentShader.getShaderCode());

        Program = GLES30.glCreateProgram();

        GLES30.glAttachShader(Program, VertComp);
        GLES30.glAttachShader(Program, FragComp);

        GLES30.glLinkProgram(Program);
        GLES30.glValidateProgram(Program);

        vertBuffer = new RunnableFloatBuffer(new float[1], 0);
        normBuffer = new RunnableFloatBuffer(new float[1], 0);

        vertMap = new HashMap<>();
        normMap = new HashMap<>();
        vertSizes = new HashMap<>();
    }

    public void Draw(float[] mvpMatrix, float[] modelMatrix, String Material, float scaleFactor){

        if(firstRun) {
            Log.d(TAG, "Draw: FIRST RUN----------------------------------------------------> " + Material);
            if (this.Obj.getNewRelationMOV().get(Material) != null && this.Obj.getNewRelationMON().get(Material) != null) {
                Mtl Mat = this.Mtl.getListOfMaterials().get(Material);

                //Vertices
                float[] positions = ConvertVectorToArray(this.Obj.getNewRelationMOV().get(Material));
                scale3dModel(scaleFactor, positions);

                //Normals
                float[] normals = ConvertVectorToArray(this.Obj.getNewRelationMON().get(Material));
                scale3dModel(scaleFactor, normals);

                FloatBuffer tempVert = setFBuffer(positions, positions.length);
                FloatBuffer tempNorm = setFBuffer(normals, normals.length);

                normMap.put(Material, tempNorm);
                vertMap.put(Material, tempVert);
                vertSizes.put(Material, positions.length);

                if (Mat != null) {
                    float[] ambient = Mat.getAmbient();
                    float[] diffuse = Mat.getDiffuse();
                    float[] specular = Mat.getSpecular();
                    float[] lightPosition = {1.0f, 0.0f, 0.0f};

                    DrawingPart(Program, ambient, diffuse, specular, lightPosition, tempVert,
                            tempNorm, mvpMatrix, modelMatrix, normals.length);
                }
            }
        }
        else{
            Mtl Mat = this.Mtl.getListOfMaterials().get(Material);

            FloatBuffer tempVert = vertMap.get(Material);
            FloatBuffer tempNorm = normMap.get(Material);
            int size = vertSizes.get(Material);

            if(Mat != null){
                float[] ambient = Mat.getAmbient();
                float[] diffuse = Mat.getDiffuse();
                float[] specular = Mat.getSpecular();
                float[] lightPosition = {1.0f, 0.0f, 0.0f};

                DrawingPart(Program, ambient, diffuse, specular, lightPosition, tempVert,
                        tempNorm, mvpMatrix, modelMatrix, size);
            }
        }
    }

    private void DrawingPart(int program, float[] ambient, float[] diffuse, float[] specular,
                             float[] lightPos, FloatBuffer vertBuffer, FloatBuffer normBuffer,
                             float[] mvpMatrix, float[] modelMatrix, int lengthArray){
        GLES30.glUseProgram(program);

        PositionHandle = GLES30.glGetAttribLocation(Program, "vPosition");
        NormalHandle = GLES30.glGetAttribLocation(Program, "vNormal");
        MVPMatrixHandle = GLES30.glGetUniformLocation(Program, "uMVPMatrix");
        ModelMatrixHandle = GLES30.glGetUniformLocation(Program, "uModelMatrix");
        AmbientHandle = GLES30.glGetUniformLocation(Program, "ambient");
        DiffuseHandle = GLES30.glGetUniformLocation(Program, "diffuse");
        SpecularHandle = GLES30.glGetUniformLocation(Program, "specular");
        LightPosHandle = GLES30.glGetUniformLocation(Program, "lightPos");

        //Set positions
        vertBuffer.position(0);
        GLES30.glEnableVertexAttribArray(PositionHandle);
        GLES30.glVertexAttribPointer(PositionHandle, 3, GLES30.GL_FLOAT, true, 12, vertBuffer);

        //Set normals
        normBuffer.position(0);
        GLES30.glEnableVertexAttribArray(NormalHandle);
        GLES30.glVertexAttribPointer(NormalHandle, 3, GLES30.GL_FLOAT, true, 12, normBuffer);

        //Set light position
        GLES30.glUniform3fv(LightPosHandle, 1, lightPos, 0);

        //Set colors
        GLES30.glUniform3fv(AmbientHandle, 1, ambient, 0);
        GLES30.glUniform3fv(DiffuseHandle, 1, diffuse, 0);
        GLES30.glUniform3fv(SpecularHandle, 1, specular, 0);

        //Set matrices
        GLES30.glUniformMatrix4fv(MVPMatrixHandle, 1, false, mvpMatrix, 0);
        GLES30.glUniformMatrix4fv(ModelMatrixHandle, 1, false, modelMatrix, 0);

        //Draw stuff
        GLES30.glDrawArrays(GLES30.GL_TRIANGLES, 0, lengthArray / 3);

        //Disable vertex attribute arrays
        GLES30.glDisableVertexAttribArray(PositionHandle);
        GLES30.glDisableVertexAttribArray(NormalHandle);
    }

    private float[] ConvertVectorToArray(Vector<Float> Vec){
        float[] array = new float[Vec.size()];

        for(int i = 0; i < Vec.size(); i++){
            array[i] = Vec.get(i);
        }

        return array;
    }

    private FloatBuffer setFBuffer(float[] array, int size){
        //Log.d(TAG, "size = " + size + " array.length = " + array.length);
        ByteBuffer vbb = ByteBuffer.allocateDirect(size * 4);
        vbb.order(ByteOrder.nativeOrder());
        FloatBuffer floatBuffer = vbb.asFloatBuffer();
        floatBuffer.put(array);
        vbb.position(0);
        floatBuffer.position(0);

        return floatBuffer;
    }

    private void scale3dModel(float ScaleFactor, float[] array){
        for (int i = 0; i < array.length; i++){
            array[i] = array[i] * ScaleFactor;
        }
    }

    //Getters

    public Obj getObj() {
        return Obj;
    }

    public Material getMtl() {
        return Mtl;
    }

    public int getPositionHandle() {
        return PositionHandle;
    }

    public int getNormalHandle() {
        return NormalHandle;
    }

    public int getModelMatrixHandle() {
        return ModelMatrixHandle;
    }

    public int getMVPMatrixHandle() {
        return MVPMatrixHandle;
    }

    public int getAmbientHandle() {
        return AmbientHandle;
    }

    public int getDiffuseHandle() {
        return DiffuseHandle;
    }

    public int getSpecularHandle() {
        return SpecularHandle;
    }

    public int getLightPosHandle() {
        return LightPosHandle;
    }

    public Shader getVertexShader() {
        return VertexShader;
    }

    public Shader getFragmentShader() {
        return FragmentShader;
    }

    public int getProgram() {
        return Program;
    }

    public HashMap<String, FloatBuffer> getVerticesBufferToMaterial() {
        return VerticesBufferToMaterial;
    }

    public HashMap<String, FloatBuffer> getNormalsBufferToMaterials() {
        return NormalsBufferToMaterials;
    }

    public HashMap<String, FloatBuffer> getTexturesBufferToMaterials() {
        return TexturesBufferToMaterials;
    }

    public boolean isFirstRun() {
        return firstRun;
    }

    //Setters
    public void setFirstRun(boolean firstRun) {
        this.firstRun = firstRun;
    }

    public static int loadShader(int type, String shaderCode){

        int shader = GLES30.glCreateShader(type);

        GLES30.glShaderSource(shader, shaderCode);
        GLES30.glCompileShader(shader);

        return shader;
    }
}
