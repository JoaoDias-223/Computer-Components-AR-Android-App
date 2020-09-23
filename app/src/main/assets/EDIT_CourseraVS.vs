attribute vec3 aVertexPosition;+uniform mat4 uMVPMatrix;
attribute vec3 aVexterNormal;

uniform vec3 uPointLightingLocation;
uniform vec3 uPointLightingColor;

varying float vDiffuseLightWeighting;
uniform vec3 uAttenuation; // light atenuation
varying float vSpecularLightWeighting;

attribute vec3 aAmbientLight;
attribute vec3 aDiffuseLight;
attribute vec3 aSpecularLight;
attribute float aSpecularExponent;
attribute float aAlpha;

varying vec3 vAmbientLight;
varying vec3 vDiffuseLight;
varying vec4 vSpecularLight;
varying float vAlpha;

void main(){
    gl_Position = uMVPMatrix *vec4(aVertexPosition, 1.0);

    vec4 mvPosition = uMVPMatrix*vec4(aVertexPosition, 1.0);
    vec3 lightDirection = normalize(uPointLightingLocation-mvPosition.xyz);
    vec3 transformedNormal = normalize((uMVPMatrix * vec4(aVertexNormal, 0,0)).xyz);

    vAmbientLight = aAmbientLight;
    vDiffuseLight = aDiffuseLight;
    vSpecularLight = aSpecularLight;
    vAlpha = aAlpha;

    vec3 eyeDirection = normalize(-mvPosition.xyz);

    vec3 reflectionDirection = reflect(-lightDirection, transformedNormal);

    vec3 vertexToLightSource = mvPosition.xyz-uPointLightingLocation;
    float diff_light_dist = length(vertexToLightSource);
    float attenuation = 1.0 / (uAttenuation.x + (uAttenuation.y * diff_light_dist) + (uAttenuation.z * diff_light_dist * diff_light_dist));
    vDiffuseLightWeighting = attenuation * max(dot(transformedNormal, lightDirection), 0.0);

    vSpecularLightWeighting = attenuation * pow(max(dot(reflectionDirection,eyeDirection), 0.0), aSpecularExponent);
}