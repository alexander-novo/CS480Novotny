#version 330 core

// Interpolated values from the vertex shaders
in vec3 diffuseLight;
in vec3 specularLight;
in vec2 uvCoord;

uniform vec3 MaterialDiffuseColor;
uniform vec3 MaterialAmbientColor;
uniform vec3 MaterialSpecularColor;

uniform vec3 AmbientLight;

uniform sampler2D gSampler;

// Output data
out vec4 frag_color;

void main(void) {
    vec4 MaterialDiffuseTexture2d = texture2D(gSampler, uvCoord.st);
    vec3 materialAmbientModified = (MaterialAmbientColor) * MaterialDiffuseTexture2d.rgb;

    frag_color.rgb = AmbientLight + materialAmbientModified + diffuseLight * MaterialDiffuseTexture2d.rgb + specularLight * MaterialSpecularColor;
    frag_color.a   = MaterialDiffuseTexture2d.a;
}