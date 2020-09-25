package com.android.gles3jni;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES30;
import android.opengl.GLUtils;
import android.util.Log;

import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.HashMap;
import java.util.Vector;

import static android.opengl.GLES30.*;

public class Object {

    private final String TAG = "Object";

    private Obj Obj;
    private com.android.gles3jni.Mtl Mtl;
    private Bitmap texture;

    private int PositionHandle;
    private int NormalHandle;
    private int ModelMatrixHandle;
    private int MVPMatrixHandle;
    private int AmbientHandle;
    private int DiffuseHandle;
    private int SpecularHandle;
    private int LightPosHandle;
    private int LightColorHandle;
    private int ViewPosHandle;
    private int ObjectColorHandle;
    private int TextureSamplerHandle;
    private int TextureCoordinateHandle;
    private int TextureIDHandle;

    private Shader VertexShader;
    private Shader FragmentShader;
    private int Program;

    private boolean firstRun = true;
    private HashMap<String, FloatBuffer> vertMap;
    private HashMap<String, FloatBuffer> normMap;
    private HashMap<String, Integer> vertSizes;
    private HashMap<String, FloatBuffer> texMap;

    public Object(Context context, String objName, String mtlName){
        Log.d(TAG, "Called constructor");

        Obj = new Obj(context, objName);
        Mtl = new Mtl(context, mtlName);

        Log.d(TAG, "OBJ Vertices Keyset: " + Obj.getNewRelationMOV().keySet().toString());
        Log.d(TAG, "OBJ Normal Keyset: " + Obj.getNewRelationMON().keySet().toString());
        Log.d(TAG, "OBJ Texture Keyset: " + Obj.getNewRelationMOT().keySet().toString());

        //printMap(Obj.getNewRelationMOT());

        VertexShader = new Shader (context, "SimpleTexVert.vsh");
        FragmentShader = new Shader (context, "SimpleTexFrag.fsh");

        int VertComp = loadShader(GL_VERTEX_SHADER, VertexShader.getShaderCode());
        int FragComp = loadShader(GL_FRAGMENT_SHADER, FragmentShader.getShaderCode());

        Program = glCreateProgram();

        glAttachShader(Program, VertComp);
        glAttachShader(Program, FragComp);

        glLinkProgram(Program);
        glValidateProgram(Program);

        TextureIDHandle = loadTexture(context, "chip_diffuse.jpg", GL_TEXTURE0);

        vertMap = new HashMap<>();
        normMap = new HashMap<>();
        vertSizes = new HashMap<>();
        texMap = new HashMap<>();
    }

    public void Draw(float[] mvpMatrix, float[] modelMatrix, String Material, float scaleFactor){

        if(firstRun) {
            Log.d(TAG, "Draw: FIRST RUN----------------------------------------------------> " + Material);
            if (this.Obj.getNewRelationMOV().get(Material) != null && this.Obj.getNewRelationMON().get(Material) != null) {
                com.android.gles3jni.Material Mat = this.Mtl.getListOfMaterials().get(Material);

                //Vertices
                float[] positions = ConvertVectorToArray(this.Obj.getNewRelationMOV().get(Material));
                scale3dModel(scaleFactor, positions);

                //Normals
                float[] normals = ConvertVectorToArray(this.Obj.getNewRelationMON().get(Material));
                scale3dModel(scaleFactor, normals);

                float[] textures = ConvertVectorToArray(this.Obj.getNewRelationMOT().get(Material));
                //scale3dModel(scaleFactor, textures);

                printArray(textures, textures.length, 2);

                FloatBuffer tempVert = setFBuffer(positions, positions.length);
                FloatBuffer tempNorm = setFBuffer(normals, normals.length);
                FloatBuffer tempTex = setFBuffer(textures, textures.length);

                normMap.put(Material, tempNorm);
                vertMap.put(Material, tempVert);
                texMap.put(Material, tempTex);
                vertSizes.put(Material, positions.length);

                if (Mat != null) {
                    float[] ambient = Mat.getAmbient();
                    float[] diffuse = Mat.getDiffuse();
                    float[] specular = Mat.getSpecular();
                    float[] lightPosition = {4.0f, 0.0f, 0.0f};

                    /*
                    DrawingPart(Program, ambient, diffuse, specular,
                            lightPosition, tempVert, tempNorm,
                            tempTex, mvpMatrix, modelMatrix,
                            normals.length);

                     */

                    Draw2(Program, tempVert, tempTex, mvpMatrix, normals.length);
                }
            }
        }
        else{
            com.android.gles3jni.Material Mat = this.Mtl.getListOfMaterials().get(Material);

            FloatBuffer tempVert = vertMap.get(Material);
            FloatBuffer tempNorm = normMap.get(Material);
            FloatBuffer tempTex = texMap.get(Material);

            int size = vertSizes.get(Material);

            if(Mat != null){
                float[] ambient = Mat.getAmbient();
                float[] diffuse = Mat.getDiffuse();
                float[] specular = Mat.getSpecular();
                float[] lightPosition = {4.0f, 0.0f, 0.0f};

                /*
                DrawingPart(Program, ambient, diffuse, specular,
                        lightPosition, tempVert, tempNorm,
                        tempTex, mvpMatrix, modelMatrix,
                        size);

                 */

                Draw2(Program, tempVert, tempTex, mvpMatrix, size);
            }
        }
    }

    private void DrawingPart(int program, float[] ambient, float[] diffuse, float[] specular,
                             float[] lightPos, FloatBuffer vertBuffer, FloatBuffer normBuffer,
                             FloatBuffer texBuffer, float[] mvpMatrix, float[] modelMatrix,
                             int lengthArray)
    {
        glUseProgram(program);

        //vertex shader handles
        PositionHandle = glGetAttribLocation(Program, "vPosition");
        NormalHandle = glGetAttribLocation(Program, "vNormal");
        TextureCoordinateHandle = glGetUniformLocation(Program, "vTextureCoordinates");
        MVPMatrixHandle = glGetUniformLocation(Program, "uMVPMatrix");
        ModelMatrixHandle = glGetUniformLocation(Program, "uModelMatrix");

        //fragment shader handles
        AmbientHandle = glGetUniformLocation(Program, "ambient");
        DiffuseHandle = glGetUniformLocation(Program, "diffuse");
        SpecularHandle = glGetUniformLocation(Program, "specular");
        LightPosHandle = glGetUniformLocation(Program, "lightPos");
        LightColorHandle = glGetUniformLocation(Program, "lightColor");
        ViewPosHandle = glGetUniformLocation(Program, "viewPos");
        ObjectColorHandle = glGetUniformLocation(Program, "objectColor");
        TextureSamplerHandle = glGetUniformLocation(Program, "textureSampler");

        //Set vertices position
        vertBuffer.position(0);
        glEnableVertexAttribArray(PositionHandle);
        glVertexAttribPointer(PositionHandle, 3, GL_FLOAT, true, 12, vertBuffer);

        //Set normals position
        normBuffer.position(0);
        glEnableVertexAttribArray(NormalHandle);
        glVertexAttribPointer(NormalHandle, 3, GL_FLOAT, true, 12, normBuffer);

        //Set texture position
        texBuffer.position(0);
        glEnableVertexAttribArray(TextureCoordinateHandle);
        glVertexAttribPointer(TextureCoordinateHandle, 2, GL_FLOAT, true, 8, texBuffer);

        //Set Texture
        bindTexture(TextureIDHandle, GL_TEXTURE0);
        glUniform1i(TextureSamplerHandle, TextureIDHandle);

        //Set light variables
        glUniform3fv(LightPosHandle, 1, lightPos, 0);
        glUniform3fv(LightColorHandle, 1, new float[]{1.0f, 1.0f, 1.0f}, 0);

        //Set camera position
        glUniform3fv(ViewPosHandle, 1, new float[]{0.0f, 0.0f, 0.0f}, 0);

        //Set colors
        glUniform3fv(AmbientHandle, 1, ambient, 0);
        glUniform3fv(DiffuseHandle, 1, diffuse, 0);
        glUniform3fv(SpecularHandle, 1, specular, 0);
        glUniform3fv(ObjectColorHandle, 1, new float[]{.5f, .5f, .5f}, 0);

        //Set matrices
        glUniformMatrix4fv(MVPMatrixHandle, 1, false, mvpMatrix, 0);
        glUniformMatrix4fv(ModelMatrixHandle, 1, false, modelMatrix, 0);

        //Draw stuff
        glDrawArrays(GL_TRIANGLES, 0, lengthArray / 3);

        //Disable vertex attribute arrays
        glDisableVertexAttribArray(PositionHandle);
        glDisableVertexAttribArray(NormalHandle);
        glDisableVertexAttribArray(TextureCoordinateHandle);
    }

    public void Draw2(int program, FloatBuffer vertBuffer, FloatBuffer texBuffer, float[] mvpMatrix, int lengthArray)
    {
        glUseProgram(program);

        PositionHandle = glGetAttribLocation(Program, "vPosition");
        TextureCoordinateHandle = glGetAttribLocation(Program, "vTextureCoordinates");
        MVPMatrixHandle = glGetUniformLocation(Program, "uMVPMatrix");
        TextureSamplerHandle = glGetUniformLocation(Program, "textureSampler");

        glEnableVertexAttribArray(PositionHandle);
        glEnableVertexAttribArray(TextureCoordinateHandle);

        vertBuffer.position(0);
        glVertexAttribPointer(PositionHandle, 3, GL_FLOAT, true, 12, vertBuffer);

        texBuffer.position(0);
        glVertexAttribPointer(TextureCoordinateHandle, 2, GL_FLOAT, true, 8, texBuffer);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureIDHandle);
        glUniform1i(TextureSamplerHandle, TextureIDHandle);

        glUniformMatrix4fv(MVPMatrixHandle, 1, false, mvpMatrix, 0);

        glDrawArrays(GL_TRIANGLES, 0, lengthArray / 3);

        glDisableVertexAttribArray(PositionHandle);
        glDisableVertexAttribArray(TextureCoordinateHandle);
    }

    public static int loadShader(int type, String shaderCode){

        int shader = glCreateShader(type);

        glShaderSource(shader, shaderCode);
        glCompileShader(shader);

        return shader;
    }

    public int loadTexture(final Context context, final String filename, int GL_TEXTURE){

        final int[] textureHandle = new int[1];

        glGenTextures(1, textureHandle, 0);
        glEnable(GL_TEXTURE_2D);

        try {
            InputStream inputStreamer = context.getAssets().open(filename);
            texture = BitmapFactory.decodeStream(inputStreamer);

            // Set filtering
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            GLUtils.texImage2D(GL_TEXTURE_2D, 0, texture, 0);

            bindTexture(textureHandle[0], GL_TEXTURE);

            texture.recycle();

            Log.d(TAG, "Loaded texture '" + filename + "'");
            Log.d(TAG, "Object Texture ID: " + textureHandle[0]);
        }
        catch (Exception e){
            Log.d(TAG, "Error loading '" + filename + "'");
        }

        return textureHandle[0];
    }

    private void bindTexture(int textureHandle, int activeTexture){
        glActiveTexture(activeTexture);
        glBindTexture(GL_TEXTURE_2D, textureHandle);
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

    private void printMap(HashMap< String, Vector<Float> > map){
        StringBuilder msg = new StringBuilder();

        for (String key : map.keySet()){
            Vector<Float> value = map.get(key);
            msg.append('\n');
            msg.append("Length of the vector / 3: ");
            if (value != null){
                msg.append(value.size()/3);
            }
            else{
                msg.append(0);
            }

            msg.append('\n');

            for (int i = 0; value!= null && i < value.size(); i++){
                if ( i % 3 == 0) {
                    msg.append('\n');
                }

                msg.append(Float.toString(value.get(i)));
                msg.append(' ');
            }
        }

        Log.d(TAG, msg.toString());
    }

    private void printArray(float[] arr, int size, int division){
        StringBuilder msg = new StringBuilder();
        msg.append("\n\n");

        for (int i = 0; i < size; i++){
            if (i % division == 0){
                msg.append('\n');
            }
            msg.append(arr[i]);
            msg.append(' ');
        }

        Log.d(TAG, msg.toString());
    }

    //Getters

    public Obj getObj() {
        return Obj;
    }

    public com.android.gles3jni.Mtl getMtl() {
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


    public boolean isFirstRun() {
        return firstRun;
    }

    //Setters
    public void setFirstRun(boolean firstRun) {
        this.firstRun = firstRun;
    }
}
