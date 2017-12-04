#version 330

layout (location = 0) in vec3 positionM;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normalM;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 modelViewMatrix;

uniform vec3 spotLightPositions[NUM_SPOT_LIGHTS];
uniform vec3 spotLightDirections[NUM_SPOT_LIGHTS];
uniform vec3 spotLightColors[NUM_SPOT_LIGHTS];
uniform float spotLightStrengths[NUM_SPOT_LIGHTS];
uniform float spotLightAngles[NUM_SPOT_LIGHTS];

uniform float shininess;

out vec3 diffuseLight;
out vec3 specularLight;
out vec2 uvCoord;

void main(void) {
    vec4 v = vec4(positionM, 1.0);
    gl_Position = (projectionMatrix * modelViewMatrix) * v;

    vec3 positionW = (modelMatrix * vec4(positionM, 1.0)).xyz;

    vec3 positionC = (modelViewMatrix * vec4(positionM ,1)).xyz;
    vec3 eyeC = vec3(0,0,0) - positionC;

    // Normal of the the vertex, in camera space
    vec3 normalC = (modelViewMatrix * vec4(normalM, 0.0)).xyz;

    uvCoord = uv;

    //All of these are in camera/eye space
    vec3 n = normalize(normalC); //Normal vector to surface at fragment location
    vec3 l;                      //Vector from fragment to light
    vec3 e = normalize(eyeC);    //Vector from fragment to camera
    vec3 r;                      //Perfect reflection vector

    vec3 lightToPosW; //Vector from light to the fragment in world space

    float distance; //Distance from fragment to light

    float cosTheta; //Angle between normal vector and light direction, for diffuse purposes
    float cosAlpha; //Angle between eye vector and reflection vector, for specular purposes
    float cosPhi;   //Angle between light direction and spot light direction, to see if the spotlight is pointing here

    diffuseLight = vec3(0.0, 0.0, 0.0);
    specularLight = vec3(0.0, 0.0, 0.0);

    // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
    vec3 lightPosC;
    vec3 spotlightDirC;
    float difference;
    for(int i = 0; i < spotLightPositions.length(); i++) {
        lightPosC = (viewMatrix * vec4(spotLightPositions[i], 1.0)).xyz;
        spotlightDirC = lightPosC + eyeC;

        l = normalize(spotlightDirC);
        r = reflect(-l, n);

        lightToPosW = positionW - spotLightPositions[i];
        cosPhi = dot(normalize(lightToPosW), spotLightDirections[i]);

        if(cosPhi < spotLightAngles[i]) {
            //Blend the edges
            difference = 1 - (spotLightAngles[i] - cosPhi) * 2 / (1 - spotLightAngles[i]);
            if(difference < 0)
                continue;
        } else {
            difference = 1.0;
        }

        distance = length(lightToPosW);

        cosTheta = max(dot(n, l), 0);
        cosAlpha = max(dot(e, r), 0);

        diffuseLight += spotLightColors[i] * spotLightStrengths[i] * cosTheta * difference / distance / distance;
        specularLight += spotLightColors[i] * spotLightStrengths[i] * pow(cosAlpha, shininess) * difference / distance / distance;
    }
}