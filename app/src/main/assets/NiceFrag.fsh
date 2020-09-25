#version 300 es

precision mediump float;

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 UV;

uniform sampler2D textureSampler;

//uniform vec3 ambient;
//uniform lowp vec3 diffuse;
//uniform vec3 specular;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 objectColor;

vec4 CalculateColor();

void main() {
    vec4 result = CalculateColor();
    vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 red = vec4(0.5, 0.0, 0.0, 1.0);
    FragColor = result;
}

vec4 CalculateColor(){
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;

    vec4 result =   vec4(1.0, 1.0, 1.0, 1.0)
                    //* vec4((ambient + diffuse + specular), 1.0)
                    //* vec4(objectColor, 1.0)
                    * vec4(texture(textureSampler, UV).rgba)
            ;

    return result;
}