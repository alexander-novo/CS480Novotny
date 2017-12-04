#version 330

layout (location = 0) in vec3 positionM;

uniform mat4 MVP;
uniform mat4 M;

out vec3 positionW;

void main(void) {
    gl_Position = MVP * vec4(positionM, 1.0);

    positionW = (M * vec4(positionM, 0.0)).xyz;
}