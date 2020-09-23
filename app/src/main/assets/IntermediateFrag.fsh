#version 100

precision mediump float;

//Input
in vec3 Normal;
in vec3 FragPos;

//Colors
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;

//Light
uniform vec3 lightPos;

void main() {
    //ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * ambientColor;

    //diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor;

    //final result
    vec3 result = ambient + diffuse;
    gl_FragColor = vec4(result, 1.0);
}