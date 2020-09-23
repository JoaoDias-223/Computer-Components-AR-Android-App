#version 300 es

uniform mat4 uMVPMatrix;
uniform mat4 uModelMatrix;

in vec4 vPosition;
in vec4 vNormal;

out vec3 FragPos;
out vec3 Normal;

void main(){
    FragPos = vec3(uModelMatrix * vPosition);
    Normal = mat3(transpose(inverse(uModelMatrix))) * vNormal.xyz;

    gl_Position = uMVPMatrix * vPosition;
}