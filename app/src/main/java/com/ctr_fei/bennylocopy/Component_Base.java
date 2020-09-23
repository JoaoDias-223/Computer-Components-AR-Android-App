package com.ctr_fei.bennylocopy;

public abstract class Component_Base {

    String name;
    String description;

    //model data
    Obj obj;
    Material mtl;

    //shaders
    Shader vertexShader;
    Shader fragmentShader;

    public abstract boolean draw();
}
