#version 100

precision mediump float;

uniform vec3 ambient;
uniform lowp vec3 diffuse;
uniform vec3 specular;

void main() {
    vec3 result = diffuse;
    gl_FragColor = vec4(result.xyz, 1.0);
}