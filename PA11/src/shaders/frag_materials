#version 330 core

// Interpolated values from the vertex shaders
in vec3 positionW;
in vec3 normalC;
in vec3 eyeC;
in vec3 spotlightDirC[NUM_SPOT_LIGHTS];
in vec2 uvCoord;
in vec4 ShadowCoord[NUM_SPOT_LIGHTS];

uniform vec3 MaterialDiffuseColor;
uniform vec3 MaterialAmbientColor;
uniform vec3 MaterialSpecularColor;
uniform float shininess;

uniform vec3 AmbientLight;

uniform sampler2D gSampler;
//uniform sampler2DArrayShadow spotlightShadowSampler;
uniform sampler2DArrayShadow spotlightShadowSampler;

uniform vec3 spotLightPositions[NUM_SPOT_LIGHTS];
uniform vec3 spotLightDirections[NUM_SPOT_LIGHTS];
uniform vec3 spotLightColors[NUM_SPOT_LIGHTS];
uniform float spotLightStrengths[NUM_SPOT_LIGHTS];
uniform float spotLightAngles[NUM_SPOT_LIGHTS];

uniform int numShadowSamples;

vec2 poissonDisk[16] = vec2[](
   vec2( -0.94201624, -0.39906216 ),
   vec2( 0.94558609, -0.76890725 ),
   vec2( -0.094184101, -0.92938870 ),
   vec2( 0.34495938, 0.29387760 ),
   vec2( -0.91588581, 0.45771432 ),
   vec2( -0.81544232, -0.87912464 ),
   vec2( -0.38277543, 0.27676845 ),
   vec2( 0.97484398, 0.75648379 ),
   vec2( 0.44323325, -0.97511554 ),
   vec2( 0.53742981, -0.47373420 ),
   vec2( -0.26496911, -0.41893023 ),
   vec2( 0.79197514, 0.19090188 ),
   vec2( -0.24188840, 0.99706507 ),
   vec2( -0.81409955, 0.91437590 ),
   vec2( 0.19984126, 0.78641367 ),
   vec2( 0.14383161, -0.14100790 )
);

/*vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);*/

// Output data
out vec4 frag_color;

void main(void) {
    vec4 MaterialDiffuseTexture2d = texture2D(gSampler, uvCoord.st);

    //If we don't have a texture, default to the materials
    if(MaterialDiffuseTexture2d.rgb == vec3(0.0, 0.0, 0.0)) {
        MaterialDiffuseTexture2d.rgb = MaterialDiffuseColor;
    }

    vec3 materialAmbientModified = (MaterialAmbientColor) * MaterialDiffuseTexture2d.rgb;

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

    vec3 diffuseLight = vec3(0.0, 0.0, 0.0);
    vec3 specularLight = vec3(0.0, 0.0, 0.0);

    float difference; //Difference between the spot light vector and the light vector, for purposes of blending
    float visibility; //How much of the spotlight actually reaches our object - shadows
    float random_dot; //Used for poisson sampling of shadows
    float bias = 0.00075;

    for(int i = 0; i < spotlightDirC.length(); i++) {
        l = normalize(spotlightDirC[i]);
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

        visibility = 0.05;
        for(int j = 0; j < numShadowSamples; j++) {
            visibility += 0.95 / numShadowSamples * texture(spotlightShadowSampler, vec4(ShadowCoord[i].xy + poissonDisk[j] / 3000.0, i, ShadowCoord[i].z - bias));
        }

        if(numShadowSamples == 0) {
            visibility = 1.0;
        }

        diffuseLight += MaterialDiffuseTexture2d.rgb * spotLightColors[i] * spotLightStrengths[i] * cosTheta * difference * visibility / distance / distance;
        specularLight += MaterialSpecularColor * spotLightColors[i] * spotLightStrengths[i] * pow(cosAlpha, shininess) * difference * visibility / distance / distance;
    }

    frag_color.rgb = AmbientLight + materialAmbientModified + diffuseLight + specularLight;
    frag_color.a   = MaterialDiffuseTexture2d.a;
}