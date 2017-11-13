#version 330

layout (location = 0) in vec3 positionM;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normalM;

uniform mat4 biasMVP[NUM_SPOT_LIGHTS];

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 modelViewMatrix;

uniform vec3 spotLightPositions[NUM_SPOT_LIGHTS];

out vec3 positionW;
out vec3 normalC;
out vec3 eyeC;
out vec3 spotlightDirC[NUM_SPOT_LIGHTS];
out vec2 uvCoord;
out vec4 ShadowCoord[NUM_SPOT_LIGHTS];

void main(void) {
    vec4 v = vec4(positionM, 1.0);
    gl_Position = (projectionMatrix * modelViewMatrix) * v;

    positionW = (modelMatrix * vec4(positionM, 1.0)).xyz;

    vec3 positionC = (modelViewMatrix * vec4(positionM ,1)).xyz;
    eyeC = vec3(0,0,0) - positionC;

    // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
    vec3 lightPosC;
    for(int i = 0; i < spotLightPositions.length(); i++) {
        lightPosC = (viewMatrix * vec4(spotLightPositions[i], 1.0)).xyz;
        spotlightDirC[i] = lightPosC + eyeC;
        ShadowCoord[i] = biasMVP[i] * v;
        ShadowCoord[i] = ShadowCoord[i] / ShadowCoord[i].w / 2 + vec4(0.5, 0.5, 0.5, 0.5);
    }

    // Normal of the the vertex, in camera space
    normalC = (modelViewMatrix * vec4(normalM, 0.0)).xyz;

    uvCoord = uv;
}