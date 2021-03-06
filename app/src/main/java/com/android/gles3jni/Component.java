package com.android.gles3jni;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLES30;
import android.util.Log;

import java.nio.FloatBuffer;
import java.util.HashMap;

public class Component extends Component_Base {

    //opengl variables
    private int positionHandle;
    private int normalHandle;
    private int modelMatrixHandle;
    private int MVPMatrixHandle;
    private int ambientHandle;
    private int diffuseHandle;
    private int specularHandle;
    private int lightPosHandle;
    private int program;

    private HashMap<String, FloatBuffer> VerticesBufferToMaterial;
    private HashMap<String, FloatBuffer> NormalsBufferToMaterials;
    private HashMap<String, FloatBuffer> TexturesBufferToMaterials;

    private RunnableFloatBuffer vertBuffer;
    private RunnableFloatBuffer normBuffer;

    private boolean Initialized = false;

    public Component(){}

    public Component (Obj obj, Mtl mtl, Shader vertexShader, Shader fragmentShader){
        this.obj = obj;
        this.mtl = mtl;
        this.vertexShader = vertexShader;
        this.fragmentShader = fragmentShader;

        Setup();
    }

    public Component (Context context, String obj_filename, String mtl_filename,
                      String vertexShader_filename, String fragmentShader_filename)
    {
        this.obj = new Obj(context, obj_filename);
        this.mtl = new Mtl(context, mtl_filename);
        this.vertexShader = new Shader(context, vertexShader_filename);
        this.fragmentShader = new Shader(context, fragmentShader_filename);

        Setup();
    }

    public void Setup(){
        Log.d(name, "called Setup()");
        Initialized = true;

        int vertComp = loadShader(GLES20.GL_VERTEX_SHADER, vertexShader.getShaderCode());
        int fragComp = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentShader.getShaderCode());

        program = GLES20.glCreateProgram();

        GLES20.glAttachShader(program, vertComp);
        GLES20.glAttachShader(program, fragComp);

        GLES20.glLinkProgram(program);
        GLES20.glValidateProgram(program);

        vertBuffer = new RunnableFloatBuffer(new float[1], 0);
        normBuffer = new RunnableFloatBuffer(new float[1], 0);
    }

    @Override
    public boolean draw() {
        if (Initialized){


            return true;
        }

        Log.d(name, "Before drawing, you MUST call Setup() first.");
        return false;
    }

    public static int loadShader(int type, String shaderCode){

        int shader = GLES30.glCreateShader(type);

        GLES30.glShaderSource(shader, shaderCode);
        GLES30.glCompileShader(shader);

        return shader;
    }

    public Obj getObj() {
        return obj;
    }

    public void setObj(Obj obj) {
        this.obj = obj;
    }

    public Mtl getMtl() {
        return mtl;
    }

    public void setMtl(Mtl mtl) {
        this.mtl = mtl;
    }

    public boolean isInitialized() {
        return Initialized;
    }

    public void setInitialized(boolean initialized) {
        Initialized = initialized;
    }
}
