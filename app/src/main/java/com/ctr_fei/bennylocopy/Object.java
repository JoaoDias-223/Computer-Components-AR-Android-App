package com.ctr_fei.bennylocopy;

import android.content.Context;
import android.opengl.GLES20;
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

    private boolean firstRun = false;
    private RunnableFloatBuffer vertBuffer;
    private RunnableFloatBuffer normBuffer;

    public Object(Context context, String objName, String mtlName){
        Log.d(TAG, "Called constructor");

        Obj = new Obj(context, objName);
        Mtl = new Material(context, mtlName);

        VertexShader = new Shader (context, "SimpleVert.vsh");
        FragmentShader = new Shader (context, "SimpleFrag.fsh");

        int VertComp = J_OpenGLRenderer.loadShader(GLES20.GL_VERTEX_SHADER, VertexShader.getShaderCode());
        int FragComp = J_OpenGLRenderer.loadShader(GLES20.GL_FRAGMENT_SHADER, FragmentShader.getShaderCode());

        Program = GLES20.glCreateProgram();

        GLES20.glAttachShader(Program, VertComp);
        GLES20.glAttachShader(Program, FragComp);

        GLES20.glLinkProgram(Program);
        GLES20.glValidateProgram(Program);

        vertBuffer = new RunnableFloatBuffer(new float[1], 0);
        normBuffer = new RunnableFloatBuffer(new float[1], 0);
    }

    public void Draw(float[] mvpMatrix, float[] modelMatrix, String Material, float scaleFactor){

        if(!firstRun) {
            if (this.Obj.getNewRelationMOV().get(Material) != null && this.Obj.getNewRelationMON().get(Material) != null) {
                Mtl Mat = this.Mtl.getListOfMaterials().get(Material);

                //Vertices
                float[] positions = ConvertVectorToArray(this.Obj.getNewRelationMOV().get(Material));
                vertBuffer.setFloatArray(positions);
                vertBuffer.setScalefactor(scaleFactor);
                vertBuffer.run();

                //Normals
                float[] normals = ConvertVectorToArray(this.Obj.getNewRelationMON().get(Material));
                normBuffer.setFloatArray(normals);
                normBuffer.setScalefactor(scaleFactor);
                normBuffer.run();

                if (Mat != null) {
                    float[] ambient = Mat.getAmbient();
                    float[] diffuse = Mat.getDiffuse();
                    float[] specular = Mat.getSpecular();
                    float[] lightPosition = {1.0f, 0.0f, 0.0f};

                    DrawingPart(Program, ambient, diffuse, specular, lightPosition, vertBuffer.getFloatBuffer(),
                            normBuffer.getFloatBuffer(), mvpMatrix, modelMatrix, vertBuffer.getFloatArray().length);
                }
            }
        }else{
            Mtl Mat = this.Mtl.getListOfMaterials().get(Material);

            //Vertices
            float[] positions = ConvertVectorToArray(this.Obj.getNewRelationMOV().get(Material));
            vertBuffer.setFloatArray(positions);
            vertBuffer.setScalefactor(scaleFactor);
            vertBuffer.run();

            //Normals
            float[] normals = ConvertVectorToArray(this.Obj.getNewRelationMON().get(Material));
            normBuffer.setFloatArray(normals);
            normBuffer.setScalefactor(scaleFactor);
            normBuffer.run();

            if(Mat != null){
                float[] ambient = Mat.getAmbient();
                float[] diffuse = Mat.getDiffuse();
                float[] specular = Mat.getSpecular();
                float[] lightPosition = {1.0f, 0.0f, 0.0f};

                DrawingPart(Program, ambient, diffuse, specular, lightPosition, vertBuffer.getFloatBuffer(),
                        normBuffer.getFloatBuffer(), mvpMatrix, modelMatrix, normBuffer.getFloatArray().length);
            }
        }
    }

    private void DrawingPart(int program, float[] ambient, float[] diffuse, float[] specular,
                             float[] lightPos, FloatBuffer vertBuffer, FloatBuffer normBuffer,
                             float[] mvpMatrix, float[] modelMatrix, int lengthArray){
        GLES20.glUseProgram(program);

        PositionHandle = GLES20.glGetAttribLocation(Program, "vPosition");
        NormalHandle = GLES20.glGetAttribLocation(Program, "vNormal");
        MVPMatrixHandle = GLES20.glGetUniformLocation(Program, "uMVPMatrix");
        ModelMatrixHandle = GLES20.glGetUniformLocation(Program, "uModelMatrix");
        AmbientHandle = GLES20.glGetUniformLocation(Program, "ambient");
        DiffuseHandle = GLES20.glGetUniformLocation(Program, "diffuse");
        SpecularHandle = GLES20.glGetUniformLocation(Program, "specular");
        LightPosHandle = GLES20.glGetUniformLocation(Program, "lightPos");

        //Set positions
        GLES20.glEnableVertexAttribArray(PositionHandle);
        GLES20.glVertexAttribPointer(PositionHandle, 3, GLES20.GL_FLOAT, true, 12, vertBuffer);

        //Set normals
        GLES20.glEnableVertexAttribArray(NormalHandle);
        GLES20.glVertexAttribPointer(NormalHandle, 3, GLES20.GL_FLOAT, true, 12, normBuffer);

        //Set light position
        GLES20.glUniform3fv(LightPosHandle, 1, lightPos, 0);

        //Set colors
        GLES20.glUniform3fv(AmbientHandle, 1, ambient, 0);
        GLES20.glUniform3fv(DiffuseHandle, 1, diffuse, 0);
        GLES20.glUniform3fv(SpecularHandle, 1, specular, 0);

        //Set matrices
        GLES20.glUniformMatrix4fv(MVPMatrixHandle, 1, false, mvpMatrix, 0);
        GLES20.glUniformMatrix4fv(ModelMatrixHandle, 1, false, modelMatrix, 0);

        //Draw stuff
        GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, lengthArray / 3);

        //Disable vertex attribute arrays
        GLES20.glDisableVertexAttribArray(PositionHandle);
        GLES20.glDisableVertexAttribArray(NormalHandle);
    }

    private float[] ConvertVectorToArray(Vector<Float> Vec){
        float[] array = new float[Vec.size()];

        for(int i = 0; i < Vec.size(); i++){
            array[i] = Vec.get(i);
        }

        return array;
    }

    private float[] getCorrectOrder(int start, int end, Obj obj){
        int size = end - start;

        //Log.d(TAG, "start: " + start + " end: " + end);

        float[] vertexPos = new float[size*3];
        int j = 0;

        for(int i = start; i < end; i++){
            vertexPos[j] = obj.getVertices().get(i)[0];
            j++;
            vertexPos[j] = obj.getVertices().get(i)[1];
            j++;
            vertexPos[j] = obj.getVertices().get(i)[2];
            j++;
        }

        return vertexPos;
    }

    private int checkWord(String word, Vector<String> Wordlist){
        for(int i = 0; i < Wordlist.size(); i++){
            if(Wordlist.get(i).equals(word)){
                return i;
            }
        }
        return -1;
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

    private IntBuffer setIBuffer(int[] array, int size){
        ByteBuffer ibb = ByteBuffer.allocateDirect(size * 4);
        ibb.order(ByteOrder.nativeOrder());
        IntBuffer intBuffer = ibb.asIntBuffer();
        intBuffer.put(array);
        ibb.position(0);
        intBuffer.position(0);

        return intBuffer;
    }

    private void scale3dModel(float ScaleFactor, float[] array){
        for (int i = 0; i < array.length; i++){
            array[i] = array[i] * ScaleFactor;
        }
    }

    //Getters

    public com.ctr_fei.bennylocopy.Obj getObj() {
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

    //setter


    public void setFirstRun(boolean firstRun) {
        this.firstRun = firstRun;
    }
}
