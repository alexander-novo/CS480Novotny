#version 330

layout (location = 0) in vec3 positionM;

uniform mat4 MVP;

void main(void) {
    gl_Position = MVP * vec4(positionM, 1.0);
}